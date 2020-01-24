/*    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */


/** wdbsup.c
 *
 *  DESCRIPTION: 
 *      This file contains misc. 'support' functions for WDB.  These
 *      are not public (exported).
 *
 *  HISTORY:
 *      4/30/91     cjp     touched up a few things
 *      3/10/91     cjp     put in this comment
 *      1/27/92     cek     Put in OutputDebugString support
 *
 ** cjp */


/* grab the includes we need */
    #include <windows.h>
    #include "wdb.h"
    #include "wdbi.h"


/** void FAR PASCAL wdbHourGlass( BOOL fHourGlass )
 *
 *  DESCRIPTION: 
 *      This function changes the cursor to that of the hour glass or
 *      back to the previous cursor.
 *
 *  ARGUMENTS:
 *      BOOL fHourGlass :   TRUE if we need the hour glass.  FALSE if
 *                          we need the arrow back.
 *
 *  RETURN (void):
 *      On return, the cursor will be what was requested.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL wdbHourGlass( BOOL fHourGlass )
{
    static HCURSOR  hCursor;
    static UINT     wWaiting = 0;

    if ( fHourGlass )
    {
        if ( !wWaiting )
        {
            hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
            ShowCursor( TRUE );
        }

        wWaiting++;
    }

    else
    {
        if ( !--wWaiting )
        {
            ShowCursor( FALSE );
            SetCursor( hCursor );
        }
    }
} /* wdbHourGlass() */


/** BOOL FAR PASCAL wdbOpenOutput( LPWDBSESSION lpSession )
 *
 *  DESCRIPTION: 
 *      This function is responsible for opening the output device for
 *      lpSession.  No checks are made for an existing output handle
 *      on lpSession--should we?
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Pointer to WDB session structure
 *                                  containing the output device to open.
 *                                  The handle is then jammed back into
 *                                  this structure.
 *                                  
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if this function successfully opened a
 *      valid output device.  It is FALSE if no output device could be
 *      opened.
 *
 *  NOTES:
 *      As an enhancement to this, a message box should be displayed
 *      asking for a different output device if the default fails.  Right
 *      now, if the open of an output device fails, stdaux is used.  If
 *      that fails, we're totally hosed.  Instead, the stdaux device
 *      should be checked--if it is not available (user hits Cancel on
 *      'can not write to AUX' assert), then disable debug output??
 *
 ** cjp */

BOOL FAR PASCAL wdbOpenOutput( LPWDBSESSION lpSession )
{
    /* determine what the output device is */
    switch ( lpSession->wdbConfig.wOutput )
    {
        case WDB_OUTPUT_FILE:
        {
            LPSTR   lpszFileName = lpSession->wdbConfig.szOutFile;
            BOOL    fAppend;

            /* if '>>' are the first two chars, then it is an append */
            fAppend = (*((UINT FAR *)lpszFileName) == (('>'<<8)|'>'));

            /* skip over all the '>' and white space--if any */
            while ( (*lpszFileName == '>') || isspace(*lpszFileName) )
                lpszFileName++;

            /* is it append file or new file */
            if ( fAppend )
            {
                /* try to append to existing file */
                lpSession->hOutput = (HFILE)_lopen( lpszFileName, WDBC_OF_FLAGS );

                if ( lpSession->hOutput != (HFILE)-1 )
                {
                    _llseek( lpSession->hOutput, 0L, SEEK_END );
                    break;
                }
            }

            /* create a new file */
            lpSession->hOutput = (HFILE)_lcreat( lpszFileName, WDBC_CREAT_FLAGS );

            if ( lpSession->hOutput != (HFILE)-1 )
                break;

#ifdef ORIGINAL
            /* this just isn't our day... default to aux */
            goto auxOutput;
#else
            /* use ODS by default */
            goto odsOutput;
#endif
        }

        case WDB_OUTPUT_WINDOW:
            if ( lpSession->hOutput = wdbOpenWindow( lpSession ) )
                break;

            /** fall through **/

        case WDB_OUTPUT_AUX:
#ifdef ORIGINAL
auxOutput:
#endif
            /* in case we are defaulting to AUX, set device as such */
            lpSession->wdbConfig.wOutput = WDB_OUTPUT_AUX;
            lpSession->hOutput = (HFILE)FH_AUX;
        break;

        case WDB_OUTPUT_ODS:
odsOutput:
            lpSession->hOutput = (HFILE)NULL ;
            lpSession->wdbConfig.wOutput = WDB_OUTPUT_ODS ;
        break ;

        case WDB_OUTPUT_MONO:
            lpSession->hOutput = (HFILE)NULL ;
            wdbMonoOpen( TRUE ) ;
        break ;

    } /* switch() */

    /* return success */
    return ( TRUE );
} /* wdbOpenOutput() */


