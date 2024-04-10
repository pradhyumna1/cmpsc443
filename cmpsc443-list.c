/**********************************************************************

   File          : cmpsc443-list.c

   Description   : This file contains the list processing code

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

/* Globals */

/* Functional Prototypes */  

/**********************************************************************

    Function    : init
    Description : initialize list
    Inputs      : list - list object
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int init( list *l )
{
	l->head = NULL;

	return 0;
}


/**********************************************************************

    Function    : insert
    Description : Insert element in list after (optional) elt
    Inputs      : list - list object
                  new - new element to insert
                  after - optional element to insert after (if null, place at head)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int insert( list *l, elt *new, elt *after )
{
	elt *tmp;

	if ( !after ) {
		tmp = l->head;
		l->head = new;
		new->next = tmp;
	}
	else {
		tmp = after->next;
		after->next = new;
		new->next = tmp;
	}
	
	return 0;
}

/**********************************************************************

    Function    : get
    Description : get element that matches 
    Inputs      : list - list object
                  obj - object to compare
                  cmp - comparison function 
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

elt *get( list *l, void *obj, int(*cmp)(elt *e, void *obj))
{
	elt *node = l->head;
	
	while( node != NULL )
	{
		if ( cmp( node, obj ) ) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}



/**********************************************************************

    Function    : pos
    Description : retrieve position in list of elt
    Inputs      : list - list object
                  obj - object to match from elts in list
                  cmp - function for comparison
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

int pos( list *l, void *obj, int(*cmp)(elt *e, void *obj))
{
	elt *node = l->head;
	int ct = 0;
	
	while( node != NULL )
	{
		if ( cmp( node, obj ) ) {
			return ct;
		}
		node = node->next;
		ct++;
	}
	return -1;
}



