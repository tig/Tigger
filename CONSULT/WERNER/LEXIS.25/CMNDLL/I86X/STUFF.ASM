        title   wnsubs.asm

;** wnsubs.asm
;*      
;*  DESCRIPTION:
;*      This file contains a bunch of assembly subroutines that WinNET-II
;*  uses.  They are written to be library routines -- so don't go doin'
;*  special case garbage!!!
;*
;*  HISTORY:
;*      1/13/91     cjp     wrote it
;*
;**


;?PLM = 1
?WIN = 1

.xlist
        include cmacros.inc
.list


sBegin  DATA

sEnd    DATA



sBegin  CODE

        assumes cs,code
        assumes ds,data


;** WORD FAR PASCAL dosSetCurrentDir( LPSTR lpDirName )
;*
;*  DESCRIPTION:
;*      Simply performs a DOS Set Directory (INT 21h AX=3B00h).  If
;*  you are going to use this, you might wanna think about using
;*  INT 21h AX=4700h to get the current directory _before_ calling
;*  this routine.  Then you can RESTORE it when you are done!  Novel
;*  idea, eh?
;*
;*  ARGUMENTS:
;*      LPSTR lpDirName :  Pointer to sz buffer containing directory
;*                         to set the current dir to.
;*
;*  RETURN (WORD):
;*      The return is FALSE if successful.  If it fails, AX is
;*  returned unmunged from the 3Bh call -- will most likely be
;*  03h, path not found.
;*
;*  NOTES:
;*
;**

cProc dosSetCurrentDir, <PUBLIC, FAR, PASCAL>, <ds>
        ParmD   lpDirName
cBegin

        lds     dx, lpDirName   ; ds:dx -> lpDirName

        mov     ah, 3Bh         ; change current directory
        int     21h
        jc      scdExit         ; skip on error
        xor     ax, ax          ; return FALSE if successful

scdExit:

cEnd dosSetCurrentDir


;************************************************************************


;** WORD FAR PASCAL sysGetExtErrorInfo( LPDOSERRORINFO lpErrInfo )
;*
;*  DESCRIPTION:
;*      Calls the DOS Get Extended Error Info (INT 21h AX=5900h).
;*  The return value is simply AX as returned by the ever informative
;*  DOS.
;*
;*  ARGUMENTS:
;*      LPDOSERRORINFO lpErrInfo  :   Pointer to buffer to contain
;*                                    returned registers.  If NULL,
;*                                    only AX is returned.
;*
;*  RETURN (WORD):
;*      The return is 00h if no extended error info can be given -- 
;*  maybe no error occurred, eh?  Otherwise, it is one of the mass
;*  number of error codes defined in your favorite DOS reference.
;*
;*      If you supply a pointer to a DOSERRORINFO structure, then
;*  you will also get, at no extra charge, the value of BH, BL, and
;*  even CX!  And if you order now, I will even throw in another
;*  copy of AX!!
;*
;*  NOTES:
;*
;**

cProc sysGetExtErrorInfo, <PUBLIC, FAR, PASCAL>, <si, di, es, ds>
        ParmD   lpErrInfo
cBegin

        mov     bx, 00h         ; DOS ref. says so...
        mov     ah, 59h         ; get ext. error info
        int     21h             ; fire it off and return AX

        push    ax              ; save AX
        lds     si, lpErrInfo   ; load pointer to LPDOSERRORINFO
        mov     ax, ds          ; test if null
        or      ax, si
        pop     ax              ; restore AX
        jz      geeiExit        ; if null, just return AX

        xor     cl, cl          ; make cx = (short)ch
        xchg    ch, cl
        mov     word ptr [si + 4], cx   ; lpErrInfo->wErrorLocus = CH
;;      mov     byte ptr [si + 3], bh   ; lpErrInfo->wErrorCode = BH
;;      mov     byte ptr [si + 2], bl   ; lpErrInfo->wErrorAction = BL
        mov     word ptr [si + 2], bx   ; lpErrInfo->wErrorClass/Action = BX
        mov     word ptr [si + 0], ax   ; lpErrInfo->wErrorCode = AX

geeiExit:

cEnd sysGetExtErrorInfo


;************************************************************************


;** WORD FAR PASCAL netGetRedirListEntry( WORD index, LPSTR lpLocal, LPSTR lpRemote )
;*
;*  DESCRIPTION:
;*      This function gets the network redirection list entry of
;*  'index.'  This is zero based.  So, the following code would
;*  grab all net names:
;*
;*      i = 0;
;*
;*      /* loop on each network drive... */
;*      while ( (f = netGetRedirListEntry(i++, szLocal, szRemote)) > 0 )
;*      {
;*          /** copy szLocal and szRemote to another buffer **/
;*
;*          /** f == 03h for printer or 04h for disk drive **/
;*      }
;*
;*  ARGUMENTS:
;*      WORD index      :   Index of device to examine; zero based.
;*
;*      LPSTR lpLocal   :   Pointer to 128 byte buffer to receive
;*                          local name (eg n:).
;*
;*      LPSTR lpRemote  :   Pointer to 128 byte buffer to receive
;*                          remote name (eg \\curtisp\public).
;*
;*  RETURN (WORD):
;*      The return value -1 if an error occured.  This is most likely
;*  due to there being no more entries left in the table (index is
;*  greater than the last entry).  On success, the following values
;*  are returned:
;*                      03h :  device is a printer
;*                      04h :  device is a disk drive
;*
;*  NOTES:
;*
;**

