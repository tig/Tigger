struct dde_adv_data_tag
{
	/* Word 1 (flags) */
	WORD	fRes:13;
	WORD	fRel:1;
	WORD	fNoData:1;
	WORD	fAck:1;

	/*Word 2 (format) */
	WORD	cfFormat;
};
typedef struct dde_adv_data_tag DDE_ADV_DATA;
typedef struct dde_adv_data_tag FAR *DDE_ADV_DATA_PTR;
	
extern HANDLE advised_on(int,HWND);
extern int add_advise(char*,int,HANDLE,DDE_ADV_DATA);
extern BOOL remove_advise(int,HWND);
extern void Advise(HANDLE,LONG,char *);
extern void remove_all_advise();
extern void upcase(char *);

#define dEBUGADV
#ifdef DEBUGADV
extern void d_adv_data();
#endif
