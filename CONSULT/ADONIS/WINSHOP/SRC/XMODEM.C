/*************************************************************\
 ** FILE:  xmodem.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  XMODEM transfer routines
 **
 ** HISTORY:   
 **   Doug Kent  May 7, 1990 (created)
 **
\*************************************************************/
#include <windows.h>
#include <io.h>
#include "ws.h"
#include "comm.h"
#include "misc.h"
#include "winundoc.h"
#include "rc_symb.h"
#include "error.h"
#include "string.h"
#include "download.h"
#include "custinfo.h"
#include "file.h"


/** =========================================== **/
/** calls from uncompress library in dilute.obj **/
       void     FAR *variables = NULL;
static HANDLE   hvariables = NULL;
static BYTE     output_buffer[512];
#if 0
extern int FAR dilute(char FAR *,WORD FAR *,
              char FAR *,WORD FAR *,
              void FAR *,WORD);
unsigned int FAR diluteWorkSize(void);
extern void FAR initDilute(void FAR *);
#else
extern int FAR PASCAL dilute(char FAR *,WORD FAR *,
              char FAR *,WORD FAR *,
              void FAR *,char);
unsigned int FAR PASCAL diluteWorkSize(void);
extern void FAR PASCAL initDilute(void FAR *);
#endif
/** =========================================== **/

