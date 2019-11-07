/*
 * MUSCROLL.C
 *
 * Contains the main window procedure of the MicroScroll control
 * that handles mouse logic, and Windows messages.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 */

/* cek 9/16/92 - moved all into one source file */
#include "PRECOMP.H"
#include "ICEK.H"

/*
#include <windows.h>
*/

#include "muscrdll.h"

/*
 * MicroScrollWndProc
 *
 * Purpose:
 *  Window Procedure for the MicroScroll custom control.  Handles all
 *  messages like WM_PAINT just as a normal application window would.
 *  Any message not processed here should go to DefWindowProc.
 *
 * Parameters:
 *  hWnd            HWND handle to the control window.
 *  iMsg            UINT message identifier.
 *  wParam          UINT parameter of the message.
 *  lParam          LONG parameter of the message.
 *
 * Return Value:
 *  LONG            Varies with the message.
 *
 */
LRESULT CALLBACK MicroScrollWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
    LPMUSCROLL       pMS;
    #ifdef WIN32
    POINTS          pt;
    POINT           pt32 ;
    #else
    POINT           pt;
    #endif
    RECT            rect;
    int           x, y;
    int           cx, cy;
    UINT            wState;


    /*
     * Get a pointer to the MUSCROLL structure for this control.
     * Note that if we do this before WM_NCCREATE where we allocate
     * the memory, pMS will be NULL, which is not a problem since
     * we do not access it until after WM_NCCREATE.
     */
    pMS=(LPMUSCROLL)GetWindowLong(hWnd, GWL_MUSCROLL);


    //Let the API handler process WM_USER+xxxx messages
    if (iMsg >= WM_USER)
        return LMicroScrollAPI(hWnd, iMsg, wParam, lParam, pMS);


    //Handle standard Windows messages.
    switch (iMsg)
        {
        case WM_NCCREATE:
        case WM_CREATE:
            return LMicroScrollCreate(hWnd, iMsg, pMS, (LPCREATESTRUCT)lParam);

        case WM_NCDESTROY:
            //Free the control's memory.
            GlobalFreePtr( pMS );
            break;

        case WM_ERASEBKGND:
            /*
             * Eat this message to avoid erasing portions that
             * we are going to repaint in WM_PAINT.  Part of a
             * change-state-and-repaint strategy is to rely on
             * WM_PAINT to do anything visual, which includes
             * erasing invalid portions.  Letting WM_ERASEBKGND
             * erase the background is redundant.
             */
            break;


        case WM_PAINT:
            return LMicroScrollPaint(hWnd, pMS);


        case WM_ENABLE:
            /*
             * Handles disabling/enabling case.  Example of a
             * change-state-and-repaint strategy since we let the
             * painting code take care of the visuals.
             */
            if (wParam)
                StateClear(pMS, MUSTATE_GRAYED);
            else
                StateSet(pMS, MUSTATE_GRAYED);

            //Force a repaint since the control will look different.
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
            break;


        case WM_SHOWWINDOW:
            /*
             * Set or clear the hidden flag. Windows will
             * automatically force a repaint if we become visible.
             */
            if (wParam)
                StateClear(pMS, MUSTATE_HIDDEN);
            else
                StateSet(pMS, MUSTATE_HIDDEN);

            break;


        case WM_CANCELMODE:
            /*
             * IMPORTANT MESSAGE!  WM_CANCELMODE means that a
             * dialog or some other modal process has started.
             * we must make sure that we cancel any clicked state
             * we are in, kill the timers, and release the capture.
             */
            StateClear(pMS, MUSTATE_DOWNCLICK | MUSTATE_UPCLICK);
            KillTimer(hWnd, IDT_FIRSTCLICK);
            KillTimer(hWnd, IDT_HOLDCLICK);
            ReleaseCapture();
            break;


        case WM_TIMER:
            /*
             * We run two timers:  the first is the initial delay
             * after the first click before we begin repeating, the
             * second is the repeat rate.
             */
            if (wParam==IDT_FIRSTCLICK)
                {
                KillTimer(hWnd, wParam);
                SetTimer(hWnd, IDT_HOLDCLICK, CTICKS_HOLDCLICK, NULL);
                }

            /*
             * Send a new scroll message if the mouse is still in the
             * originally clicked area.
             */
            if (!StateTest(pMS, MUSTATE_MOUSEOUT))
                PositionChange(hWnd, pMS);

            break;


        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
            /*
             * When we get a mouse down message, we know that the mouse
             * is over the control.  We then do the following steps
             * to set up the new state:
             *  1.  Hit-test the coordinates of the click to
             *      determine in which half the click occurred.
             *  2.  Set the appropriate MUSTATE_*CLICK state
             *      and repaint that clicked half.  This is another
             *      example of a change-state-and-repaint strategy.
             *  3.  Send an initial scroll message.
             *  4.  Set the mouse capture.
             *  5.  Set the initial delay timer before repeating
             *      the scroll message.
             *
             * A WM_LBUTTONDBLCLK message means that the user clicked
             * the control twice in succession which we want to treat
             * like WM_LBUTTONDOWN.  This is safe since we will receive
             * WM_LBUTTONUP before the WM_LBUTTONDBLCLK.
             */

            //Get the mouse coordinates.

            x=LOWORD(lParam);

            y=HIWORD(lParam);


            /*
             * Only need to hit-test the upper half for a vertical
             * control or the left half for a horizontal control.
             */
            GetClientRect(hWnd, &rect);
            cx=rect.right  >> 1;
            cy=rect.bottom >> 1;

            if (MSS_VERTICAL & pMS->dwStyle)
                wState=(y > cy) ? MUSTATE_DOWNCLICK : MUSTATE_UPCLICK;
            else
                wState=(x > cx) ? MUSTATE_RIGHTCLICK : MUSTATE_LEFTCLICK;

            //Change-state-and-repaint
            StateSet(pMS, wState);
            ClickedRectCalc(hWnd, pMS, &rect);
            InvalidateRect(hWnd, &rect, TRUE);
            UpdateWindow(hWnd);

            PositionChange(hWnd, pMS);
            SetCapture(hWnd);
            SetTimer(hWnd, IDT_FIRSTCLICK, CTICKS_FIRSTCLICK, NULL);
            break;


        case WM_MOUSEMOVE:
            /*
             * On WM_MOUSEMOVE messages we want to know if the mouse
             * has moved out of the control when the control is in
             * a clicked state.  If the control has not been clicked,
             * then we have nothing to do.  Otherwise we want to set
             * the MUSTATE_MOUSEOUT flag and repaint so the button
             * visually comes up.
             */
            if (!StateTest(pMS, MUSTATE_CLICKED))
                break;


            //Get the area we originally clicked and the new POINT
            ClickedRectCalc(hWnd, pMS, &rect);

            #ifdef WIN32
            pt=MAKEPOINTS(lParam);
            #else
            pt=MAKEPOINT(lParam);
            #endif

            wState=pMS->wState;

            //Hit-Test the rectange and change the state if necessary.
            #ifdef WIN32
            pt32.x = pt.x ;
            pt32.y = pt.y ;
            if (PtInRect(&rect, pt32))
                StateClear(pMS, MUSTATE_MOUSEOUT);
            else
                StateSet(pMS, MUSTATE_MOUSEOUT);
            #else
            if (PtInRect(&rect, pt))
                StateClear(pMS, MUSTATE_MOUSEOUT);
            else
                StateSet(pMS, MUSTATE_MOUSEOUT);
            #endif

            /*
             * If the state changed, repaint the appropriate part of
             * the control.
             */
            if (wState!=pMS->wState)
                {
                InvalidateRect(hWnd, &rect, TRUE);
                UpdateWindow(hWnd);
                }

            break;


        case WM_LBUTTONUP:
            /*
             * A mouse button up event is much like WM_CANCELMODE since
             * we have to clean out whatever state the control is in:
             *  1.  Kill any repeat timers we might have created.
             *  2.  Release the mouse capture.
             *  3.  Clear the clicked states and repaint, another example
             *      of a change-state-and-repaint strategy.
             */
            KillTimer(hWnd, IDT_FIRSTCLICK);
            KillTimer(hWnd, IDT_HOLDCLICK);

            ReleaseCapture();



            /*
             * Repaint if necessary, only if we are clicked AND the mouse
             * is still in the boundaries of the control.
             */
            if (StateTest(pMS, MUSTATE_CLICKED) &&
                StateTest(pMS, ~MUSTATE_MOUSEOUT))
                {
                //Calculate the rectangle before clearing states.
                ClickedRectCalc(hWnd, pMS, &rect);

                //Clear the states so we repaint properly.
                StateClear(pMS, MUSTATE_MOUSEOUT);
                StateClear(pMS, MUSTATE_CLICKED);


                InvalidateRect(hWnd, &rect, TRUE);
                UpdateWindow(hWnd);
                }

            //Insure that we clear out the states.
            break;


        default:
            return DefWindowProc(hWnd, iMsg, wParam, lParam);
        }

    return 0L;
    }






