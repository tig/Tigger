/* typedef.h               (c) KindelCo Software Systems, 1987

                   TYPEDEF - header file for typedefs used in make.
*/

#define SMAC        1                  /* static macro flag */
#define DMAC        2                  /* dynamic macro flage */
#define SFILE       3                  /* file flag */
#define INACTIVE    0
#define ACTIVE      1                  /* active flage for update */
#define PROCESSED   2                  /* this node has been processed... */
#define TRUE        1
#define FALSE       0


#define FATAL          1
#define BADOPT_ERR     1
#define NOOPEN_ERR     2
#define TOLONG_ERR     3
#define DONTKNOW_ERR   4
#define BADMACDEF_WAR  5
#define NOMACDEF_ERR   6
#define TOOLNG_ERR     7
#define EXPTOOLNG_ERR  8
#define BADCONT_ERR    9
#define NANDTOPT_ERR   10
#define UANDTOPT_ERR   11

#define MAXLINE        132
#define MAXEXPAND      512

/* typedefs */


typedef struct cmnd_list                  /* linked list for command structure */
        {    struct cmnd_list *cmnd_link; /* link to next command */
             char *cmnd;                  /* pointer to the command */
        } CMNDLIST, *CMNDLISTPTR;

typedef struct dep_list                   /* linked list of dependency nodes */
        {    struct dep_list *dep_link;   /* link to next dependency */
             struct grph_node *dep_nodep; /* pointer to the dependency node */
              /* GRNODEPTR */
        } DEPLIST, *DEPLISTPTR;


typedef struct grph_node                  /* macro and file structure */
        {    struct grph_node *grph_link; /* link to next strucure */
             short nodetype;              /* the type of node (macro or file)*/
             char *nodename;              /* the name of the file or macro */
             char *macroval;              /* the value of the macro (if a macro) */
             short outdate;               /* out of date flag */
             short status;                /* status of the node (proccessed or active) */
             long time;                  /* time (used in update) */
             DEPLISTPTR prelist;          /* pointer to list of precursors */
             CMNDLISTPTR cmndlist;        /* pointer to list of commands */
        } GRNODE, *GRNODEPTR;


/* end of typedef.h */