/*************************************************************\
 ** XMReceive
 **
 ** ARGUMENTS:  char *szDestFileName
 **             char *szCompFileName
 **             char *szDldPath
 **             WORD NumBlocks
 **             HWND hProgressBar
 **
 ** DESCRIPTION:    
 **  Receive a file and uncompress it into szDestFileName.  
 **  
 **  If szCompFileName is not NULL, then put the compressed info 
 **  szCompFileName as well.
 **
 **  Prepend szDldPath to both output file names.
 **  
 **  NumBlocks is the number of XMBLOCKs in the file to be
 **  downloaded.
 **  
 **  hProgressBar is optional and is the handle to a rectangular
 **  area representing the progress of the download.
 **  
 **  hTimeWnd is optional and is the handle to a text window
 **  to display the estimated download time of the file.
 **  
 ** ASSUMES:    
 **
 ** MODIFIES:
 **  Issues error messages.
 **  Hangs up if problem.
 **
 ** RETURNS:
 **  OK if successful, FAIL if not.
 **
 ** HISTORY:   Lauren Bricker  June 22, 1990 (written)
 **  (8.3.90) D. Kent Modified
 **  (9.18.90) D. Kent -- added redundant ACK/NAK sends
\*************************************************************/
int
XMReceive(char *szDestFileName, char *szCompFileName,
          char *szDldPath, WORD NumBlocks, BOOL bUpdateMeter,
          BOOL bNeedTimeEst, DWORD lUncompSize)
{
    int     fpFull = -1, fpComp = -1;
    BYTE    *pBuf;
    XMBLOCK BlockBuf;
    WORD    BlockNum;
    int     nRetval = OK, nRetry;
    HANDLE  hCom = CommDCB.Id, hSaveDldWnd=hDldWnd;
    char    szFullFileName[_MAX_PATH];
    char    szFullCompName[_MAX_PATH];
    HCURSOR hCursor=NULL;
    int     which_cursor=0;
    long time1,time2;
    int nWhichTime=1;
    BOOL bHighSpeed = ((custinfo.baud + BAUD_ID_BASE) > ID_2400);
    DWORD lTotalBytes=0L;

    strcpy (szFullFileName, szDldPath);
    Append (szFullFileName,szDestFileName);

    if ((fpFull = _lcreat(szFullFileName, 0)) == -1) 
        END(FAIL)

    if ( szCompFileName )
    {
        strcpy (szFullCompName, szDldPath);
        Append (szFullCompName,szCompFileName);

        if ((fpComp = _lcreat(szFullCompName, 0)) == -1) 
            END(FAIL)
    }

    if(SendByte(hCom,NAK) != OK)
        END(FAIL)
    if(SendByte(hCom,NAK) != OK)
        END(FAIL)

    BlockNum=1;
    nRetry=0;

    /** initialize uncompression memory **/
    if (init_buffers() != OK)
        END(FAIL)
    initDilute(variables);

    hCursor=SetCursor(hPhoneCursArr[which_cursor]);

	if ( hDldWnd && IsWindow (hDldWnd) )
		EnableWindow (GetDlgItem (hDldWnd, IDCANCEL), TRUE);  //Terminate Button

    while(1)
    /** read blocks **/
    {
        if (bNeedTimeEst)
        {
            switch (nWhichTime)
            {
                case 1:
                    time1 = GetCurrentTime();
                    nWhichTime = 2;
                break;
                case 2:
                    time2 = GetCurrentTime();
                    if (IsWindow(hDldWnd))
                        SendMessage(hDldWnd, MM_SET_EST_TIME, 0, (time2-time1) * NumBlocks/1000);
                    nWhichTime = -1;
                break;
            }
        }

#ifdef INTERNAL
        //AuxPrint("Reading block (%d)",BlockNum);
#endif

        SetCursor(hPhoneCursArr[which_cursor]);
        which_cursor = which_cursor == 3 ? 0 : which_cursor + 1;

        /** read a block **/
        if (BlockNum > NumBlocks) /* then have read last block, get EOT's **/
        {
            pBuf = (BYTE *)&BlockBuf;
            if (ReceiveBuf(hCom,pBuf,2,WAIT_FOR_BBS) != OK)
            {
#ifdef INTERNAL
                AuxPrint("Timed out waiting for EOTs.");
#endif
                END(FAIL)
            }

            if ((pBuf[0] == EOT) AND (pBuf[1] == EOT))
            {
#ifdef INTERNAL
                AuxPrint("Finished downloading.");
#endif
                END(OK) // All Done
            }
        }
        else if (ReceiveBuf(hCom,(BYTE *)&BlockBuf,sizeof(XMBLOCK),45000L) != OK)
        {
#ifdef INTERNAL
            AuxPrint("Error receiving block.");
#endif
            END(FAIL)
        }

        if (BlockBuf.Soh != SOH)
        {
#ifdef INTERNAL
            AuxPrint("Invalid SOH.")
#endif
            ;
        }
        /** see if block number jibes **/
        else if ((BlockBuf.BlockNum == (BYTE)(BlockNum & 0xFF)) AND 
                 (BlockBuf.BlockNumCom == (BYTE)(~(BlockNum & 0xFF))))
        {
#ifdef INTERNAL
            //AuxPrint("Checking CRC.");
#endif

            /* make sure CRC is correct if low speed modem (high speed assume hardware
               error checking) */
            if (bHighSpeed OR (BlockBuf.Crc == CalcCRC(BlockBuf.Data,sizeof(BlockBuf.Data))))
            /* OK so write data to file */
            { 
#ifdef INTERNAL
                //AuxPrint("Expanding buffer.");
#endif
                if (BlockNum == NumBlocks)
                {
                    int count;
                    BYTE *p;

                    /* get rid of zero padding at end of block */
                    for (p = BlockBuf.Data + sizeof(BlockBuf.Data) - 1,
                         count = sizeof(BlockBuf.Data); 
                         p >= BlockBuf.Data; 
                         --count, --p)
                        if (*p)
                            break;
                    lTotalBytes += expand_buf(BlockBuf.Data,count,fpFull,FALSE);
                    // always use FALSE

                    if ( szCompFileName )
                    {
                        //AuxPrint("Writing buffer to compressed file.");
                        _lwrite (fpComp, BlockBuf.Data, count);
                    }

                    while (lTotalBytes < lUncompSize)
                    {
                        BlockBuf.Data[0] = 0;
                        lTotalBytes += expand_buf(BlockBuf.Data,1,fpFull,FALSE);
                        if ( szCompFileName )
                        {
#ifdef INTERNAL
                            //AuxPrint("Writing zeroes to compressed file.");
#endif
                            _lwrite (fpComp, BlockBuf.Data, 1);
                        }
                    }
                }
                else
                {
                    lTotalBytes += expand_buf(BlockBuf.Data,sizeof(BlockBuf.Data),fpFull,FALSE);

                    if ( szCompFileName )
                    {
#ifdef INTERNAL
                        //AuxPrint("Writing buffer to compressed file.");
#endif
                        _lwrite (fpComp, BlockBuf.Data, sizeof(BlockBuf.Data));
                    }
                }


                /** do this before ACK **/
                if (hDldWnd != hSaveDldWnd) // ie hDlDWnd == NULL
                    END(FAIL)

                if(SendByte(hCom,ACK) != OK)
                {
#ifdef INTERNAL
                    AuxPrint("Error sending ack.");
#endif
                    END(FAIL)
                }
                if(SendByte(hCom,ACK) != OK)
                {
#ifdef INTERNAL
                    AuxPrint("Error sending ack.");
#endif
                    END(FAIL)
                }

                ++BlockNum;
                nRetry=0;
                if ((IsWindow(hDldWnd)) AND (bUpdateMeter))
                    SendMessage(hDldWnd, MM_INC_PARTS_COMPLETE, 0, 0L);
                continue; // back up to top of read block loop
            }
        }

#ifdef INTERNAL
        AuxPrint("Gotta retry (%d).",nRetry);
#endif

        /** if we get to here then we gotta retry **/
        if(nRetry == MAXRETRIES) // tried long enuf
            END(FAIL)
        else
        {
            SendByte(hCom,NAK);
            SendByte(hCom,NAK);
            ++nRetry;
        }

    } /* end of read block loop */

    end:
    if (fpFull != -1)
        _lclose(fpFull);
    if (fpComp != -1)
        _lclose(fpComp);

    if (nRetval == FAIL)
    {
        if (bConnect)
        {
            SendByte(hCom,CTRLX);
            SendByte(hCom,CTRLX);
        }
        /* partial files may exist */
        if (szFullFileName)
            unlink(szDestFileName);
        if (szCompFileName)
            unlink(szCompFileName);
    }

    if (hCursor)
        SetCursor(hCursor);

    close_buffers();
    return (nRetval);
}