/*
 * ClickedRectCalc
 *
 * Purpose:
 *  Calculates the rectangle of the clicked region based on the
 *  state flags MUSTATE_UPCLICK, MUSTATE_DOWNCLICK, MUSTATE_LEFTCLICK,
 *  and MUSTATE_RIGHTLICK, depending on the style.
 *
 * Parameter:
 *  hWnd            HWND handle to the control window.
 *  lpRect          LPRECT rectangle structure to fill.
 *
 * Return Value:
 *  void
 *
 */

void PASCAL ClickedRectCalc(HWND hWnd, LPMUSCROLL pMS, LPRECT lpRect)
    {
    int       cx, cy;

    GetClientRect(hWnd, lpRect);
    cx=lpRect->right  >> 1;
    cy=lpRect->bottom >> 1;

    if (MSS_VERTICAL & pMS->dwStyle)
        {
        if (StateTest(pMS, MUSTATE_DOWNCLICK))
            lpRect->top=cy;

        if (StateTest(pMS, MUSTATE_UPCLICK))
            lpRect->bottom=1+cy;
        }
    else
        {
        //MSS_HORIZONTAL
        if (StateTest(pMS, MUSTATE_RIGHTCLICK))
            lpRect->left=cx;

        if (StateTest(pMS, MUSTATE_LEFTCLICK))
            lpRect->right=1+cx;
        }

    return;
    }





/*
 * PositionChange
 *
 * Purpose:
 *  Checks what part of the control is clicked, modifies the current
 *  position accordingly (taking MSS_INVERTRANGE into account) and
 *  sends an appropriate message to the associate.  For MSS_VERTICAL
 *  controls we send WM_VSCROLL messages and for MSS_HORIZONTAL controls
 *  we send WM_HSCROLL.
 *
 *  The scroll code in the message is always SB_LINEUP for the upper
 *  or left half of the control (vertical and horizontal, respectively)
 *  and SB_LINEDOWN for the bottom or right half.
 *
 *  This function does not send a message if the position is pegged
 *  on the minimum or maximum of the range if MSS_NOPEGSCROLL is
 *  set in the style bits.
 *
 * Parameters:
 *  hWnd            HWND of the control.
 *  pMS             LPMUSCROLL pointer to control data structure.
 *
 * Return Value:
 *  void
 */

