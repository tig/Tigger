/** fgrep.c
 *
 * DESCRIPTION:
 *    Modified to work with WinGREP! for windows.
 *
 ** Grog! */


/* the includes required */
   #include <windows.h>
   #include <stdio.h>
   #include <string.h>

   #include "fgrep.h"


/** void fgEnque( FGTRIE *new_p, FGQUEUE **frontq_p )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

void fgEnque( FGTRIE *new_p, FGQUEUE **frontq_p )
{
   FGQUEUE *endq_p, *tempq_p;

   if ( (*frontq_p) == NULL )
   {
      endq_p = (FGQUEUE *)malloc( sizeof(FGQUEUE) );
      (*frontq_p) = endq_p;
   }

   else
   {    
      endq_p = (FGQUEUE *)malloc( sizeof(FGQUEUE) );
      tempq_p = (*frontq_p);

      while ( (tempq_p->nextq_p) != NULL )
         tempq_p = tempq_p->nextq_p;

      tempq_p->nextq_p = endq_p;
   }

   endq_p->trie_p = new_p;
   endq_p->nextq_p = NULL;
   return;
} /* fgEnque() */



/** FGTRIE *fgDeque( FGQUEUE **frontq_p )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

FGTRIE *fgDeque( FGQUEUE **frontq_p )
{
   FGQUEUE *dequed_p;
   FGTRIE  *temp_p;

   if ( (*frontq_p) == NULL )
      return ( NULL );

   else
   {
      temp_p = (*frontq_p)->trie_p;
      dequed_p = (*frontq_p);
      (*frontq_p) = (*frontq_p)->nextq_p;
      free( dequed_p );

      return ( temp_p );
   }
} /* fgDeque() */



/** FGQUEUE *fgBuildFailStates( FGTRIE  *pZState, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

FGQUEUE *fgBuildFailStates( FGTRIE  *pZState, short flags )
{
   FGQUEUE *frontq_p;   /* pointer to the front of the q    */
   FGTRIE  *s_p,        /* current trie                     */
           *temp_p;     /* temporary pointer to a trie      */
   char     a;

   /* empty the queue */
   frontq_p = NULL;

   /* if there are more tries */
   if ( (s_p = pZState->link_p) != NULL )
   {
      /* go down first level an enque them */
      while ( s_p->alt_p != NULL )
      {
         fgEnque( s_p, &frontq_p );
         s_p = s_p->alt_p;
      }

      fgEnque( s_p, &frontq_p );

      while ( (s_p = fgDeque( &frontq_p )) != NULL )
      {
         temp_p = s_p;
         if (temp_p->link_p != NULL)
         {
            temp_p = temp_p->link_p;
            fgEnque( temp_p, &frontq_p );
            temp_p->fail_p = fgNext( s_p->fail_p, temp_p->c, pZState, flags );

            while ( (temp_p->alt_p) != NULL )
            {
               temp_p = temp_p->alt_p;
               fgEnque( temp_p, &frontq_p );
               temp_p->fail_p = fgNext( s_p->fail_p, temp_p->c, pZState, flags );

               if ( temp_p->fail_p->c == FG_ENDOFKEY )
                  temp_p->c = FG_ENDOFKEY;
            }
         }
      }
   }

   /* pointer to the front of the q */
   return ( frontq_p );
} /* fgBuildFailStates() */



/** void fgDestroyTable( FGTRIE  *pZState )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

void fgDestroyTable( FGTRIE  *pZState )
{
   /* this function should free all memory in use by the pZState */


} /* fgDestroyTable() */



