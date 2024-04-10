/**********************************************************************

   File          : cmpsc443-lattice.h

   Description   : Lattice defines and functions.

***********************************************************************/

/* Include Files */

/* Defines */
struct elt_t {
	int type;
	void *data;
	struct elt_t *next;
	struct elt_t *prev;
};

typedef struct elt_t elt;


typedef struct {
	elt *head;
} list;


/* Functional Prototypes */

/**********************************************************************

    Function    : init
    Description : initialize list
    Inputs      : list - list object
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int init( list *l );

/**********************************************************************

    Function    : insert
    Description : Insert element in list after (optional) elt
    Inputs      : list - list object
                  new - new element to insert
                  after - optional element to insert after (if null, place at head)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int insert( list *l, elt *new, elt *after );

/**********************************************************************

    Function    : get
    Description : get element that matches 
    Inputs      : list - list object
                  name - name of object to retrieve
                  cmp - comparison function 
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern elt *get( list *l, void *obj, int(*cmp)(elt *e, void *obj));


/**********************************************************************

    Function    : pos
    Description : retrieve position in list of elt
    Inputs      : list - list object
                  obj - object to match from elts in list
                  cmp - function for comparison
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int pos( list *l, void *obj, int(*cmp)(elt *e, void *obj));
