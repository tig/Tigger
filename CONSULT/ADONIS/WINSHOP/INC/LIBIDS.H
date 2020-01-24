#define BITMAP_INFO 1

#define	BASE_BITMAP_IDS	1000
#define lpBMP_ID(index)   (MAKEINTRESOURCE(BASE_BITMAP_IDS+index))
#define nBMP_ID(index)   (BASE_BITMAP_IDS+index)

#if 0
#define BASE_GROUP_INFO_IDS 10000
#define lpGROUP_INFO_ID(index)   (MAKEINTRESOURCE(BASE_GROUP_INFO_IDS+index))
#define nGROUP_INFO_ID(index)   (BASE_GROUP_INFO_IDS+index)
#endif

/** GROUP INFO stuff **/
#define NOT_IN_GROUP    (WORD)0
#define FIRST_IN_GROUP  (WORD)'F'
#define LAST_IN_GROUP   (WORD)'L'
#define IN_GROUP        (WORD)'M'
#define bIS_INGROUP(code) (code)
#define szBITMAP_GINFO "BMG"
