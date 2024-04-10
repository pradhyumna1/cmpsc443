/**********************************************************************

   File          : cmpsc443-refmon.c

   Description   : This file contains the reference monitor command
                   processing code

***********************************************************************/

/* Include Files */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>

/* Project Include Files */
#include "cmpsc443-utilities.h"
#include "cmpsc443-list.h"
#include "cmpsc443-lattice.h"
#include "cmpsc443-refmon.h"

/* Globals */
list system_mapping;
int mic = 0;
int cmdCt = 1;

/* Functional Prototypes */  

/**********************************************************************

    Function    : labelFile
    Description : Compute label (level) for file given process creator (optional)
    Inputs      : fh - output file pointer
                  proc - process name
                  file - file name
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int labelFile( FILE *fh, char *proc, char *file )
{
	char *level;
	elt *new;
	map *proc_map = NULL, *file_map = NULL;

	/* make mapping node for new file */
	map *m = (map *) malloc(sizeof(map));
	m->len = strlen( file );
	m->name = (char *) malloc( m->len );
	strncpy( m->name, file, m->len );

	/* find if a label mapping with filename-prefix */
	elt *fileMatch = get( &label_mapping, file, &matchMapName );
	if ( fileMatch ) 
		file_map = (map *)fileMatch->data;

	/* find label mapping for creating process if any */
	elt *procMatch = ( proc ? get( &system_mapping, proc, matchMapName ) : NULL );
	if ( procMatch ) 
		proc_map = (map *)procMatch->data;

	/* use lowest level of two: creating process and filename-prefix */
	if ( proc_map )
	{
		if ( file_map ) {
			int proc_level = pos( &lattice, proc_map->l, &matchLevel );
			int file_level = pos( &lattice, file_map->l, &matchLevel );
			
			if ( proc_level <= file_level ) {
				m->l = proc_map->l;
			}
			else {
				m->l = file_map->l;
			}
		}
		else {
			m->l = proc_map->l;
		}
	}
	else if ( file_map ) {
			m->l = file_map->l;
	}
	else {
		fprintf( fh, "labelFile[t%d]: no mapping for process: %s\n", cmdCt, proc );
		return -1;
	}

	/* add elt to system mappings */
	new = (elt *) malloc(sizeof(elt));
	new->type = E_MAP;
	new->data = (void *)m;
	insert( &system_mapping, new, NULL );

	/* log result */
	level = (char *) malloc( m->l->len+1 );
	strncpy( level, m->l->name, m->l->len );
	level[m->l->len] = 0;
	fprintf( fh, "labelFile[t%d]: Set label for file %s to %s\n", cmdCt, file, level );
	free( level );

	return 0;		
}

/**********************************************************************

    Function    : labelProcess
    Description : Compute label (level) for process
    Inputs      : fh - output file pointer
                  name - user name or parent process name
                  proc - process name
                  mapping - label or system mapping for finding level
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int labelProcess( FILE *fh, char *name, char *proc, list *mapping )
{
	char *level;
	elt *new;

	/* make mapping node */
	/* YOUR CODE */
	map *m = (map *)malloc(sizeof(map));
	m->len = strlen(proc);
	m->name = (char *)malloc(m->len + 1); // +1 for null terminator
	strncpy(m->name, proc, m->len);
	m->name[m->len] = '\0'; // Ensure nu
	/* use 'name' to determine level:
	   user name use label_mapping 
	   proc name use system_mapping	*/
	/* YOUR CODE */
	elt *nameMatch = get(mapping, name, &matchMapName);
	if (nameMatch) {
		m->l = ((map *)(nameMatch->data))->l;
	}
	else {
		return -1;
	}

	/* add new mapping to system mappings */
	/* YOUR CODE */
	new = (elt *)malloc(sizeof(elt));
	new->type = E_MAP;
	new->data = (void *)m;
	insert(&system_mapping, new, NULL);
	/* log result */
	level = (char *) malloc( m->l->len+1 );
	strncpy( level, m->l->name, m->l->len );
	level[m->l->len] = 0;
	fprintf( fh, "labelProcess[t%d]: Set label for process %s to %s\n", cmdCt, proc, level );
	free( level );

	return 0;
}


