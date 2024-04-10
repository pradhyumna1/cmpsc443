/**********************************************************************

   File          : cmpsc443-lattice.c

   Description   : This file contains the policy processing commands

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
list lattice;
list label_mapping;
list trans_mapping;

/* Functional Prototypes */  

/**********************************************************************

    Function    : addLattice
    Description : Add levels - add from low to high levels
    Inputs      : fh - output file pointer
                  low - name of lower level
                  high - name of higher level
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int addLattice( FILE *fh, char *low, char *high )
{
	/* normally: add after existing low */	
	elt *lowElt = get( &lattice, low, &matchLevelName );
	
	/* if no low already, must make one */
	if ( !lowElt ) {
		/* make new level for low */
		level *l = (level *) malloc(sizeof(level));
		l->len = strlen( low );
		l->name = (char *) malloc( l->len );
		strncpy( l->name, low, l->len );

		/* make new elt for low in lattice */
		lowElt = (elt *) malloc(sizeof(elt));
		lowElt->type = E_LEVEL;
		lowElt->data = (void *)l;
		lowElt->next = NULL;

		/* add to lattice */
		insert( &lattice, lowElt, NULL );
		fprintf( fh, "Add Lattice[p%d]: level: %s \n", cmdCt, low );
	}

	/* make new level for high */
	level *l = (level *) malloc(sizeof(level));
	l->len = strlen( high );
	l->name = (char *) malloc( l->len );
	strncpy( l->name, high, l->len );

	/* make new elt for high in lattice */
	elt *new = (elt *) malloc(sizeof(elt));
	new->type = E_LEVEL;
	new->data = (void *)l;
	new->next = NULL;

	/* add to lattice */
	insert( &lattice, new, lowElt );
	fprintf( fh, "Add Lattice[p%d]: level: %s > %s \n", cmdCt, high, low );
	
	return 0;
}


/**********************************************************************

    Function    : addLabelPolicy
    Description : Add mapping from name to level
    Inputs      : fh - output file pointer
                  name - name (prefix) of objects at level
                  level - name of level
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int addLabelPolicy( FILE *fh, char *name, char *level )
{
	elt *new, *levelElt;

	/* make mapping node */
	map *m = (map *) malloc(sizeof(map));
	m->len = strlen( name );
	m->name = (char *) malloc( m->len );
	strncpy( m->name, name, m->len );

	/* find lattice level for map */
	levelElt = NULL;    	/* YOUR CODE */
	if ( !levelElt ) {
		fprintf( fh, "AddLabelPolicy[p%d]: can't find level %s\n", cmdCt, level );	
		return -1;
	}

	/* add mapping between name and level */
	/* YOUR CODE */

	/* create elt for label_mapping */
	/* YOUR CODE */

    /* add to label mappings */ 
	/* YOUR CODE */
	
	fprintf( fh, "AddLabelPolicy[p%d]: Add mapping for name %s to level %s\n", 
		 cmdCt, name, level );	
	return 0;
}


/**********************************************************************

    Function    : addTransPolicy
    Description : Add mapping from name to level
    Inputs      : fh - output file pointer
                  subj_level, 
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int addTransPolicy( FILE *fh, char *subj_level, char *obj_level, char* new_level, 
		    int op, int ttype )
{
	elt *new;

	/* make trans node */
	trans *t = (trans *) malloc(sizeof(trans));
	t->op = op;

	/* get subject, object, new levels */
	elt *subjElt = get( &lattice, subj_level, &matchLevelName );
	t->subj = (level *)subjElt->data;
	elt *objElt = get( &lattice, obj_level, &matchLevelName );
	t->obj = (level *)objElt->data;
	elt *newElt = get( &lattice, new_level, &matchLevelName );
	t->new = (level *)newElt->data;
	
	/* check that all levels were found */
	if ( !t->subj || !t->obj || !t->new ) {
		fprintf( fh, "AddTransPolicy[p%d]: No level for mapping to %s(0x%p):%s(0x%p):%s(0x%p)\n", 
			 cmdCt, subj_level, t->subj, obj_level, t->obj, new_level, t->new );
		return -1;
	}

	/* add elt to mapping list */
	new = (elt *) malloc(sizeof(elt));
	new->type = E_TRANS;
	new->data = (void *)t;
	insert( &trans_mapping, new, NULL );

	fprintf( fh, "AddTransPolicy[p%d]: on %s, if subject level is %s and object level is %s, \n\t then transition %s to level %s\n", 
		 cmdCt,
		 (( op == O_EXEC ) ? "EXEC" : (( op == O_READ ) ? "READ" : "WRITE" )),
		 subj_level, obj_level, 
		 (( ttype == T_PROC ) ? "PROCESS" : "FILE"),
		 new_level );
	return 0;
}


/* lattice matching functions */

/**********************************************************************

    Function    : matchName
    Description : Find if node name matches supplied name
    Inputs      : elt - element to check 
                  obj - obj to match on 
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int matchLevelName( elt *e, void *obj )
{
	if ( strncmp( (char *)obj, ((level *)(e->data))->name, ((level *)(e->data))->len ) == 0 ) 
		return 1;
	return 0;
}


int matchMapName( elt *e, void *obj )
{
	/* 
	if ( strncmp( (char *)obj, ((map *)(e->data))->name, ((map *)(e->data))->len ) == 0 ) 
		return 1;
	return 0;
	*/
	return( strncmp( (char *)obj, ((map *)(e->data))->name, ((map *)(e->data))->len ) == 0 );
}


int matchLevel( elt *e, void *obj )
{
	return( ((level *)(e->data)) == (level *)obj );
}


int matchTrans( elt *e, void *obj )
{
	trans *t1, *t2;
	t1 = (trans *)e->data;
	t2 = (trans *)obj;

	return( (t1->subj == t2->subj) &&
		(t1->obj == t2->obj) &&
		(t1->op == t2->op) );
}


