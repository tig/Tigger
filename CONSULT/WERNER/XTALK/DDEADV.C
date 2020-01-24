

/* This module implements the DDE Advise/Unadvise functions */

/* The way it is:
Since DDE Advise is hacked into Crosstalk For Windows, I had to
keep the advise list as a linked list of numbers of variables that
the interpreter will check whenever it does an assignment.  If
there is an advise on a vairable then the message will be sent.

I did not want to fix a limit on the number of variables so the
linked list is the logical conclusion.  Since windows memory management
sucks I could not use a classical linked list structure.  Pointers don't.

The list is defined by the structure "list_item".  This structure
has two integers (HANDLES).  The "varno" handle is the variable
handle from interp.c.  The "next" handle is the handle of
the next entry in the linked list.  It too was allocated with
GlobalAlloc.  If "next" == 0 then you have found the end of
the list.  I initialize the advise_list variable to null and handle
the rest in the classical linked list manner.
	-JON
*/

#include <windows.h>
#include <dde.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "xtalk.h"
#include "ddeadv.h"


typedef struct
  {
  WORD	fEmpty:12;
  WORD	fResponse:1;
  WORD	fRelease:1;
  WORD	fNoData:1;
  WORD	fAck:1;
  WORD	cfFormat;
  BYTE	info[30];
  } DDEDATA;

typedef DDEDATA *PDDEDATA;
typedef DDEDATA FAR *LPDDEDATA;

LPSTR lstrcmp(LPSTR,LPSTR);
LPSTR lstrcpy(LPSTR,LPSTR);

/* This is a prototype */
BOOL SendDDEAdvData(HWND, WORD, char *, DDE_ADV_DATA);

struct list_item_tag
{
	int varno;
	HANDLE name;
	HANDLE next;
	HANDLE hClient;
	DDE_ADV_DATA adv_data;
};
typedef struct list_item_tag list_item;

/* The head of the linked list of variable numbers */
HANDLE adv_list = NULL;
extern ATOM aAdviseItem;	//in prodde.c

int add_advise(name,varno,hClient,adv_data)
char *name;
int varno;
HANDLE hClient;
DDE_ADV_DATA adv_data;
{
list_item FAR *new_item;
HANDLE new_entry;

list_item FAR *cur_item;
HANDLE cur_entry;

char FAR *new_name;

	/* Check for first item on the list */
	if(adv_list == NULL)
	{
		/* Allocate an item for the head */
		adv_list = GlobalAlloc(GMEM_MOVEABLE,(LONG)sizeof(list_item));
		new_item = (list_item FAR *)GlobalLock(adv_list);
		new_item->next = NULL;
		new_item->hClient = hClient;

		/* save the name for advise messages */
		new_item->name = GlobalAlloc(GMEM_MOVEABLE,
				(LONG)strlen(name)+1);
		new_name = (char FAR *)GlobalLock(new_item->name);
		lstrcpy(new_name,name);
		GlobalUnlock(new_item->name);

		/* save the variable handle */
		new_item->varno = varno;

		/* store the words */
		new_item->adv_data = adv_data;
		GlobalUnlock(adv_list);
		return(TRUE);
	}
	else
	{
		/* This is the normal case */

		/* Search the list for a dupe */
		if(advised_on(varno,hClient) != NULL)
			return(TRUE);

		/* Find a place for it  */
		cur_entry = adv_list;
		while(1)
		{
			cur_item = (list_item FAR *)GlobalLock(cur_entry);
			if(cur_item->next == NULL)
			{
				/* Add the new item handle to current entry */
				new_entry = GlobalAlloc(GMEM_MOVEABLE,
						(LONG)sizeof(list_item));
				cur_item->next = new_entry;
				GlobalUnlock(cur_entry);

				/* and add the new item to memory */
				new_item=(list_item FAR *)GlobalLock(new_entry);
				new_item->varno = varno;
				new_item->next = NULL;

				/* save the name for advise messages */
				new_item->name = GlobalAlloc(GMEM_MOVEABLE,
						(LONG)strlen(name)+1);
				new_name = (LPSTR)GlobalLock(new_item->name);
				lstrcpy(new_name,name);
				GlobalUnlock(new_item->name);

				/* store the words */
				new_item->adv_data = adv_data;
				new_item->hClient = hClient;

				/* unlock the new entry */
				GlobalUnlock(new_entry);

				/* exit while */
				break;
			}
			else
			{
			HANDLE temp_entry;

				/* Remember where we are */
				temp_entry = cur_item->next;

				/* free the memory */
				GlobalUnlock(cur_entry);

				/* and do the next */
				cur_entry = temp_entry;
			}
		}
	}
	return(TRUE);
}