DWORD
expand_buf(BYTE *input_buffer, 
           WORD size, // of input
           int output_handle, // file to write to
           BOOL bLast) 
 {
  unsigned input_remaining; /* data remaining in input block */
  unsigned input_size;      /* size of current pass */
  unsigned buf_pos;         /* position in input buffer */
  unsigned output_size;     /* size of current output block */
  DWORD BytesOut = 0L;

    input_remaining = size;
    buf_pos = 0;    /* set buffer position to start */
    do 
    {
      input_size = input_remaining; /* maximum input = remaining input */
      output_size = sizeof(output_buffer); /* maximum output = size of buffer */
      dilute(input_buffer + buf_pos, &input_size,
            output_buffer,          &output_size,
            variables, (WORD) bLast);
      buf_pos += input_size;
      input_remaining -= input_size; /** input size is bytes processed **/
      _lwrite(output_handle, output_buffer, output_size); /* write out buffer */
      BytesOut += (DWORD)output_size;
     } while(input_remaining);
  return BytesOut;
 }

int
init_buffers(void)  /* allocate buffers + variable area */
 {
   DWORD variable_size;
#ifdef INTERNAL
  //AuxPrint("calling diluteWorkSize");
#endif
  variable_size = (DWORD)diluteWorkSize();
#ifdef INTERNAL
  //AuxPrint("allocating %x bytes for decompression",variable_size);
#endif
  if ((hvariables = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,variable_size)) == NULL) {
    SetErr(IDS_ERR_NO_MEMORY);
    return FAIL;
   }
  if ((variables = GlobalLock(hvariables)) == NULL) {
    SetErr(IDS_ERR_NO_MEMORY);
    return FAIL;
   }
   return OK;
 }



