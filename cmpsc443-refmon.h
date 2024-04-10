/**********************************************************************

   File          : cmpsc443-refmon.h

   Description   : Reference monitor defines

***********************************************************************/

/* Include Files */

/* Defines */
#define O_READ  1
#define O_WRITE 2
#define O_EXEC  4

#define MAX_OPS 3

#define T_PROC  16
#define T_FILE  32

/* globals */
extern int cmdCt;

/* Functional Prototypes */

/**********************************************************************

    Function    : execCmds
    Description : Process exec commands 
    Inputs      : fh - output file pointer
                  data - command data
                  len - length of command data
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int execCmds( FILE *fh, char *data, int len );

/**********************************************************************

    Function    : policyCmds
    Description : Process policy commands 
    Inputs      : fh - output file pointer
                  data - command data
                  len - length of command data
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int policyCmds( FILE *fh, char *data, int len );

/**********************************************************************

    Function    : labelFile
    Description : Compute label (level) for file given process creator (optional)
    Inputs      : fh - output file pointer
                  proc - process name
                  file - file name
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int labelFile( FILE *fh, char *proc, char *file );

/**********************************************************************

    Function    : labelProcess
    Description : Compute label (level) for process
    Inputs      : fh - output file pointer
                  name - user name or parent process name
                  proc - process name
                  mapping - label or system mapping for finding level for name
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int labelProcess( FILE *fh, char *name, char *proc, list *mapping );

/**********************************************************************

    Function    : checkAccess
    Description : Authorize op
    Inputs      : fh - output file pointer
                  proc - name of process
                  file - name of file accessed
                  op - identity of operation (some combination of rwx)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int checkAccess( FILE *fh, char *proc, char *file, int op );

/**********************************************************************

    Function    : checkTrans
    Description : Perform transition of levels if policy requires
    Inputs      : fh - output file pointer
                  proc - name of process
                  file - name of file accessed
                  op - operation being performed
                  ttype - transition type (process or file)
    Outputs     : 0 if successfully completed, -1 if failure

***********************************************************************/

extern int checkTrans( FILE *fh, char *proc, char *file, int op, int ttype );