/** BOOL FAR PASCAL wdbCloseOutput( LPWDBSESSION lpSession )
 *
 *  DESCRIPTION: 
 *      Closes the output device allocated to lpSession.  The handle
 *      contained in lpSession is then invalidated--returning the session
 *      to a non-initialized state.
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Pointer to session to shut down.
 *
 *  RETURN (BOOL):
 *      The return value is TRUE if lpSession was successfully shut down.
 *      It is FALSE if this function fails, which it never does right now.
 *
 *  NOTES:
 *
 ** cjp */

BOOL FAR PASCAL wdbCloseOutput( LPWDBSESSION lpSession )
{
    /* determine what the output device is */
    switch ( lpSession->wdbConfig.wOutput )
    {
        case WDB_OUTPUT_FILE:
            /* close the file if the handle is valid */
            if ( lpSession->hOutput && (lpSession->hOutput != (HFILE)-1) )
                _lclose( lpSession->hOutput );
        break;

        case WDB_OUTPUT_WINDOW:
            /* close window */
            wdbCloseWindow( lpSession );
        break;
        
        case WDB_OUTPUT_MONO:
            wdbMonoClose() ;
        break ;
    }

    /* invalidate handle */
    lpSession->hOutput = NULL;

    /* return success */
    return ( TRUE );
} /* wdbCloseOutput() */


/** LPWDBSESSION wdbValidateSession( HWDB hWDB, UINT wLevel )
 *
 *  DESCRIPTION: 
 *      This call is responsible for 'validating' the output device for
 *      a session.  In this way, the device will be opened 'only when
 *      it is written to.'  It will remain open after that; it just won't
 *      be opened until it is used.
 *
 *      This is used most often for 'WINDOW' output.  The user can, at
 *      any time, close the output window.  If more debug info needs to
 *      be written, then this call will open it again... and so begins
 *      the story of the wSemaphore field.
 *
 *      Take the following example.  If the debug window is open and 
 *      reporting all messages that go through 'DefWindowProc(),' you
 *      are fine.  Now, if the user closes the debug window, yet has 
 *      a debug print statement in *their* window proc, we will get 
 *      called to print some messages caused by killing the debug
 *      window.  Ok, so we need to re-open the window--no big deal, right?
 *
 *      Well almost.  When the debug window is re-created it sends MORE
 *      messages to the users window proc... which causes their debug
 *      print statement to be issued... which tells us to open the window...
 *      Believe me, the results 'aint pretty.
 *
 *      SO!  Enter the wSemaphore.  You won't get ALL DefWindowProc() 
 *      messages, but you also won't crash!  If you need all messages,
 *      just use Spy!
 *
 *  ARGUMENTS:
 *      HWDB hWDB   :   Handle to WDB session to validate output for.
 *
 *      UINT wLevel :   The debugging level to validate for.  This can
 *                      be 0 to WDB_LEVEL_MAX.  Remember that 0 is used
 *                      as a disable...
 *
 *  RETURN (LPWDBSESSION):
 *      The return value is a GlobalLock()'d hWDB session pointer if
 *      this call succeeds.  The *CALLER* is responsible for unlocking
 *      the hWDB when they are finished with it.  The return value
 *      is NULL if the session could not be validated.
 *
 *  NOTES:
 *
 ** cjp */

