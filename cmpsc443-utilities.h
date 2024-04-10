/**********************************************************************

   File          : cmpsc443-util.h

   Description   : Utilities
   
***********************************************************************/

/* Include Files */
#include <gcrypt.h>

/* Defines */
#define AES128_KEY_BYTES 16
#define FILE_CLEAR 1
#define FILE_APPEND 0

/* Functional Prototypes */

/**********************************************************************

    Function    : errorMessage
    Description : prints an error mesage to stderr
    Inputs      : msg - pointer to string message
    Outputs     : 0 if successful, -1 if failure

***********************************************************************/
int errorMessage( char *msg );

/**********************************************************************

    Function    : write_to_file
    Description : writes content to a file
    Inputs      : fname - file name
                  content - data to write to fname file
                  len - length of content
                  flag - clear or append to file
    Outputs     : 0 if successful, -1 if failure

***********************************************************************/

int write_to_file(char *fname, char *content, int len, unsigned flag);

/**********************************************************************

    Function    : read_from_file
    Description : read content to a file
    Inputs      : fname - file name
                  content - data to write to fname file
    Outputs     : length if successful, -1 if failure

***********************************************************************/

int read_from_file(char *fname, char **content);