void PASCAL PositionChange(HWND hWnd, LPMUSCROLL pMS)
    {
    UINT         wScrollMsg;
    UINT         wScrollCode;
    BOOL         fPegged=FALSE;

    if (StateTest(pMS, MUSTATE_UPCLICK | MUSTATE_LEFTCLICK))
        wScrollCode=SB_LINEUP;

    if (StateTest(pMS, MUSTATE_DOWNCLICK | MUSTATE_RIGHTCLICK))
        wScrollCode=SB_LINEDOWN;

    wScrollMsg=(MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;

    /*
     * Modify the current position according to the following rules:
     *
     * 1. On SB_LINEUP with an inverted range, increment the position.
     *    If the position is already at the maximum, set the pegged flag.
     *
     * 2. On SB_LINEUP with an normal range, decrement the position.
     *    If the position is already at the minimum, set the pegged flag.
     *
     * 3. On SB_LINEDOWN with an inverted range, treat like SB_LINEUP
     *    with a normal range.
     *
     * 4. On SB_LINEDOWN with an normal range, treat like SB_LINEUP
     *    with an inverted range.
     */

    if (wScrollCode==SB_LINEUP)
        {
        if (MSS_INVERTRANGE & pMS->dwStyle)
            {
            if (pMS->iPos==pMS->iMax)
                fPegged=TRUE;
            else
                pMS->iPos++;
            }
        else
            {
            if (pMS->iPos==pMS->iMin)
                fPegged=TRUE;
            else
                pMS->iPos--;
            }
        }
    else
        {
        if (MSS_INVERTRANGE & pMS->dwStyle)
            {
            if (pMS->iPos==pMS->iMin)
                fPegged=TRUE;
            else
                pMS->iPos--;
            }
        else
            {
            if (pMS->iPos==pMS->iMax)
                fPegged=TRUE;
            else
                pMS->iPos++;
            }
        }


    /*
     * Send a message if we changed and are not pegged, or did not change
     * and MSS_NOPEGSCROLL is clear.
     */
    if (!fPegged || !(MSS_NOPEGSCROLL & pMS->dwStyle))
        {
        #ifdef WIN32
        SendMessage(pMS->hWndAssociate, wScrollMsg,
                    MAKELPARAM(wScrollCode, pMS->iPos), (LPARAM)hWnd );
        #else
        SendMessage(pMS->hWndAssociate, wScrollMsg,
                    wScrollCode, MAKELPARAM(pMS->iPos, hWnd));
        #endif
        }

    return;
    }


/*
 * MSAPI.C
 *
 * Contains the API implementation of the MicroScroll custom
 * control, including functional messages and function message
 * equivalents.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 */

/*
#include <windows.h>
#include "muscrdll.h"
*/

/*
 * LMicroScrollAPI
 *
 * Purpose:
 *  Processes any control-specific function messages for the
 *  MicroScroll control.
 *
 * Parameters:
 *  hWnd            HWND handle to the control window.
 *  iMsg            UINT message identifier.
 *  wParam          UINT parameter of the message.
 *  lParam          LONG parameter of the message.
 *  pMS             LPMUSCROLL pointer to control-specific data.
 *
 * Return Value:
 *  LONG            Varies with the message.
 *
 */



LONG PASCAL LMicroScrollAPI(HWND hWnd, UINT iMsg, UINT wParam,
                            LONG lParam, LPMUSCROLL pMS)
    {
    DWORD           dwT;
    COLORREF        cr;
    HWND            hWndT;
    UINT            iMin, iMax;
    UINT            iPos;
    UINT            wID;

    switch (iMsg)
        {
        case MSM_HWNDASSOCIATESET:
            //Change the associate window of this control.
            if (!IsWindow((HWND)wParam))
                return -1;

            //Save old window handle.
            hWndT=pMS->hWndAssociate;

            //Get our ID value, then send WM_COMMAND notifications.
            wID=GetWindowID(hWnd);

            #ifdef WIN32
            SendMessage(hWndT, WM_COMMAND, 
                        MAKELPARAM( wID, MSN_ASSOCIATELOSS ), (LPARAM)hWnd ) ;
            #else
            SendMessage(hWndT, WM_COMMAND, wID,
                        MAKELONG(hWnd, MSN_ASSOCIATELOSS));
            #endif

            pMS->hWndAssociate=(HWND)wParam;

            #ifdef WIN32
            SendMessage(hWndT, WM_COMMAND, 
                        MAKELPARAM( wID, MSN_ASSOCIATEGAIN ), (LPARAM)hWnd ) ;
            #else
            SendMessage(pMS->hWndAssociate, WM_COMMAND, wID,
                        MAKELONG(hWnd, MSN_ASSOCIATEGAIN));
            #endif

            return MAKELONG(hWndT, 0);


        case MSM_HWNDASSOCIATEGET:
            return MAKELONG(pMS->hWndAssociate, 0);


        case MSM_DWRANGESET:
            /*
             * Set the new range, sending the appropriate notifications.
             * Also send a scroll message if the position has to change.
             * If the minimum is greater than the max, return error.
             */
            if (LOWORD(lParam) >= HIWORD(lParam))
                return -1L;

            wID=GetWindowID(hWnd);

            #ifdef WIN32
            SendMessage(hWndT, WM_COMMAND, 
                        MAKELPARAM( wID, MSN_RANGECHANGE ), (LPARAM)hWnd ) ;
            #else
            SendMessage(pMS->hWndAssociate, WM_COMMAND, wID,
                        MAKELONG(hWnd, MSN_RANGECHANGE));
            #endif

            //Save old values.
            iMin=pMS->iMin;
            iMax=pMS->iMax;

            pMS->iMin=LOWORD(lParam);
            pMS->iMax=HIWORD(lParam);

            /*
             * If current position is outside of new range, force it to
             * the average of the range, otherwise leave it be.
             */
            if ((pMS->iMin >= pMS->iPos) ||
                (pMS->iMax <= pMS->iPos))
                {
                pMS->iPos=(pMS->iMin + pMS->iMax) >> 1;

                //Send a scroll message if we change position.

                iMsg=(MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;
                #ifdef WIN32
                wParam=MAKELONG( SB_THUMBTRACK, pMS->iPos );
                lParam=0 ;
                #else
                wParam=SB_THUMBTRACK ;
                lParam=MAKELONG( pMS->iPos, 0 ) ;
                #endif

                SendMessage(pMS->hWndAssociate, iMsg, wParam, lParam);
                }

            //Return old range.
            return MAKELONG(iMin, iMax);

        case MSM_DWRANGEGET:
            return MAKELONG(pMS->iMin, pMS->iMax);


        case MSM_WCURRENTPOSSET:
            /*
             * Set the new position if it falls within the valid range,
             * sending the appropriate scroll message.
             */

            //Save current position
            iPos=pMS->iPos;

            if ((pMS->iMin <= wParam) && (pMS->iMax >= wParam))
                {
                pMS->iPos=wParam;

                iMsg=(MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;

                #ifdef WIN32
                wParam=MAKELONG( SB_THUMBTRACK, pMS->iPos );
                lParam=0 ;
                #else
                wParam=SB_THUMBTRACK;
                lParam=MAKELONG(pMS->iPos, 0);
                #endif

                SendMessage(pMS->hWndAssociate, iMsg, wParam, lParam);

                //Return old position.
                return MAKELONG(iPos, 0);
                }

            //Invalid position.
            return -1;

        case MSM_WCURRENTPOSGET:
            return MAKELONG(pMS->iPos, 0);


        case MSM_FNOPEGSCROLLSET:
            /*
             * Set the MSS_NOPEGSCROLL style to the value in
             * lParam which is zero or MSS_NOPEGSCROLL.
             */
            dwT=pMS->dwStyle & MSS_NOPEGSCROLL;

            //Either set of clear the style.
            if ((BOOL)wParam)
                pMS->dwStyle |= MSS_NOPEGSCROLL;
            else
                pMS->dwStyle &= ~MSS_NOPEGSCROLL;

            //Return TRUE or FALSE if the bit was or wasn't set
            return (dwT ? 1L : 0L);

        case MSM_FNOPEGSCROLLGET:
            return (pMS->dwStyle & MSS_NOPEGSCROLL);


        case MSM_FINVERTRANGESET:
            /*
             * Set the MSS_INVERTRANGE style to the value in
             * lParam which is zero or MSS_INVERTRANGE.
             */
            dwT=pMS->dwStyle & MSS_INVERTRANGE;

            //Either set of clear the style.
            if ((BOOL)wParam)
                pMS->dwStyle |= MSS_INVERTRANGE;
            else
                pMS->dwStyle &= ~MSS_INVERTRANGE;

            //Return TRUE or FALSE if the bit was or wasn't set
            return (dwT ? 1L : 0L);


        case MSM_FINVERTRANGEGET:
            return (pMS->dwStyle & MSS_INVERTRANGE);


        case MSM_CRCOLORSET:
            if (wParam >= CCOLORS)
                return 0L;

            cr=pMS->rgCr[wParam];

            //If -1 is set in rgCr the paint procedure uses a default.
            pMS->rgCr[wParam]=(COLORREF)lParam;

            //Force repaint since we changed a state.
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);

            //Return the old color.
            return (LONG)cr;

        case MSM_CRCOLORGET:
            if (wParam >= CCOLORS)
                return 0L;

            return (LONG)pMS->rgCr[wParam];
        }
    return 0L;
    }




/*
 * Message API Functions
 *
 * The advantage of using a function instead of SendMessage is that
 * you get type checking on the parameters and the return value.'
 *
 * Header comments are provided on these functions in pairs.  All
 * functions take hWnd (control handle), and the Set functions
 * usually take an extra paarameter containing the new value.
 *
 */


/*
 * MSHwndAssociateSet
 * MSHwndAssociateGet
 *
 * Purpose:
 *  Change or retrieve the associate window of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  hWndAssociate   HWND of new associate.
 *
 * Return Value:
 *  HWND            Handle of previous associate (set) or current
 *                  associate (set).
 */

HWND WINAPI MSHwndAssociateSet(HWND hWnd, HWND hWndAssociate)
    {
    return (HWND)SendMessage(hWnd, MSM_HWNDASSOCIATESET,
                             (UINT)hWndAssociate, 0L);
    }

HWND WINAPI MSHwndAssociateGet(HWND hWnd)
    {
    return (HWND)SendMessage(hWnd, MSM_HWNDASSOCIATEGET, 0, 0L);
    }


/*
 * MSRangeSet
 * MSRangeGet
 *
 * Purpose:
 *  Change or retrieve the range of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  iMin            UINT new minimum of the range.
 *  iMax            UINT new maximum of the range.
 *
 * Return Value:
 *  DUINT           Low-order word contains the previous (set) or
 *                  current (get) minimum, high-order word contains
 *                  the previous or current maximum.
 */

DWORD WINAPI MSDwRangeSet(HWND hWnd, UINT iMin, UINT iMax)
    {
    return (DWORD)SendMessage(hWnd, MSM_DWRANGESET, 0, MAKELONG(iMin, iMax));
    }

DWORD WINAPI MSDwRangeGet(HWND hWnd)
    {
    return (DWORD)SendMessage(hWnd, MSM_DWRANGEGET, 0, 0L);
    }


/*
 * MSWCurrentPosSet
 * MSWCurrentPosGet
 *
 * Purpose:
 *  Change or retrieve the current position of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  iPos            UINT new position to set.
 *
 * Return Value:
 *  UINT            Previous (set) or current (get) position.
 *
 */

UINT WINAPI MSWCurrentPosSet(HWND hWnd, UINT iPos)
    {
    return (UINT)SendMessage(hWnd, MSM_WCURRENTPOSSET, iPos, 0L);
    }

UINT WINAPI MSWCurrentPosGet(HWND hWnd)
    {
    return (UINT)SendMessage(hWnd, MSM_WCURRENTPOSGET, 0, 0L);
    }




/*
 * MSFNoPegScrollSet
 * MSFNoPegScrollGet
 *
 * Purpose:
 *  Change or retrieve the state of the MSS_NOPEGSCROLL style bit.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  fNoPegScroll    BOOL flag to set (TRUE) or clear (FALSE) the style.
 *
 * Return Value:
 *  BOOL            Previous (set) or current (get) state of this
 *                  style bit, either TRUE for on, FALSE for off.
 */

BOOL WINAPI MSFNoPegScrollSet(HWND hWnd, BOOL fNoPegScroll)
    {
    return (BOOL)SendMessage(hWnd, MSM_FNOPEGSCROLLSET, fNoPegScroll, 0L);
    }

BOOL WINAPI MSFNoPegScrollGet(HWND hWnd)
    {
    return (BOOL)SendMessage(hWnd, MSM_FNOPEGSCROLLGET, 0, 0L);
    }



/*
 * MSFInvertRangeSet
 * MSFInvertRangeGet
 *
 * Purpose:
 *  Change or retrieve the state of the MSS_INVERTRANGE style bit.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  fInvertRange    BOOL flag to set (TRUE) or clear (FALSE) the style.
 *
 * Return Value:
 *  BOOL            Previous (set) or current (get) state of this
 *                  style bit, either TRUE for on, FALSE for off.
 */

BOOL WINAPI MSFInvertRangeSet(HWND hWnd, BOOL fInvertRange)
    {
    return (BOOL)SendMessage(hWnd, MSM_FINVERTRANGESET, fInvertRange, 0L);
    }

BOOL WINAPI MSFInvertRangeGet(HWND hWnd)
    {
    return (BOOL)SendMessage(hWnd, MSM_FINVERTRANGEGET, 0, 0L);
    }



/*
 * MSCrColorSet
 * MSCrColorGet
 *
 * Purpose:
 *  Change or retrieve a configurable color.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *  iColor          UINT index to the control to modify or retrieve.
 *
 * Set Parameters:
 *  cr              COLORREF new value of the color.
 *
 * Return Value:
 *  COLORREF        Previous (set) or current (get) color value.
 *
 */

COLORREF WINAPI MSCrColorSet(HWND hWnd, UINT iColor, COLORREF cr)
    {
    return (BOOL)SendMessage(hWnd, MSM_CRCOLORSET, iColor, (LONG)cr);
    }

COLORREF WINAPI MSCrColorGet(HWND hWnd, UINT iColor)
    {
    return (COLORREF)SendMessage(hWnd, MSM_CRCOLORGET, iColor, 0L);
    }


/*
 * PAINT.C
 *
 * Contains any code related to MicroScroll visuals, primarily
 * the WM_PAINT handler.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 */

/*
#include <windows.h>
#include "muscrdll.h"
*/

//Array of default colors, matching the order of MSCOLOR_* values.
UINT rgColorDef[CCOLORS]={
                         COLOR_BTNFACE,
                         COLOR_BTNTEXT,
                         COLOR_BTNSHADOW,
                         (UINT)(HBRUSH)-1,
                         COLOR_WINDOWFRAME
                         };

/*
 * LMicroScrollPaint
 *
 * Purpose:
 *  Handles all WM_PAINT messages for the control and paints
 *  the control for the current state, whether it be clicked
 *  or disabled.
 *
 *  PLEASE NOTE!
 *  This painting routine makes no attempt at optimizations
 *  and is intended for demonstration and education.
 *
 * Parameters:
 *  hWnd            HWND Handle to the control.
 *  pMS             LPMUSCROLL control data pointer.
 *
 * Return Value:
 *  LONG            0L.
 */

LONG PASCAL LMicroScrollPaint(HWND hWnd, LPMUSCROLL pMS)
    {
    PAINTSTRUCT ps;
    LPRECT      lpRect;
    RECT        rect;
    HDC         hDC;
    COLORREF    rgCr[CCOLORS];
    HPEN        rgHPen[CCOLORS];
    UINT        iColor;

    HBRUSH      hBrushArrow;
    HBRUSH      hBrushFace;
    HBRUSH      hBrushBlack;

    POINT       rgpt1[3];
    POINT       rgpt2[3];

    UINT        xAdd1=0, yAdd1=0;
    UINT        xAdd2=0, yAdd2=0;

    UINT        cx,  cy;    //Whole dimensions
    UINT        cx2, cy2;   //Half dimensions
    UINT        cx4, cy4;   //Quarter dimensions


    lpRect=&rect;

    hDC=BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, lpRect);

    /*
     * Get colors that we'll need.  We do not want to cache these
     * items since we may our top-level parent window may have
     * received a WM_WININICHANGE message at which time the control
     * is repainted.  Since this control never sees that message,
     * we cannot assume that colors will remain the same throughout
     * the life of the control.
     *
     * We use the system color if pMS->rgCr[i] is -1, otherwise we
     * use the color in pMS->rgCr[i].
     */

    for (iColor=0; iColor < CCOLORS; iColor++)
        {
        if (-1L==pMS->rgCr[iColor])
            {
            //HACK:  Windows 3.0 has no system color for button highlight
            if (-1==rgColorDef[iColor])
                rgCr[iColor]=RGB(255, 255, 255);
            else
                rgCr[iColor]=GetSysColor(rgColorDef[iColor]);
            }
        else
            rgCr[iColor]=pMS->rgCr[iColor];

        rgHPen[iColor]=CreatePen(PS_SOLID, 1, rgCr[iColor]);
        }

    hBrushFace =CreateSolidBrush(rgCr[MSCOLOR_FACE]);
    hBrushArrow=CreateSolidBrush(rgCr[MSCOLOR_ARROW]);
    hBrushBlack=GetStockObject(BLACK_BRUSH);

    /*
     * These values are extremely cheap to calculate for the amount
     * we are going to use them.
     */
    cx =lpRect->right  - lpRect->left;
    cy =lpRect->bottom - lpRect->top;
    cx2=cx  >> 1;
    cy2=cy  >> 1;
    cx4=cx2 >> 1;
    cy4=cy2 >> 1;


    /*
     * If one half is depressed, set the x/yAdd varaibles that we use
     * to shift the small arrow image down and right.
     */
    if (!StateTest(pMS, MUSTATE_MOUSEOUT))
        {
        if (StateTest(pMS, MUSTATE_UPCLICK | MUSTATE_LEFTCLICK))
            {
            xAdd1=1;
            yAdd1=1;
            }

        if (StateTest(pMS, MUSTATE_DOWNCLICK | MUSTATE_RIGHTCLICK))
            {
            xAdd2=1;
            yAdd2=1;
            }
        }


    //Draw the face color and the outer frame
    SelectObject(hDC, hBrushFace);
    SelectObject(hDC, rgHPen[MSCOLOR_FRAME]);
    Rectangle(hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);


    //Draw the arrows depending on the orientation.
    if (MSS_VERTICAL & pMS->dwStyle)
        {
        //Draw the horizontal center line.

        MoveToEx(hDC, 0,  cy2, NULL);
        LineTo(hDC, cx, cy2);

        /*
         * We do one of three modifications for drawing the borders:
         *  1) Both halves un-clicked.
         *  2) Top clicked,   bottom unclicked.
         *  3) Top unclicked, bottom clicked.
         *
         * Case 1 is xAdd1==xAdd2==0
         * Case 2 is xAdd1==1, xAdd2=0
         * Case 3 is xAdd1==0, xAdd2==1
         *
         */

        //Draw top and bottom buttons borders.
        Draw3DButtonRect(hDC, rgHPen[MSCOLOR_HIGHLIGHT],
                         rgHPen[MSCOLOR_SHADOW],
                         0,  0,  cx-1, cy2,  xAdd1);

        Draw3DButtonRect(hDC, rgHPen[MSCOLOR_HIGHLIGHT],
                         rgHPen[MSCOLOR_SHADOW],
                         0, cy2, cx-1, cy-1, xAdd2);


        //Select default line color.
        SelectObject(hDC, rgHPen[MSCOLOR_ARROW]);

        //Draw the arrows depending on the enable state.
        if (StateTest(pMS, MUSTATE_GRAYED))
            {
            /*
             * Draw arrow color lines in the upper left of the
             * top arrow and on the top of the bottom arrow.
             * Pen was already selected as a default.
             */

            MoveToEx(hDC, cx2,   cy4-2, NULL);      //Top arrow
            LineTo(hDC, cx2-3, cy4+1);

            MoveToEx(hDC, cx2-3, cy2+cy4-2, NULL);  //Bottom arrow
            LineTo(hDC, cx2+3, cy2+cy4-2);

            /*
             * Draw highlight color lines in the bottom of the
             * top arrow and on the ;pwer right of the bottom arrow.
             */
            SelectObject(hDC, rgHPen[MSCOLOR_HIGHLIGHT]);

            MoveToEx(hDC,   cx2-3, cy4+1,NULL);      //Top arrow
            LineTo(hDC,   cx2+3, cy4+1);

            MoveToEx(hDC,   cx2+3, cy2+cy4-2, NULL);  //Bottom arrow
            LineTo(hDC,   cx2,   cy2+cy4+1);
            SetPixel(hDC, cx2,   cy2+cy4+1, rgCr[MSCOLOR_HIGHLIGHT]);
            }
        else
            {
            //Top arrow polygon
            rgpt1[0].x=xAdd1+cx2;
            rgpt1[0].y=yAdd1+cy4-2;
            rgpt1[1].x=xAdd1+cx2-3;
            rgpt1[1].y=yAdd1+cy4+1;
            rgpt1[2].x=xAdd1+cx2+3;
            rgpt1[2].y=yAdd1+cy4+1;

            //Bottom arrow polygon
            rgpt2[0].x=xAdd2+cx2;
            rgpt2[0].y=yAdd2+cy2+cy4+1;
            rgpt2[1].x=xAdd2+cx2-3;
            rgpt2[1].y=yAdd2+cy2+cy4-2;
            rgpt2[2].x=xAdd2+cx2+3;
            rgpt2[2].y=yAdd2+cy2+cy4-2;

            //Draw the arrows
            SelectObject(hDC, hBrushArrow);
            Polygon(hDC, (LPPOINT)rgpt1, 3);
            Polygon(hDC, (LPPOINT)rgpt2, 3);
            }
        }
    else
        {
        //Draw the vertical center line, assume the frame color is selected.

        MoveToEx(hDC, cx2, 0, NULL);
        LineTo(hDC, cx2, cy);

        /*
         * We do one of three modifications for drawing the borders:
         *  1) Both halves un-clicked.
         *  2) Left clicked,   right unclicked.
         *  3) Left unclicked, right clicked.
         *
         * Case 1 is xAdd1==xAdd2==0
         * Case 2 is xAdd1==1, xAdd2=0
         * Case 3 is xAdd1==0, xAdd2==1
         *
         */

        //Draw left and right buttons borders.
        Draw3DButtonRect(hDC, rgHPen[MSCOLOR_HIGHLIGHT],
                         rgHPen[MSCOLOR_SHADOW],
                         0,   0, cx2,  cy-1, xAdd1);

        Draw3DButtonRect(hDC, rgHPen[MSCOLOR_HIGHLIGHT],
                         rgHPen[MSCOLOR_SHADOW],
                         cx2, 0, cx-1, cy-1, xAdd2);


        //Select default line color.
        SelectObject(hDC, rgHPen[MSCOLOR_ARROW]);

        //Draw the arrows depending on the enable state.
        if (StateTest(pMS, MUSTATE_GRAYED))
            {
            /*
             * Draw arrow color lines in the upper left of the
             * left arrow and on the left of the right arrow.
             * Pen was already selected as a default.
             */

            MoveToEx(hDC, cx4-2,     cy2, NULL);        //Left arrow
            LineTo(hDC, cx4+1,     cy2-3);

            MoveToEx(hDC, cx2+cx4-2, cy2-3, NULL);      //Right arrow
            LineTo(hDC, cx2+cx4-2, cy2+3);

            /*
             * Draw highlight color lines in the bottom of the
             * top arrow and on the ;pwer right of the bottom arrow.
             */
            SelectObject(hDC, rgHPen[MSCOLOR_HIGHLIGHT]);

            MoveToEx(hDC, cx4+1,     cy2-3, NULL);
            LineTo(hDC, cx4+1,     cy2+3);

            MoveToEx(hDC, cx2+cx4+1, cy2, NULL);
            LineTo(hDC, cx2+cx4-2, cy2+3);
            }
        else
            {
            //Left arrow polygon
            rgpt1[0].x=xAdd1+cx4-2;
            rgpt1[0].y=yAdd1+cy2;
            rgpt1[1].x=xAdd1+cx4+1;
            rgpt1[1].y=yAdd1+cy2+3;
            rgpt1[2].x=xAdd1+cx4+1;
            rgpt1[2].y=yAdd1+cy2-3;

            //Right arrow polygon
            rgpt2[0].x=xAdd2+cx2+cx4+1;
            rgpt2[0].y=yAdd2+cy2;
            rgpt2[1].x=xAdd2+cx2+cx4-2;
            rgpt2[1].y=yAdd2+cy2+3;
            rgpt2[2].x=xAdd2+cx2+cx4-2;
            rgpt2[2].y=yAdd2+cy2-3;

            //Draw the arrows
            SelectObject(hDC, hBrushArrow);
            Polygon(hDC, (LPPOINT)rgpt1, 3);
            Polygon(hDC, (LPPOINT)rgpt2, 3);
            }
        }

    //Clean up
    EndPaint(hWnd, &ps);

    DeleteObject(hBrushFace);
    DeleteObject(hBrushArrow);

    for (iColor=0; iColor < CCOLORS; iColor++)
        DeleteObject(rgHPen[iColor]);

    return 0L;
    }




/*
 * Draw3DButtonRect
 *
 * Purpose:
 *  Draws the 3D button look within a given rectangle.  This rectangle
 *  is assumed to be bounded by a one pixel black border, so everything
 *  is bumped in by one.
 *
 * Parameters:
 *  hDC         DC to draw to.
 *  hPenHigh    HPEN highlight color pen.
 *  hPenShadow  HPEN shadow color pen.
 *  x1          UINT Upper left corner x.
 *  y1          UINT Upper left corner y.
 *  x2          UINT Lower right corner x.
 *  y2          UINT Lower right corner y.
 *  fClicked    BOOL specifies if the button is down or not (TRUE==DOWN)
 *
 * Return Value:
 *  void
 *
 */

void PASCAL Draw3DButtonRect(HDC hDC, HPEN hPenHigh, HPEN hPenShadow,
                             UINT x1, UINT y1, UINT x2, UINT y2,
                             BOOL fClicked)
    {
    HPEN        hPenOrg;

    //Shrink the rectangle to account for borders.
    x1+=1;
    x2-=1;
    y1+=1;
    y2-=1;

    hPenOrg=SelectObject(hDC, hPenShadow);

    if (fClicked)
        {
        //Shadow on left and top edge when clicked.
        MoveToEx(hDC, x1, y2, NULL);
        LineTo(hDC, x1, y1);
        LineTo(hDC, x2+1, y1);
        }
    else
        {
        //Lowest shadow line.
        MoveToEx(hDC, x1, y2, NULL);
        LineTo(hDC, x2, y2);
        LineTo(hDC, x2, y1-1);

        //Upper shadow line.
        MoveToEx(hDC, x1+1, y2-1, NULL);
        LineTo(hDC, x2-1, y2-1);
        LineTo(hDC, x2-1, y1);

        SelectObject(hDC, hPenHigh);

        //Upper highlight line.
        MoveToEx(hDC, x1,   y2-1, NULL);
        LineTo(hDC, x1,   y1);
        LineTo(hDC, x2,   y1);
        }

    SelectObject(hDC, hPenOrg);
    return;
    }


BOOL WINAPI FRegisterControl(HANDLE hInstance)
    {
    static BOOL     fRegistered=FALSE;
    WNDCLASS        wc;
    char            szClass[40];

    if (!fRegistered)
        {
        //Load the class name
        if (0==LoadString(hInstance, IDS_CLASSNAME, szClass, 40))
            return FALSE;

        wc.lpfnWndProc   = MicroScrollWndProc;
        wc.cbClsExtra    = CBCLASSEXTRA;
        wc.cbWndExtra    = CBWINDOWEXTRA;
        wc.hInstance     = hInstance;
        wc.hIcon         = NULL;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)((UINT)COLOR_BTNFACE+1);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = szClass;
        wc.style         = CS_DBLCLKS | CS_GLOBALCLASS |
                           CS_VREDRAW | CS_HREDRAW;

        fRegistered=RegisterClass(&wc);
        }

    return fRegistered;
    }