/**********************************************************************

    Function    : checkAccess
    Description : Authorize op
    Inputs      : fh - output file pointer
                  proc - name of process
                  file - name of file accessed
                  op - identity of operation (some combination of rwx)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int checkAccess( FILE *fh, char *proc, char *file, int op )
{
	/* get process mapping and file mapping from system_mapping */
	/* YOUR CODE */
        elt *procElt = get(&system_mapping, proc, &matchMapName);
        elt *fileElt = get(&system_mapping, file, &matchMapName);
	/* check whether operation is authorized for those level */
	if ( !procElt || !fileElt ) 
	{
		fprintf( fh, "checkAccess[t%d]: no mapping for proc 0x%p or file 0x%p\n", 
			 cmdCt, procElt, fileElt );
		return -1;
	}
	
	/* extract mapping from elt */
	map *proc_map = (map *)(procElt->data);
	map *file_map = (map *)(fileElt->data);

	/* get level index of process and file for authorization */
	/* YOUR CODE */
       int proc_level = pos(&lattice, proc_map->l, &matchLevel);
       int file_level = pos(&lattice, file_map->l, &matchLevel);

	if (( proc_level < 0 ) || ( file_level < 0 )) {
		fprintf( fh, "checkAccess[t%d]: labeling problem for process %s or file %s\n", 
			 cmdCt, proc, file );
		return -1;
	}

	/* process level must be dominated or equal to file level to read/exec */
	if ( op & (O_READ | O_EXEC) ) {
		if (proc_level > file_level) {  /* can't read */
			if (!( mic && (!( op & O_EXEC )))) { /* still allow for read in mic */
				fprintf( fh, "checkAccess[t%d]: deny %s: process %s for file %s\n", 
					 cmdCt,
					 proc, 
					 (( ( op & ( O_READ | O_EXEC )) == ( O_READ | O_EXEC )) ? "read/exec" : 
					  (( ( op & ( O_READ | O_EXEC )) == O_READ ) ? "read" : "exec")),
					 file );
				return -1;
			}
		}
		/* log result */
		fprintf( fh, "checkAccess[t%d]: OK! proc: %s may %s on file: %s\n", 
			 cmdCt, 
			 proc, 
			 (( ( op & ( O_READ | O_EXEC )) == ( O_READ | O_EXEC )) ? "read/exec" : 
			  (( ( op & ( O_READ | O_EXEC )) == O_READ ) ? "read" : "exec")),
			 file );
	}

	/* file level must be dominated or equal to process level to write */
	if ( op & O_WRITE ) {
		if (file_level > proc_level ) {  /* can't write */
			fprintf( fh, "checkAccess[t%d]: deny write: process %s for file %s\n", 
				 cmdCt, proc, file );
			return -1;
		}
		/* log result */
		fprintf( fh, "checkAccess[t%d]: OK! proc: %s may write file: %s\n", 
			 cmdCt, proc, file );
	}

	return 0;
}

/**********************************************************************

    Function    : checkTrans
    Description : Perform transition of levels if policy requires
    Inputs      : fh - output file pointer
                  proc - name of process
                  file - name of file accessed
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int checkTrans( FILE *fh, char *proc, char *file, int op, int ttype )
{
	/* get process mapping and file mapping from system_mapping */
	/* YOUR CODE */
        elt *procElt = get(&system_mapping, proc, matchMapName);
        elt *fileElt = get(&system_mapping, file, matchMapName);
	/* check whether operation is authorized for those level */
	if ( !procElt || !fileElt ) 
	{
		fprintf( fh, "checkTrans[t%d]: no mapping for proc 0x%p or file 0x%p\n", 
			 cmdCt, procElt, fileElt );
		return -1;
	}

	/* extract mapping from elt */
	map *proc_map = (map *)(procElt->data);
	map *file_map = (map *)(fileElt->data);

	/* build trans objects for current case to see if one exists in trans_mapping */
	trans *t = (trans *)malloc(sizeof(trans));
	t->subj = proc_map->l;
	t->obj = file_map->l;
	int i;
	for ( i = 0; i < MAX_OPS; i++ ) {
		int testop = (1 << i);
		t->op = testop;

		/* if op is applicable, see if trans for that combo of proc, file, op */
		/* then, apply any applicable transition (change proc/file level) */
		if ( op & testop ) {
			/* YOUR CODE */
		        elt *apply = get(&trans_mapping, t, matchTrans);
            		if (apply)
            		{
                		/* apply transition */
                		proc_map->l = ((trans *)(apply->data))->new;
                		file_map->l = ((trans *)(apply->data))->new;

                		/* log result */
                		char *level = (char *)malloc(((trans *)(apply->data))->new->len + 1);
                		strncpy(level, ((trans *)(apply->data))->new->name, ((trans *)(apply->data))->new->len);
                		level[((trans *)(apply->data))->new->len] = 0;
                		fprintf(fh, "checkTrans[t%d]: trans %s %s to %s\n",
                         	cmdCt,
                        	((ttype & T_PROC) ? "PROCESS" : "FILE"),
                        	((ttype & T_PROC) ? proc : file),
                        	level);
                		free(level);
                	}


		}
	}

	free( t );
	return 0;		
}