void
close_buffers(void)
{
  if (variables && hvariables)
    GlobalUnlock(hvariables);
  if (hvariables)
    GlobalFree(hvariables);
  hvariables = NULL;
  variables = NULL;
}

/***************************************************************************
 * CalcCRC
 *
 * DESCRIPTION:
 *  Calculate the cyclic redundancy check on an array of unsigned chars
 *  code ripped off from Galacticom XMODEM-CRC Protocol pg 183 'The Major BBS'
 *
 * AFFECTS:
 *   nothing
 *
 * RETURNS: int value representing CRC of string.
 *
 * HISTORY:
 *  Dec 29 '89 - R. Bellas - written
\**************************************************************************/

CRC
CalcCRC(BYTE *cCRCBuf,WORD dCRC_BUFFSIZE)
{
    CRC crc=0,i;    

    while(dCRC_BUFFSIZE--)
        {
         crc = crc^ (CRC)*cCRCBuf++ << 8;
         for(i=8;i--;)
            {
             if(crc&0x08000) 
                {
                 crc = (crc<<1)^0x1021;
                }
             else
                {
                 crc = crc<<1;
                }
            }
        }
#ifdef INTERNAL
        //AuxPrint("CRC (%x)",crc);
#endif
        return(crc);
}

#if 0
LONG FAR PASCAL 
WSCancelDlgProc (HWND hWndDlg, unsigned message, WORD wParam, LONG lParam) 
{
    static WORD wPartsInJob;
    static WORD wPartsComplete;
    char szPercentage[10];
    LONG lResult = 0;

    switch(message)
    {
      case WM_INITDIALOG:
      {
		   EnableWindow (GetDlgItem (hWndDlg, IDCANCEL), FALSE);

         CenterPopUp(hWndDlg);
         hDldWnd = hWndDlg;
         break;
      }

      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HBRUSH hBrush;
         DWORD dwColor;
         RECT rcClient, rcPrcnt;

         if (wPartsInJob == 0)
         {
            wPartsInJob = 1;
            wPartsComplete = 0;
         }
         wsprintf(szPercentage, "%d%%", (100 * wPartsComplete) / wPartsInJob);

         BeginPaint(hWndDlg, (LPPAINTSTRUCT)&ps);

         // Set-up default foreground and background text colors.
         SetBkColor(ps.hdc, GetSysColor(COLOR_WINDOW));
         SetTextColor(ps.hdc, GetSysColor(COLOR_WINDOWTEXT));

         // Send WM_CTLCOLOR message to parent in case parent want to 
         // use a different color in the Meter control.
         //hBrush = (HBRUSH) SendMessage(hWndDlg, WM_CTLCOLOR, ps.hdc,
            //MAKELONG(hWndMeter, CTLCOLOR_METER));

         // Always use brush returned by parent.
         //SelectObject(ps.hdc, hBrush);
         hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));

         SetTextAlign(ps.hdc, TA_CENTER | TA_TOP);

         // Invert the foreground and background colors.
         dwColor = GetBkColor(ps.hdc);
         SetBkColor(ps.hdc, SetTextColor(ps.hdc, dwColor));

         // Set rectangle coordinates to include only left
         // percentage of the window.
         GetClientRect(GetDlgItem(hWndDlg, ID_PERCENT_COMPLETE), (LPRECT)&rcClient);
         SetRect(&rcPrcnt, 0, 0,
            (rcClient.right * wPartsComplete) / wPartsInJob, rcClient.bottom);

         // Output the percentage value in the window.
         // Function also paints left part of window.
         ExtTextOut(ps.hdc, rcClient.right / 2,
            (rcClient.bottom - HIWORD(GetTextExtent(ps.hdc, "X", 1))) / 2,
            ETO_OPAQUE | ETO_CLIPPED, &rcPrcnt,
            szPercentage, lstrlen(szPercentage), NULL);

         // Adjust rectangle so that it includes the remaining 
         // percentage of the window.
         rcPrcnt.left = rcPrcnt.right;
         rcPrcnt.right = rcClient.right;

         // Invert the foreground and background colors.
         dwColor = GetBkColor(ps.hdc);
         SetBkColor(ps.hdc, SetTextColor(ps.hdc, dwColor));

         // Output the percentage value a second time in the window.
         // Function also paints right part of window.
         ExtTextOut(ps.hdc, rcClient.right / 2,
            (rcClient.bottom - HIWORD(GetTextExtent(ps.hdc, "X", 1))) / 2,
            ETO_OPAQUE | ETO_CLIPPED, &rcPrcnt,
            szPercentage, lstrlen(szPercentage), NULL);

         EndPaint(hWndDlg, &ps);
         break;
      }

      case MM_SET_PARTS_IN_JOB:
      case MM_SET_PARTS_COMPLETE:
      case MM_INC_PARTS_COMPLETE:
      {
         RECT rcClient;
         switch (message)
         {
            case MM_SET_PARTS_IN_JOB : wPartsInJob = wParam;
#ifdef INTERNAL
               AuxPrint("Initializing, PARTS_IN_JOB = %d", wPartsInJob);
#endif
               break;
            case MM_SET_PARTS_COMPLETE : wPartsComplete = wParam;
#ifdef INTERNAL
               AuxPrint("Initializing, PARTS_COMPLETE = %d", wPartsComplete);
#endif
               break;
            case MM_INC_PARTS_COMPLETE : wPartsComplete++;
#ifdef INTERNAL
               AuxPrint("Initializing, Incrementing PARTS_COMPLETE = %d", wPartsComplete);
#endif
               break;
         }

         GetClientRect(GetDlgItem(hWndDlg, ID_PERCENT_COMPLETE), (LPRECT)&rcClient);
         InvalidateRect(hWndDlg, (LPRECT)&rcClient, FALSE);
         UpdateWindow(hWndDlg);
         break;
      }

      case MM_GET_PARTS_IN_JOB:
         lResult = (LONG) wPartsInJob;
         break;

      case MM_GET_PARTS_COMPLETE:
         lResult = (LONG) wPartsComplete;
         break;

      case MM_SET_EST_TIME:
      {
         char str[10];
#ifdef INTERNAL
         AuxPrint("Initializing, time to %ld secs", lParam);
#endif
         SetWindowText(GetDlgItem(hWndDlg, ID_DLDTIME),
                        (LPSTR)GetTimeStr(lParam, str));
         lResult = OK;
         break;
      }

      case MM_SET_ICON:
      {
         char msgText[MAX_MSG_LEN];

		   if ( wParam == IDS_MSG_UPDATE_DATABASE )
          {
		        ShowWindow (GetDlgItem (hWndDlg, ID_DOWNLOAD), SW_HIDE);
              ShowWindow (GetDlgItem (hWndDlg, IDS_SHOPPERICON), SW_HIDE);
          }
          else
              ShowWindow (GetDlgItem (hWndDlg, IDS_DBICON), SW_HIDE);

         LoadString (hInstanceWS, wParam, msgText, sizeof (msgText));
         SetDlgItemText (hWndDlg, IDS_DESCRIPTION, msgText);

         lResult = OK;
         break;
      }

      case WM_COMMAND:
            switch (wParam)
            {
                case IDCANCEL:
                    SetDest (hWndDlg, NOBEEP, MB_ICONQUESTION | MB_YESNO, IDS_TITLE_APPNAME);
                    if (ErrMsg(IDS_MSG_QUERY_INTERRUPT) == IDYES)
                    {
                        DestroyWindow(hWndDlg);
                        hDldWnd = NULL;
                    }
                    break;
            }
            break;
        default:
            return(FALSE);
            break;
    } /* end switch message */
    return(lResult);
}
#endif
