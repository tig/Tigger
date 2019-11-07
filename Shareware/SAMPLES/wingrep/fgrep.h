/** fgrep.h                                                       **/


/* local symbolic definitions */
   #define FG_MAXLINE   255   /* maximum line length              */
   #define FG_ENDOFKEY  '*'   /* end of keyword flag (trie)       */


/* flags for searching */
   #define FGF_NOCASE      0x8000   /* non-case sensitive grep       */
   #define FGF_FIRSTONLY   0x0001   /* stops after first occurrance  */


/* macros */
   #define new    (FGTRIE *)malloc(sizeof(FGTRIE))
   #define fgcmp(a,b)   ((flags&FGF_NOCASE)?toupper(a)==toupper(b):(a)==(b))
                                             

/* typedefs */
   typedef struct tFGTRIE FGTRIE;
   struct tFGTRIE {
      char     c;             /* the character in the trie        */
      char     end;           /* flag for end of keyword          */
      FGTRIE  *link_p;        /* the link pointer                 */
      FGTRIE  *alt_p;         /* the alternate pointer            */
      FGTRIE  *fail_p;        /* the failure pointer              */
   };

   /* structure for the queue */
   typedef struct tFGQUEUE FGQUEUE;
   struct tFGQUEUE {
      FGQUEUE *nextq_p;
      FGTRIE  *trie_p;
   };


/* function prototypes */
   void fgDestroyTable( FGTRIE  *state_zero_p );
   FGTRIE *fgBuildTable( char **aszText, short flags );
   FGQUEUE *fgBuildFailStates( FGTRIE  *state_zero_p, short flags );
   FGTRIE *fgGoto( FGTRIE *state_p, char in, short flags );
   FGTRIE *fgNext( FGTRIE *state_p, char in, FGTRIE  *state_zero_p, short flags );
   void fgEnque( FGTRIE *new_p, FGQUEUE **frontq_p );
   FGTRIE *fgDeque( FGQUEUE **frontq_p );
   short fgEnumSearch( char *szFile, FGTRIE *state_zero_p, FARPROC lpEnumFunc, short flags );
   short fgEnumFile( char *szFile, char **aszText, FARPROC lpEnumFunc, short flags );