/**********************************************************************

    Function    : execCmds
    Description : Process exec commands 
    Inputs      : fh - output file pointer
                  data - command data
                  len - length of command data
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int execCmds( FILE *fh, char *data, int len )
{
	char *line;
	char *arg1, *arg2;
	cmdCt = 1;

	/* get line */
	line = strtok( data, "\n" );

	while ( line != NULL )
	{
		//fprintf( fh, "Command: %s\n", line );
		arg1 = (char *) malloc(strlen(line));
		arg2 = (char *) malloc(strlen(line));

		/* perform commands */
		/* label existing files based on name (arg1) */
		if ( sscanf( line, "old-file %s", arg1 ) == 1 )
		{
			labelFile( fh, NULL, arg1 );
		}

		/* label existing files based on name (arg2) or process (arg1) 
		   if no rule for name */
		else if ( sscanf( line, "create %s %s", arg1, arg2 ) == 2 )
		{
			labelFile( fh, arg1, arg2 );
		}

		/* label login process (name in arg2) based on username (arg1) */
		else if ( sscanf( line, "login %s %s", arg1, arg2 ) == 2 )
		{
			labelProcess( fh, arg1, arg2, &label_mapping);
		}

		/* label forked process (name in arg2) based on parent (arg1) */
		else if ( sscanf( line, "fork %s %s", arg1, arg2 ) == 2 )
		{
			labelProcess( fh, arg1, arg2, &system_mapping );
		}

		/* can proc (arg1) exec file (arg2)?  check for transition of proc */
		else if ( sscanf( line, "exec %s %s", arg1, arg2 ) == 2 )
		{
			checkTrans( fh, arg1, arg2, O_EXEC, T_PROC );
			checkAccess( fh, arg1, arg2, O_EXEC );
		}

		/* can proc (arg1) read file (arg2)?  check for transition of proc */
		else if ( sscanf( line, "open-read %s %s", arg1, arg2 ) == 2 )
		{
			checkTrans( fh, arg1, arg2, O_READ, T_PROC );
			checkAccess( fh, arg1, arg2, O_READ );
		}

		/* can proc (arg1) write file (arg2)?  check for transition of file */
		else if ( sscanf( line, "open-write %s %s", arg1, arg2 ) == 2 )
		{
			checkTrans( fh, arg1, arg2, O_WRITE, T_FILE );
			checkAccess( fh, arg1, arg2, O_WRITE );
		}

		/* can proc (arg1) read-write file (arg2)?  check for transitions of file and proc */
		else if ( sscanf( line, "open-rw %s %s", arg1, arg2 ) == 2 )
		{
			checkTrans( fh, arg1, arg2, O_READ, T_PROC );
			checkTrans( fh, arg1, arg2, O_WRITE, T_FILE );
			checkAccess( fh, arg1, arg2, O_READ | O_WRITE );
		}

		/* can proc (arg1) read file (arg2)? */
		else if ( sscanf( line, "read %s %s", arg1, arg2 ) == 2 )
		{
			checkAccess( fh, arg1, arg2, O_READ );
		}

		/* can proc (arg1) write file (arg2)? */
		else if ( sscanf( line, "write %s %s", arg1, arg2 ) == 2 )
		{
			checkAccess( fh, arg1, arg2, O_WRITE );
		}

		else 
		{
//			fprintf( fh, "Command fail %d: %s\n", cmdCt, line );
		}
		
		free(arg1), free(arg2);
		line = strtok( NULL, "\n" );
		cmdCt++;
	}
		
	fprintf( fh, "Commands completed: %d\n", cmdCt );
	return cmdCt;
}