LPWDBSESSION FAR PASCAL wdbValidateSession( HWDB hWDB, UINT wLevel )
{
    LPWDBSESSION    lpSession;
    LPWDBCONFIG     lpConfig;

    /* if our session pointer is valid */
    if ( lpSession = hWDB ? (LPWDBSESSION)GlobalLock(hWDB) :
                            (LPWDBSESSION)&gWDBDefSession )
    {
        lpConfig = &lpSession->wdbConfig;

        /* if wEnable and wOutput are on for this session */
        if ( lpConfig->wEnable && lpConfig->wOutput && !lpSession->wSemaphore )
        {
            /* check the debugging level */
            if ( lpConfig->wLevel && (wLevel <= lpConfig->wLevel) )
            {
                /* if output stream not ready, prepare it */
                if ( !lpSession->hOutput )
                {
                    /* block! */
                    lpSession->wSemaphore++;
                    {
                        wdbOpenOutput( lpSession );
                    }
                    lpSession->wSemaphore--;
                }

                /* success */
                return ( lpSession );
            }
        }

        /*  It is the *CALLER'S* responsibility to GlobalUnlock() the
         *  session handle--ONLY unlock it if the session is not stable!
         *  And we know that the session is not stable right now.
         */
        if ( hWDB )  GlobalUnlock( hWDB );
    }

    /* return failure for any number of reasons */
    return ( NULL );
} /* wdbValidateSession() */