/** FGTRIE *fgBuildTable( char **aszTokens, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

FGTRIE *fgBuildTable( char **aszTokens, short flags )
{
   FGTRIE  *pZState;
   FGTRIE  *current_p;     /* current trie                     */
   FGTRIE  *new_trie_p;    /* a new trie                       */
   FGTRIE  *next_p;        /* the next trie                    */
   char    *keyword, c;
   int      i;             /* keyword char index               */
   int      j;             /* keyword array index              */

   /* malloc a zero state */
   pZState = new;

   /* init state zero */
   pZState->fail_p = pZState;
   pZState->link_p = NULL;
   pZState->alt_p = NULL;
   pZState->c = NULL;
   pZState->end = NULL;

   /* step through each keyword */
   for ( j = 0; keyword = aszTokens[j]; j++ )
   {
      /* point to origin and zero keyword char index */
      current_p = pZState;  i = 0;

      /* while there's more chars */
      while ( (c = keyword[i++]) != NULL )
      {
         /* if link is not there */
         if ( current_p->link_p == NULL )
         {
            /* make a new trie */
            new_trie_p = new;         
            new_trie_p->fail_p = pZState;
            new_trie_p->link_p = NULL;
            new_trie_p->alt_p = NULL;
            new_trie_p->end = NULL;
            new_trie_p->c = c;

            /* point to new trie   */
            current_p->link_p = new_trie_p;
            current_p = new_trie_p;
         }

         /* then there's a link */
         else
         {
            /* point to linked trie */
            next_p = current_p->link_p;

            /* go down alts till char matches or alt is null */
            while ( (next_p->c != c) && (next_p->alt_p != NULL) )
               next_p = next_p->alt_p;


            /* if char matches move on */
            if ( fgcmp( next_p->c, c ) )
                  current_p = next_p;

            /* else build an alt */
            else                 
            {    
               new_trie_p = new;
               new_trie_p->fail_p = pZState;
               new_trie_p->link_p = NULL;
               new_trie_p->alt_p = NULL;
               new_trie_p->c = c;
               new_trie_p->end = NULL;
               next_p->alt_p = new_trie_p;
               current_p = new_trie_p;
            }
         }
      }

      /* end of keyword! */
      current_p->end = FG_ENDOFKEY;
   }

   /* find fail states in trie struct */
   fgBuildFailStates( pZState, flags );

   return ( pZState );
} /* fgBuildTable() */



/** FGTRIE *fgGoto( FGTRIE *state_p, char in, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

FGTRIE *fgGoto( FGTRIE *state_p, char in, short flags )
{
   FGTRIE  *temp_p;        /* a temporary pointer              */

   /* no where to look */
   if ( state_p->link_p == NULL )
      return ( NULL );

   else
   {   
      if ( fgcmp( state_p->link_p->c, in ) )
         return ( state_p->link_p );

      else
      {
         temp_p = state_p->link_p;

         while ( temp_p->alt_p != NULL )
         {
            if ( fgcmp( temp_p->alt_p->c, in ) )
               return ( temp_p->alt_p );

            else
               temp_p = temp_p->alt_p;
         }

         return ( fgcmp( temp_p->c, in ) ? temp_p : NULL );
      }
   }
} /* fgGoto() */



/** FGTRIE *fgNext( FGTRIE *state_p, char in, FGTRIE  *pZState, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

FGTRIE *fgNext( FGTRIE *state_p, char in, FGTRIE  *pZState, short flags )
{    
   FGTRIE *temp_p;

   if ( (temp_p = fgGoto( state_p, in, flags )) == NULL )
   {
      if ( state_p != pZState )
         state_p = fgNext( state_p->fail_p, in, pZState, flags );
   }

   /* if fgGoto == NULL then return statezero */
   return ( temp_p ? temp_p : pZState );
} /* fgNext() */



/** short fgEnumSearch( char *szFile, FGTRIE *pZState, FARPROC lpEnumFunc, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

short fgEnumSearch( char *szFile, FGTRIE *pZState, FARPROC lpEnumFunc, short flags )
{
   FILE    *fp;
   FGTRIE  *current_p;
   char     line[FG_MAXLINE + 1];
   char     c;
   int      i;
   short    matches = 0;      /* no matches found yet       */


   /* if we can open the file */
   if ( fp = fopen( szFile, "r" ) )
   {
      current_p = pZState;

      /* get a line */
      while ( fgets(line, FG_MAXLINE, fp) )
      {
         /* go through line       */
         for ( i = 0; c = line[i]; i++ )
         {
            /* checking the states */
            current_p = fgNext( current_p, c, pZState, flags );

            if ( current_p->end == FG_ENDOFKEY )
            {
               /* increment the number of matches currently */
               matches++;

               /* if there is a callback, then call it */
               if ( lpEnumFunc ) 
               {
                  /* if callback told us to stop */
                  if ( !(* lpEnumFunc)( line ) )
                  {
                     /** need to create stop flag **/
                  }
               }

               break;
            }
         }
      }

      /* close the file that we opened */
      fclose( fp );
   }

   /* return the number of matches found */
   return ( matches );
} /* fgEnumSearch() */


/** short fgEnumFile( char *szFile, char **aszTokens, FARPROC lpEnumFunc, short flags )
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN (void):
 *
 ** Grog! */

short fgEnumFile( char *szFile, char **aszTokens, FARPROC lpEnumFunc, short flags )
{  
   FGTRIE  *pZState;
   short    matches;

   /* build table from aszTokens[] array */
   pZState = fgBuildTable( aszTokens, flags );

   /* search file and enumerate found sequences */
   matches = fgEnumSearch( szFile, pZState, lpEnumFunc, flags );

   /* free up all memory that the table currently holds */
   fgDestroyTable( pZState );

   /* return number of matches found -- 0:none found or file error */
   return ( matches );
} /* fgEnumFile() */


/** EOF: fgrep.c **/
