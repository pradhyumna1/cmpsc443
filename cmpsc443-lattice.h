/**********************************************************************

   File          : cmpsc443-lattice.h
   Description   : Lattice defines and functions.

***********************************************************************/

/* Include Files */

/* Defines */
#define E_LEVEL 1
#define E_MAP   2
#define E_TRANS 4


/* Policy Types */
typedef struct level_t {
	char *name;
	int len;
} level;


typedef struct map_t {
	char *name;
	int len;
	level *l;
} map;


typedef struct trans_t {
	level *subj;
  	level *obj;
	level *new;
	int op;
} trans;


/* Globals */
extern list lattice;
extern list label_mapping;
extern list trans_mapping;


/* Functional Prototypes */

/**********************************************************************

    Function    : addLattice
    Description : Add levels - add from low to high levels
    Inputs      : fh - output file pointer
                  low - name of lower level
                  high - name of higher level
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int addLattice( FILE *fh, char *low, char *high );

/**********************************************************************

    Function    : addLabelPolicy
    Description : Add mapping from name to level
    Inputs      : fh - output file pointer
                  name - name (prefix) of objects at level
                  level - name of level
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int addLabelPolicy( FILE *fh, char *name, char *level );

/**********************************************************************

    Function    : addTransPolicy
    Description : Add mapping from name to level
    Inputs      : fh - output file pointer
                  subj_level - level of subject of op
                  obj_level - level of object of op
                  new_level - resultant level of subj or obj (op-dependent)
                  op - operation (rwx)
                  ttype - transition type (process or file)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int addTransPolicy( FILE *fh, char *subj_level, char *obj_level, char* new_level, 
			   int op, int ttype );

/**********************************************************************

    Function    : matchName
    Description : Find if node name matches supplied name
    Inputs      : elt - element to check 
                  obj - object to match on
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int matchLevelName( elt *e, void *obj );
extern int matchMapName( elt *e, void *obj );
extern int matchLevel( elt *e, void *obj );
extern int matchTrans( elt *e, void *obj );