/** BOOL FAR PASCAL wdbWriteFile( LPWDBSESSION lpSession, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      This function is responsible for outputing lpsz to the current
 *      output 'file.'  'File' includes the AUX port.  Unlike the window
 *      print routine, this one will put a 'prelude' in front of each
 *      new line--unless the user disabled it.
 *
 *      This function also handles output to the debug port using
 *      OutputDebugString.  If wOutput is WDB_OUTPUT_ODS output
 *      will be done using ODS. 
 *
 *      One other note: this function converts LF's to CR/LF's (if it
 *      is enabled) *in place*.  What I mean by this is that this func
 *      *modifies* lpsz and then fixes it again.  This works great unless
 *      the buffer the lpsz points to is READ ONLY--this will cause a
 *      fault.  Should this be changed?  (The code that goes to
 *      ODS doesn't do this).
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   WDB session pointer--output device
 *                                  must have already been validated.
 *
 *      LPSTR lpsz              :   Pointer to asciiz string to write.
 *
 *  RETURN (BOOL FAR PASCAL):
 *      The return value is TRUE if this function succeeds.  It is FALSE
 *      if it does not.
 *
 *  NOTES:
 *      It is assumed that this function does NOT use gachBigTemp.  In this
 *      way, functions can (and do) pass gachBigTemp as the lpsz.  This
 *      also applies to gachTemp.
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteFile( LPWDBSESSION lpSession, LPSTR lpsz )
{
    LPSTR   p, q, r, s;
    char    ach;
    HANDLE  hOutput = lpSession->hOutput;
    BOOL    fODS = (lpSession->wdbConfig.wOutput == WDB_OUTPUT_ODS) ;

    /* someone bein' stupid? or did I mess up in codin' this beast? */
    if ( !fODS && (!hOutput || (hOutput == -1)) )
        return ( FALSE );

    /*
     * Braindead, Mr. Palmer!  Why in the heck are you writing
     * to stdaux() if the user selects "AUX"?!?!
     *
     * New option:  If wdbCofnig->wOutput is ODS use OutputDebugString().
     *
     */
   if (fODS)
   {
      /* write prelude if one is defined */
      if ( *(r = lpSession->wdbConfig.szPrelude) )
         OutputDebugString( r );

      /*  Output to ODS, but convert \n's to \n\r's if enabled;
       *  code below is designed to minimize calls to ODS().
       */
      if ( lpSession->wdbConfig.wLFtoCRLF )
      {
         LPSTR          lpszOut ;
         GLOBALHANDLE   gh ;
         int            n = lstrlen( lpsz ) + 3 ;

         if (!(gh = GlobalAlloc( GHND, n )))
         {
            OutputDebugString( lpsz ) ;
            return TRUE ;
         }

         if (lpszOut = GlobalLock( gh ))
         {
            s = lpszOut ;

            for ( p = q = lpsz; *p != 0; p++ ) 
            {
               /* e.g. lpsz = "hello\nabc", q->'h', p->'\n' */
               if ( *p == '\n' )
               {
                  *s = '\r' ;
                  *(s+1) = '\n' ;
                  *(s+2) = '\0' ;

                  OutputDebugString( lpszOut );
                  s = lpszOut ;

                  /* write prelude if more chars after \n */
                  if ( *(p+1) && *(r = lpSession->wdbConfig.szPrelude) )
                     OutputDebugString( r );
               }
               else
               {
                  *s = *p ;
                  s++ ;
                  *s = '\0' ;
               }
            }

            /* any part of lpsz left to ODS */
            if ( s != lpszOut)
               OutputDebugString( s ) ;

            GlobalUnlock( gh ) ;
         }
         else
         {
            GlobalFree( gh ) ; 
            OutputDebugString( lpsz ) ;
         }
      }
      else
      {
         /* no translations--just write verbatim */
         OutputDebugString( lpsz ) ;
      }

      /* return result */
      return ( TRUE );
   }
   else
   {

      /* someone bein' stupid? or did I mess up in codin' this beast? */
      if ( !hOutput || (hOutput == (HFILE)-1) )
         return ( FALSE );

      /* write prelude if one is defined */
      if ( *(r = lpSession->wdbConfig.szPrelude) )
         _lwrite( hOutput, r, lstrlen( r ) );

      /*  Output to hOutput, but convert \n's to \n\r's if enabled;
      *  code below is designed to minimize calls to _lwrite().
      */
      if ( lpSession->wdbConfig.wLFtoCRLF )
      {
         for ( p = q = lpsz; *p != 0; p++ ) 
         {
               /* e.g. lpsz = "hello\nabc", q->'h', p->'\n' */
               if ( *p == '\n' )
               {
                  /*  hack: temporarily replace next char by \r
                  *  won't work if string is READ-ONLY!!!
                  */
                  ach = *++p;                     /* remember it          */
                  p[ 0 ] = '\n';                  /* replace with \n      */
                  p[ -1 ]= '\r';                  /* replace with \r      */
                  _lwrite( hOutput, q, p - q + 1 );
                  q = p;                          /* for next _lwrite()   */
                  *p-- = ach;                     /* un-hack              */
                  *p = '\n';

                  /* write prelude if more chars after \n */
                  if ( ach && *(r = lpSession->wdbConfig.szPrelude) )
                     _lwrite( hOutput, r, lstrlen( r ) );
               }
         }

         /* any part of lpsz left to _lwrite() */
         if ( p > q )
               _lwrite( hOutput, q, p - q );
      }

      /* no translations--just write verbatim */
      else _lwrite( hOutput, lpsz, lstrlen( lpsz ) );

      #ifndef WIN32
      /* flush the file by closing a *copy* of the file handle */
      _asm 
      {
         mov     ah, 45h         ; MS-DOS Duplicate File Handle
         mov     bx, hOutput     ; handle to dup
      }

      /* should error checking be done here? dup *can* fail... */
      _lclose( DOS3Call() );

      #else

      /* Curtis resorted to DOS trickery.  So I respond with NT coolness */
      FlushFileBuffers( hOutput ) ;

      #endif

      /* return result */
      return ( TRUE );
   }

} /* wdbWriteFile() */