/* This removes a specific entry for a specific client or all entries
for a specific client
To use it: call it with a specific client and varno set to a var or -1
to get ALL vars associted with a client */
BOOL remove_advise(varno,hClient)
int varno;
HWND hClient;
{
HANDLE prev_entry;
list_item FAR *prev_item;

HANDLE cur_entry;
list_item FAR *cur_item;

HANDLE tmp_next;
BOOL removed;


	removed = FALSE;	/* we have not found it yet */

	/* new start at the head of the list */
	prev_entry = NULL;
	cur_entry  = adv_list;

	/* locate the item on the list keeping track of previous */
	while(cur_entry != NULL)
	{
		/* Lock the item */
		cur_item = (list_item FAR *)GlobalLock(cur_entry);
		tmp_next = cur_item->next;	/* snag for patch */

		/* and compare */
		if((hClient == cur_item->hClient) &&
		((cur_item->varno == varno)||(varno==-1)))
		{

			/* remove the item from the list */

			/* unlock and free the entry */
			GlobalFree(cur_item->name);
			GlobalUnlock(cur_entry);
			GlobalFree(cur_entry);
			cur_item = NULL;
			cur_entry = NULL;

			/* now patch the table */
			if(prev_entry != NULL)
			{
				/* lock the entry */
				prev_item = (list_item FAR *)GlobalLock(
								prev_entry);
				prev_item->next= tmp_next;
				GlobalUnlock(prev_entry);
			}
			else
				adv_list = tmp_next;

			removed = TRUE;
		}
		else
		{
			/* move down the list */
			prev_entry = cur_entry;
			GlobalUnlock(cur_entry);
			cur_entry = tmp_next;
		}

		if((removed == TRUE)&&(varno != -1))
			break;
	}
	return(removed);
}

/* This checks for an advise on a variable.  If hClient is not null then 
it checks for the specific client.  If hClient IS null then it checks for
any client */
HANDLE advised_on(varno,hClient)
int varno;
HWND hClient;
{
HANDLE cur_entry;
list_item FAR *cur_item;

int comp_val;
HANDLE temp_entry;

	/* start out assumeing that varno is not on the list */
	comp_val = FALSE;

	/* Traverse the list */
	cur_entry = adv_list;
	while(cur_entry != NULL)
	{
		/* do the compare and snag the next item */
		cur_item = (list_item FAR *)GlobalLock(cur_entry);

		if((cur_item->varno == varno) &&
		((hClient==cur_item->hClient) || (hClient==NULL)))
			comp_val = TRUE;

		temp_entry = cur_item->next;
		GlobalUnlock(cur_entry);


		/* act on the compare value */
		/* or move to the next */
		if(comp_val == TRUE)
			return(cur_entry);
		else
			cur_entry = temp_entry;
	}
	return(NULL);
}


void remove_all_advise()
{
HANDLE list,next_entry;
list_item FAR *cur_item;

	/* start out at the head of the list */
	list = adv_list;
	adv_list = NULL;	/* We will not use it again */

	while(list != NULL)
	{
		/* snag the data from the list */
		cur_item = (list_item FAR *)GlobalLock(list);
		GlobalFree(cur_item->name);
		next_entry = cur_item->next;
		GlobalUnlock(list);

		/* free the entry */
		GlobalFree(list);

		/* and go on to the next one */
		list = next_entry;
	}
}

/* This is where the advise message is sent from.  The parametrs tell us */
/* what to send and how to send it:
void Advise(hAvEntry,lLong,szString)

The hAvEntry is the HANDLE returned by advised_on(varno) that tells us 
the data that the client wanted and information on the Acking and stuff.

If this is an integer(long) variable from the interpreter then lLong will
have the value and szString == NULL.  If szString != NULL then I assume
that lLong is of no use and send the string.  Pretty neat.. huh?
There's more:

The hAvEntry I get will be the first one in the advise_list chain.  I
Traverse the chain until I hit the end so that ALL Clients that are
advised on this variable will get a message */