/*
 * LMicroScrollCreate
 *
 * Purpose:
 *  Handles both WM_NCCREATE and WM_CREATE messages:
 *    WM_NCCREATE:  Allocate control MUSCROLL structure.  Any sort
 *                  of initialization that should force the create
 *                  to fail should be included in this case.
 *
 *    WM_CREATE:    Fill the MUSCROLL structure; parse the text
 *                  for initial min, max, and position for the
 *                  MSS_TEXTHASRANGE style; clear all states;
 *                  set the expansion handle to NULL.
 *
 * Parameters:
 *  hWnd            HWND handle to the control window.
 *  iMsg            UINT message, either WM_NCCREATE or WM_CREATE.
 *  pMS             LPMUSCROLL pointer to existing MUSCROLL structure.
 *
 * Return Value:
 *  LONG            0L if memory could not be allocated on WM_NCCREATE.
 *                  1L in all other cases.
 */

LONG WINAPI LMicroScrollCreate(HWND hWnd, UINT iMsg, LPMUSCROLL pMS,
                               LPCREATESTRUCT lpCreate)
    {
    BOOL            fTextRange;
    UINT            iMin, iMax, iPos;


    if (WM_NCCREATE==iMsg)
        {
        /*
         * Allocate extra FIXED memory for the control's MUSCROLL
         * structure and store that handle in the minimal window
         * extra bytes allocated for the control.
         *
         * Note that LPTR includes LMEM_ZEROINIT, so all fields are
         * initially zero.
         */
        pMS = GlobalAllocPtr( GHND, CBMUSCROLL ) ;
        if (NULL==pMS)
            return 0L;

        SetWindowLong(hWnd, GWL_MUSCROLL, (LONG)pMS);
        }

    if (WM_CREATE==iMsg)
        {
        //Our associate is the parent by default.
        pMS->hWndAssociate=(HWND)GetParent(hWnd);


        //Copy styles
        pMS->dwStyle  =lpCreate->style;

        /*
         * Enforce exclusive MSS_VERTICAL and MSS_HORIZONTAL,
         * defaulting to MSS_VERTICAL.
         */
        if ((MSS_VERTICAL & pMS->dwStyle) && (MSS_HORIZONTAL & pMS->dwStyle))
            pMS->dwStyle &= ~MSS_HORIZONTAL;

        //Either parse the text or use defaults for initial range.
        fTextRange=FALSE;

        if (MSS_TEXTHASRANGE & pMS->dwStyle)
            {
            fTextRange=FTextParse((LPSTR)lpCreate->lpszName,
                                  &iMin, &iMax, &iPos);

            /*
             * Verify that the position is in the given range.  If
             * the position is outside the range, force it to the
             * middle.
             */
            if (fTextRange)
                {
                if (iPos < iMin || iPos > iMax)
                    iPos=(iMin + iMax) >> 1;
                }
            }



        /*
         * Use defaults if we never had MSS_TEXTHASRANGE or
         * FTextParse failed.
         */
        pMS->iMin=(fTextRange ? iMin : IDEFAULTMIN);
        pMS->iMax=(fTextRange ? iMax : IDEFAULTMAX);
        pMS->iPos=(fTextRange ? iPos : IDEFAULTPOS);

        //Clear out all initial states.
        StateClear(pMS, MUSTATE_ALL);

        //Indicate that all colors are defaults.
        for (iMin=0; iMin < CCOLORS; iMin++)
            pMS->rgCr[iMin]=(COLORREF)-1L;
        }

    return 1L;
    }