/** BOOL FAR PASCAL wdbWriteMono( LPWDBSESSION lpSession, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      This function is responsible for outputing lpsz to the mono
 *      adapter.
 *
 *      One other note: this function converts LF's to CR/LF's (if it
 *      is enabled) *in place*.  What I mean by this is that this func
 *      *modifies* lpsz and then fixes it again.  This works great unless
 *      the buffer the lpsz points to is READ ONLY--this will cause a
 *      fault.  Should this be changed?  (The code that goes to
 *      ODS doesn't do this).
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   WDB session pointer--output device
 *                                  must have already been validated.
 *
 *      LPSTR lpsz              :   Pointer to asciiz string to write.
 *
 *  RETURN (BOOL FAR PASCAL):
 *      The return value is TRUE if this function succeeds.  It is FALSE
 *      if it does not.
 *
 *  NOTES:
 *      It is assumed that this function does NOT use gachBigTemp.  In this
 *      way, functions can (and do) pass gachBigTemp as the lpsz.  This
 *      also applies to gachTemp.
 *
 ** cjp */

BOOL FAR PASCAL wdbWriteMono( LPWDBSESSION lpSession, LPSTR lpsz )
{
    LPSTR   r ;

   /* write prelude if one is defined */
   if ( *(r = lpSession->wdbConfig.szPrelude) )
      wdbMonoOut( r );

   wdbMonoOut( lpsz ) ;

   return TRUE ;

} /* wdbWriteMono() */



/** int FAR PASCAL wdbAssertLog( LPWDBSESSION lpSession, LPSTR lpszText )
 *
 *  DESCRIPTION: 
 *      This function is used to log an assert to the current output 
 *      device for lpSession.  It is formatted to 'stand out.'
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Session pointer for WDB.
 *
 *      LPSTR lpszText          :   Assert text (as displayed in the message
 *                                  box).
 *
 *  RETURN (int):
 *      The return value specifies the number of characters written.
 *
 *  NOTES:
 *
 ** cjp */

int FAR PASCAL wdbAssertLog( LPWDBSESSION lpSession, LPSTR lpszText )
{
    int     iReturn;
    HWDB    hWDB;

    /* if this is for the default WDB session... */
    if ( lpSession == (LPWDBSESSION)&gWDBDefSession )
        hWDB = NULL;

    /* otherwise, get the handle for lpSession */
    else
      #ifndef WIN32
      if ( !(hWDB = LOWORD( GlobalHandle( HIWORD(lpSession) ) )) )
      #else
      if ( !(hWDB = GlobalHandle( (LPVOID)lpSession )) )
      #endif
        return ( 0 );

    /* log this info to the current WDB output device */
    iReturn = wdbPrint( hWDB, "\n%s\n    [%s]\n%s",
            (LPSTR)gszAssertTitle, (LPSTR)gszAssertInfo, lpszText );

    /* return number of chars output */
    return ( iReturn );
} /* wdbAssertLog() */


/** int wdbWriteOutput( LPWDBSESSION lpSession, LPSTR lpsz )
 *
 *  DESCRIPTION: 
 *      This function outputs lpsz to the current output device selected
 *      into lpSession.
 *
 *  ARGUMENTS:
 *      LPWDBSESSION lpSession  :   Session pointer.
 *
 *      LPSTR lpsz              :   Pointer to null terminated string to 
 *                                  write.
 *
 *  RETURN (int):
 *      The return value is the number of characters printed.
 *
 *  NOTES:
 *
 ** cjp */

int wdbWriteOutput( LPWDBSESSION lpSession, LPSTR lpsz )
{
    /* quick sanity check */
    if ( lpSession && lpsz )
    {
        /* determine what the output device is */
        switch ( lpSession->wdbConfig.wOutput )
        {
            case WDB_OUTPUT_AUX:
            case WDB_OUTPUT_ODS:
            case WDB_OUTPUT_FILE:
                return ( (int)wdbWriteFile( lpSession, lpsz ) );

            case WDB_OUTPUT_WINDOW:
                return ( (int)wdbWriteWindow( lpSession, lpsz ) );

            case WDB_OUTPUT_MONO:
                return ( (int)wdbWriteMono( lpSession, lpsz ) ) ;
        }
    }

    /* return zero chars written */
    return ( 0 );
} /* wdbWriteOutput() */


/** EOF: wdbsup.c **/
