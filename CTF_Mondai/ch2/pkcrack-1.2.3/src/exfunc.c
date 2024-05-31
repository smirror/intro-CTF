/*
 * exfunc.c
 *
 * This file contains a function extracting compressed data from a ZIP-archive.
 * Gratefully received from a person who wishes to remain anonymous.
 *
 * Slightly modified by Peter Conrad, Aug '96 - 2003, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "headers.h"

extern int      read_sig( FILE *fp );
extern int      read_local( FILE *fp, char *fname, int flags );
extern int      read_dir( FILE *fp, int flags );
extern int      read_end( FILE *fp, int flags );
extern int	read_ddesc(FILE *fp, int flags);

#define	TRUE	1
#define	FALSE	0

/** Reads the (encrypted/compressed) contents stored under "decfile" in the
 *  file "zipname".
 *  The pointer returned by this function must later be free()d.
 */
char *extract(char *zipname, char *decfile, int caseflg )
{
int end;
int error;
int found;
int ret;
int rc;
char	*data;
FILE	*fp;               /* ZIP file pointer */

  fp = fopen(zipname, "rb");
  if(fp==NULL)
  {
    perror("Error opening the ZIP file");
    return(0);
  }

  /****************************************************************************/
  /* Search the given file in the ZIP archive                                 */
  /****************************************************************************/
  found = FALSE;
  error = FALSE;
  end = FALSE;
  do
  {
    ret = read_sig(fp);
    switch(ret)
    {
      case SIGN_LOCAL:
        {
#ifdef DEBUG
	  printf("\nFile signature found\n");
#endif
	  rc = read_local(fp,decfile,caseflg);
	  switch(rc)
	  {
	    case 1:
	      {
	        found = TRUE;
	        end = TRUE;
	      }
	      break;
	    case 0:
	      break;
	    case -1:
	      {
	        printf("Error reading ZIP file\n");
	        end = TRUE;
	        error = TRUE;
	      }
	      break;
	  }
	}
	break;

      case SIGN_DIR:
          read_dir(fp,0);
	break;

      case SIGN_END:
	  read_end(fp,0);
	  end = TRUE;
	break;

      case SIGN_DDESC:
          read_ddesc(fp,0);
	break;

      default:
        printf("Error: unknown signature (ZIP file may be corrupt)\n");
	end = TRUE;
	error = TRUE;
	break;
    }
  } while (end == FALSE);

  if(error == TRUE)
  {
    fclose( fp );
    return(0);
  }
  else;

  if(found == FALSE)
  {
    fclose( fp );
    printf("File %s not found in ZIP file\n", decfile);
    return(0);
  }
  else;

  data = malloc( lh.csize );

  if(!data)
  {
    fclose( fp );
    printf("Error allocating memory\n");
    return(0);
  }
  else;

  end = FALSE;
  error = FALSE;
  ret = fread(data, (size_t) lh.csize, 1, fp);
  if(ret != 1)
  {
    perror("Error reading ZIP file");
    end = TRUE;
    error = TRUE;
    free(data);
  }

  fclose(fp);

#ifdef DEBUG
  printf("File closed\n");
#endif

  if( !error )
    return(data);
  else
    return 0;
}
