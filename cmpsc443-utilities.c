/**********************************************************************

   File          : cmpsc443-util.c
   Description   : Utility Functions

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "cmpsc443-utilities.h"

/**********************************************************************

    Function    : errorMessage
    Description : prints an error mesage to stderr
    Inputs      : msg - pointer to string message
    Outputs     : 0 if successful, -1 if failure

***********************************************************************/

int errorMessage( char *msg ) 
{
	/* Print message and return */
	fprintf( stderr, "CMPSC443 Error: %s\n", msg );
	return( 0 );
}


/**********************************************************************

    Function    : write_to_file
    Description : writes content to a file
    Inputs      : fname - file name
                  content - data to write to fname file
                  len - length of content
                  flag - clear or append to file
    Outputs     : 0 if successful, -1 if failure

***********************************************************************/

int write_to_file(char *fname, char *content, int len, unsigned flag )
{
	int fh;
	int outbytes;
	unsigned flag_set = ((flag == FILE_CLEAR) ? 
			     (O_RDWR | O_TRUNC | O_CREAT) :
			     (O_RDWR | O_APPEND));

	if ( (fh=open(fname, flag_set, S_IRUSR | S_IWUSR)) == -1 )
	{
		/* Complain, explain */
		char msg[128];
		sprintf( msg, "failure opening file [%.64s]\n", fname );
		errorMessage( msg );
		return -1;
	}

	outbytes = write( fh, content, len );

	if ( outbytes != len ) {
		/* Complain, explain */
		char msg[128];
		sprintf( msg, "failure writing to file [%.64s]\n", fname );
		errorMessage( msg );
		return -1;
	}

	return 0;
}


/**********************************************************************

    Function    : read_from_file
    Description : read content to a file
    Inputs      : fname - file name
                  content - data to write to fname file
    Outputs     : length if successful, -1 if failure

***********************************************************************/

int read_from_file(char *fname, char **content)
{
	int fh;
	int outbytes, len;
	struct stat statbuf;
	char *buf;

	if ( (fh=open(fname, O_RDONLY, 0)) == -1 )
	{
		/* Complain, explain */
		char msg[128];
		sprintf( msg, "failure opening file [%.64s]\n", fname );
		errorMessage( msg );
		return -1;
	}

	/* get file size */
	fstat(fh, &statbuf);
	outbytes = statbuf.st_size;

	/* read the file */
	buf = *content = (char *) malloc(outbytes+1);
	if ( (len = read(fh, buf, outbytes)) == -1 ) {
		char msg[128];
		sprintf( msg, "failure writing to file [%.64s]\n", fname );
		errorMessage( msg );
		return -1;
	}
	buf[outbytes] = '\0';
	return len;
}