/**********************************************************************

    Function    : policyCmds
    Description : Process policy commands 
    Inputs      : fh - output file pointer
                  data - command data
                  len - length of command data
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int policyCmds( FILE *fh, char *data, int len )
{
	char *line;
	char *arg1, *arg2, *arg3;
	cmdCt = 1;

	/* get line */
	line = strtok( data, "\n" );

	while ( line != NULL )
	{
		arg1 = (char *)malloc(strlen(line));
		arg2 = (char *)malloc(strlen(line));
		arg3 = (char *)malloc(strlen(line));

		/* construct policy */
		/* arg1: lower level; arg2: higher level */
		if ( sscanf( line, "pstate > %s %s", arg1, arg2 ) == 2 )
		{
			addLattice( fh, arg1, arg2 );
		}

		/* arg1: filename-prefix; arg2: level */
		else if ( sscanf( line, "lstate file-old %s %s", arg1, arg2 ) == 2 )
		{
			addLabelPolicy( fh, arg1, arg2 );
		}

		/* arg1: filename-prefix; arg2: level */
		else if ( sscanf( line, "lstate file-new %s %s", arg1, arg2 ) == 2 )
		{
			addLabelPolicy( fh, arg1, arg2 );
		}

		/* arg1: username-prefix; arg2: level */
		else if ( sscanf( line, "lstate user-new %s %s", arg1, arg2 ) == 2 )
		{
			addLabelPolicy( fh, arg1, arg2 );
		}

		/* arg1: subject level; arg2: object level; arg3: resultant level (process) */
		else if ( sscanf( line, "tstate proc-exec %s %s %s", arg1, arg2, arg3 ) == 3 )
		{
			addTransPolicy( fh, arg1, arg2, arg3, O_EXEC, T_PROC );
		}

		/* arg1: subject level; arg2: object level; arg3: resultant level (process) */
		else if ( sscanf( line, "tstate open-read %s %s %s", arg1, arg2, arg3 ) == 3 )
		{
			addTransPolicy( fh, arg1, arg2, arg3, O_READ, T_PROC );
		}

		/* arg1: subject level; arg2: object level; arg3: resultant level (file) */
		else if ( sscanf( line, "tstate open-write %s %s %s", arg1, arg2, arg3 ) == 3 )
		{
			addTransPolicy( fh, arg1, arg2, arg3, O_WRITE, T_FILE );
		}
		
		/* set MIC enforcement - read down is OK */
		else if ( sscanf( line, "policy %s", arg1 ) == 1 )
		{
			mic = 1;
		}

		else 
		{
			fprintf( fh, "Command fail: %s\n", line );
			return -1;
		}

		free(arg1), free(arg2), free(arg3);
		line = strtok( NULL, "\n" );
		cmdCt++;
	}
		
	fprintf( fh, "Command fail: %s\n", line );
	return cmdCt;
}



/**********************************************************************

    Function    : main
    Description : takes requests and processes them
    Inputs      : argc - number of args
                  argv - arg strings
    Outputs     : 0 if successful, -1 if failure

***********************************************************************/

int main( int argc, char *argv[] )
{
	int len;
	char *data;
	FILE *fh;
	int rtn;

	/* initialize maps */
	init( &system_mapping );
	init( &lattice );
	init( &label_mapping );
	init( &trans_mapping );

	if ( argc < 4 ) {
		printf("usage: refmon <policy-file> <exec-file> <outfile>\n");
		exit(-1);
	}

	/* open output file */
	if ( (fh=fopen( argv[3], "w" )) == NULL )
	{
		/* Complain, explain, and exit */
		printf("main: failure opening file [%.64s]\n", argv[3] );
		exit(-1);
	}

	/* get policy commands */
	len = read_from_file( argv[1], &data );

	if ( len <= 0 ) {
		printf("main: failed policy file read: %s\n", 
		       argv[1]);
		goto end;
	}

	/* run policy commands */
	rtn = policyCmds( fh, data, len );
	if ( rtn < 0 )
	{
		/* Complain, explain, and exit */
		printf("main: failure on commands: %s\n", argv[1] );
		goto end;
	}

	/* get exec commands */
	len = read_from_file( argv[2], &data );

	if ( len <= 0 ) {
		printf("main: failed exec file read: %s\n", 
		       argv[2]);
		goto end;
	}

	/* run execution commands */
	rtn = execCmds( fh, data, len );
	if ( rtn < 0 )
	{
		/* Complain, explain, and exit */
		printf("main: failure on commands: %s\n", argv[1] );
		goto end;
	}

end:
	fclose( fh );

	exit((( len <= 0 ) || ( rtn < 0 ) ? -1 : 0));
}




