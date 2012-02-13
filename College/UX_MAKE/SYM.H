/* sym.h               (c) KindelCo Software Systems, 1987 */

#define TABLESIZE    19

extern GRNODEPTR SymTable [TABLESIZE];

/* funcions */
void      SymInit ();      /* SymInit () */
GRNODEPTR SymAdd ();       /* SymAdd (name, type); */
GRNODEPTR SymFind ();      /* SymFind (name, type); */
int       SymHash ();      /* SymHash (name) */
void      SymHashAdd ();   /* SymHashAdd (node, location) */
void      SymPrint ();     /* () */





/*    END OF sym.h                     (C) KindelCo Software Systems, 1987 */