/*
 * FTextParse
 *
 * Purpose:
 *  Parses window text for a valid range and initial position.
 *  This function is used when creating the control or setting the
 *  window text to set the initial range and position but is also
 *  used to validate text entered in the Style dialog in the Dialog
 *  Editor interface if the MSM_TEXTHASRANGE style is selected.
 *
 *  The range and position must all be valid for any change to
 *  occur in piMin, piMax, and piPos.
 *
 * Parameters:
 *  psz             LPSTR pointer to the window text to parse out
 *                  the range and the position.
 *  piMin           LPINT pointer to location to store minimum.
 *  piMax           LPINT pointer to location to store maximum.
 *  piPos           LPINT pointer to location to store position.
 *
 * Return Value:
 *  BOOL            TRUE if the function successfully initializes
 *                  the range and position. FALSE if any part of
 *                  the text is not a valid number between comma
 *                  delimeters.
 */

BOOL WINAPI FTextParse(LPSTR psz, LPINT piMin, LPINT piMax, LPINT piPos)
    {
    int          iMin;
    int          iMax;
    int          iCur;

    if (psz==NULL)
        return FALSE;

    /*
     * Parse off the bottom of the range.  Note that we depend
     * on WTranslateUpToChar to modify psz to point to the character
     * after the delimeter which is why we pass &psz.
     */
    iMin=WTranslateUpToChar(&psz, ',');

    //Check for valid value AND that there's text remaining.
    if (-1==iMin || 0==*psz)
        return FALSE;

    //Parse off the top of the range.
    iMax=WTranslateUpToChar(&psz, ',');

    //Check for valid value AND that there's text remaining.
    if (-1==iMax || 0==*psz)
        return FALSE;

    //Parse off the position and validate it.
    iCur=WTranslateUpToChar(&psz, ',');

    if (-1==iCur)
        return FALSE;

    //Store the parsed values in the return locations.
    *piMin=iMin;
    *piMax=iMax;
    *piPos=iCur;

    return TRUE;
    }