extern HWND hWnd;	/* Main window handle */

void Advise(hAvEntry,lLong,szString)
HANDLE hAvEntry;
LONG lLong;
char *szString;
{
HANDLE cur_entry;	/* This is the current entry in the chain */
list_item FAR *cur_item;

HANDLE next_entry;	/* This is the next one in the chain */

LPSTR cur_name;
ATOM aName;
HWND hClient;
DDE_ADV_DATA adv_data;
char buffer[256];	/* Max for a string var */
BOOL SendData;		/* Send data flag */
int varno = -1;		/* Init to -1 so I know that the loop is new */

	if(!IfSystemStatus(SS_DDE))
		return;

	cur_entry = hAvEntry;	/* Start here */

	while(cur_entry != NULL)
	{
		/* Lock The data down */
		cur_item = (list_item FAR *)GlobalLock(cur_entry);
		cur_name = (LPSTR)GlobalLock(cur_item->name);
		hClient  = cur_item->hClient;
		next_entry = cur_item->next;

		/* This is my trick to initialize the loop */
		/* if varno == -1 then I should assign in */
		/* otherwise I should check for it */
		if(varno == -1)
			varno = cur_item->varno;

		/* Make sure that we should do this one */
		if(cur_item->varno != varno)
		{
			/* Skip this one */
			GlobalUnlock(cur_item->name);
			GlobalUnlock(cur_entry);

			/* and move on */
			cur_entry = next_entry;
			continue;
		}

		/* gather the data from the data area */
		aName = GlobalAddAtom(cur_name);
		adv_data = cur_item->adv_data;

		/* Unlock the data area */
		GlobalUnlock(cur_item->name);
		GlobalUnlock(cur_entry);

		/* Construct the data to send */
		if(adv_data.fNoData == 1)
			SendData = FALSE;
		else
			SendData = TRUE;

		if(SendData == TRUE)
		{
			/* Send the data */
			if(szString == NULL)
				sprintf(buffer,"%ld",lLong);
			else
				strcpy(buffer,szString);
		}

		/* Now send the message */
		SendDDEAdvData(hClient,aName,buffer,adv_data);

		/* And move on to the next list entry */
		cur_entry = next_entry;
	}
}

/* I could not find this in Kents code.  It probobly exists */
void upcase(string)
char *string;
{
	while(*string)
	{
		*string = toupper(*string);
		++string;
	}
}


BOOL SendDDEAdvData(hWndDDE, wAtom, buf, adv_data)
  HWND hWndDDE;
  WORD wAtom;
  char *buf;
  DDE_ADV_DATA adv_data;
{
  HANDLE    hMem;
  DWORD     dwSize;
  LPDDEDATA lpData;
  BOOL	    bResult;
  int	    len;

  if(!IfSystemStatus(SS_DDE))
	  return(0);


  bResult = FALSE;


  /* Check to see if we send a NULL data flag */
  if(adv_data.fNoData == 0)
  {
	/* Get some memory */
	len = strlen(buf) + 1;
	dwSize = 4 + len;
	hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dwSize);

	if(lpData = (LPDDEDATA)GlobalLock(hMem))
	{
		register int i;

		lpData->fRelease = adv_data.fRel;
		lpData->fResponse = FALSE;
		lpData->fAck     = adv_data.fAck;
		lpData->cfFormat = CF_TEXT;

		if(adv_data.fAck)
			aAdviseItem = wAtom;

		for(i=0; i<len; i++)
		  lpData->info[i] = buf[i];

		GlobalUnlock(hMem);

	}
      else
	GlobalFree(hMem);

    }
    else
	hMem = NULL;

    /* And send the messate */
    bResult=PostMessage(hWndDDE,WM_DDE_DATA,hWnd,MAKELONG(hMem,wAtom));

  return (bResult);
}



#ifdef DEBUGADV
void d_adv_data(hWnd,h,adv_data)
HANDLE h;
DDE_ADV_DATA adv_data;
{
char line[80];

	sprintf(line,"H: %d fRel:%d fNoData:%d fAck:%d cfFormat:%d",h,
		adv_data.fRel,adv_data.fNoData,adv_data.fAck,
		adv_data.cfFormat);

	MessageBox(hWnd,line,"Advise Data",MB_OK);
}
#endif
