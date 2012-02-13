/* make.h               (c) KindelCo Software Systems, 1987

                 make.h - (Mk)

*/


extern short Mkofflinef;
extern short Mktouchf;
extern short Mkupdatef;
extern short MkStatus;
extern char* MkTargetName;
extern long MkCurTime;

#ifdef TURBOC
void MkError (short);
#else
void MkError ();
#endif

/* END OF make.h                          (C) KindelCo Software Systems, 1987 */