/*
 * WTranslateUpToChar
 *
 * Purpose:
 *  Scans a string for digits, converting the series of digits to
 *  an integer value as the digits are scanned.  Scanning stops
 *  at chDelimeter or the end of the string.
 *
 *  If the scan sees a non-digit character, -1 is returned to indicate
 *  error.  If the scan sees a null-terminator before any text, we
 *  return 0.
 *
 * Parameters:
 *  ppsz            LPSTR * pointer to pointer to the string to scan.
 *                  On return, the pointer will point to the character
 *                  after the delimeter OR the NULL terminator.
 *
 *                  We want a pointer to the pointer so we can modify
 *                  that pointer for the calling function since we are
 *                  using the return value for the parsed value
 *
 *  chDelimiter     char delimeter at which the scanning stops.
 *
 * Return Value:
 *  UINT            -1 if the string contains non-digits excluding
 *                  the comma.  Otherwise the converted value is
 *                  returned and the pointer to the address after
 *                  the comma is stored at ppsz.
 */

UINT WINAPI WTranslateUpToChar(LPSTR FAR *ppsz, char chDelimeter)
    {
    UINT    wRet=0;
    char    ch;
    LPSTR   psz;

    psz=*ppsz;

    //Fail if there's no more string to parse.
    if (0==*psz)
	return ((UINT)-1);

    //Scan string, stopping at 0 or chDelimeter
    while (ch=*psz)
        {
        if (ch==chDelimeter)
            {
            psz++;  //Skip delimeter
            break;
            }

        //Check for digits, returning -1 on a non-digit.
        ch-='0';

        if (ch < 0 || ch > 9)
	    return ((UINT)-1);

        //Add up the value as we scan.
        wRet=(wRet*10) + (UINT)ch;

        /*
         * We increment psz here so if we hit a null-terminator
         * psz is always valid.  If we incremented in the while
         * statement then psz might be past the null-terminator
         * and possibly invalid.
         */
        psz++;
        }

    /*
     * Store the new pointer and the value.  Note that the *psz++
     * already incremented psz past the delimeter AND the zero.
     */
    *ppsz=psz;
    return wRet;
    }