cProc netGetRedirListEntry, <PUBLIC, FAR, PASCAL>, <si, di, es, ds>
        ParmW   index
        ParmD   lpLocal
        ParmD   lpRemote
cBegin

        mov     bx, index       ; fill regs with arguments
        lds     si, lpLocal
        les     di, lpRemote
        mov     ax, 5F02h       ; get redirection list entry
        int     21h
        jnc     grleExit        ; carry clear if no error
        mov     bx, -1          ; carry set, signal the error

grleExit:
        mov     ax, bx          ; return BX -- -1 if error

cEnd netGetRedirListEntry


;** WORD FAR PASCAL netSetRedirListEntry( LPSTR lpLocal, LPSTR lpRemote )
;*
;*  DESCRIPTION:
;*      This function sets a network redirection list entry.  It
;*  will automatically determine if lpLocal is a printer or disk
;*  name and connect to lpRemote as such.
;*
;*  ARGUMENTS:
;*      LPSTR lpLocal   :   Pointer to sz local name (eg "N:").
;*
;*      LPSTR lpRemote  :   Pointer to sz remote name followed by
;*                          sz password (eg "\\curtisp\public\0thug").
;*
;*  RETURN (WORD):
;*      The return is NULL if the connection was successful.  It is
;*  the error code (AX) if there was an error.  This can be:
;*
;*              01h :   invalid function; MS-DOS < V3.1??
;*              03h :   path not found
;*              05h :   access denied
;*              08h :   insufficient memory
;*              xxh :   i have no idea
;*
;*  NOTES:
;*
;**

cProc netSetRedirListEntry, <PUBLIC, FAR, PASCAL>, <si, di, es, ds>
        ParmD   lpLocal
        ParmD   lpRemote
cBegin

        lds     si, lpLocal
        les     di, lpRemote

        mov     cx, 00h         ; don't store any special shit

        mov     bl, 04h         ; assume disk drive
        cmp     byte ptr es:[si + 1], ':'  ; is 2nd char a ':'??
        jz      srleDoIt        ; if it's a disk drive

        mov     bl, 03h         ; it's a printer...

srleDoIt:
        mov     ax, 5F03h       ; set redirection list entry
        int     21h
        jc      srleExit        ; carry set if error, return AX

        xor     ax, ax          ; return success

srleExit:

cEnd netSetRedirListEntry


;** WORD FAR PASCAL netCancelRedirListEntry( LPSTR lpDevice )
;*
;*  DESCRIPTION:
;*      This function cancels a network redirection list entry.  All
;*  it needs is the local or remote device name (eg "N:", "LPT1",
;*  "\\curtisp\public", etc.).
;*
;*  ARGUMENTS:
;*      LPSTR lpDevice  :   Pointer to sz device name (eg "N:").
;*
;*  RETURN (WORD):
;*      The return is NULL if the cancellation was successful.  It is
;*  the error code (AX) if there was an error.  As far as I know, this
;*  can be:
;*
;*              01h :   invalid function; MS-DOS < V3.1??
;*
;*  NOTES:
;*
;**

cProc netCancelRedirListEntry, <PUBLIC, FAR, PASCAL>, <si, ds>
        ParmD   lpDevice
cBegin

        lds     si, lpDevice    ; ds:si -> sz device name buffer
        mov     ax, 5F04h       ; cancel redirection list entry
        int     21h
        jc      crleExit        ; carry set if error, return AX

        xor     ax, ax          ; return success

crleExit:

cEnd netCancelRedirListEntry


;** WORD FAR PASCAL netGetMachineName( LPSTR lpBuffer )
;*
;*  DESCRIPTION: 
;*      Performs an INT 21h with AX=5E00h.  Basically, it gets the
;*  machine name -- a 15 bytes ASCIIZ string used to identify the
;*  machine to a network.  If a network is not running, the results
;*  of this function can be 'unpredictable.'
;*
;*  ARGUMENTS:
;*      LPSTR lpBuffer  :   Pointer to buffer to receive the machine
;*                          name.  This MUST be at least 16 chars.
;*
;*  RETURN (WORD):
;*      The return value is the value of CX.  This can be:
;*
;*              CH =    00h, name not defined
;*              CH >    00h, name defined
;*              CL      NetBIOS name number (CH > 00h)
;*
;*  lpBuffer will contain the machine name if CH > 00h.  Also, if
;*  INT 21h AX=5E00h returns 'invalid function,' then the return
;*  will be 0 to show there was an error (as with ANY error, zero
;*  is returned).  Invalid Function should only happen on < DOS 3.1
;*  -- or maybe IBM DOS 4.x!!!  8-)
;*
;*  NOTES:
;*
;** cjp */

cProc netGetMachineName, <PUBLIC, FAR, PASCAL>, <ds>
        ParmD   lpBuffer
cBegin

        lds     dx, lpBuffer    ; ds:dx -> lpBuffer

        mov     ax, 5E00h       ; get machine name
        int     21h
        jnc     gmnExit         ; just return cx on success
        xor     cx, cx          ; return failure

gmnExit:
        mov     ax, cx          ; return CX

cEnd netGetMachineName

sEnd    CODE

end


;** EOF: wnsubs.asm **
