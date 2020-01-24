

SO! Here is a short routine that returns a far pointer to a 'shadow'
of the MSR byte.  DO NOT WRITE TO THIS LOCATION.


#define COMM_MAGIC_MSRSHADOW    35      /* magic cookie #911432     */
#define MSR_CTS                 0x10    /* absolute cts state in MSR*/
#define MSR_RLSD                0x80    /* abs dcd, cd, rlsd, etc.  */

LPBYTE GetCommMSRShadow( short nCid )
{

    /* return far pointer to MSR shadow byte */
    return ( ((LPBYTE)SetCommEventMask( nCid, 0 )) + COMM_MAGIC_MSRSHADOW );

} /* GetCommMSRShadow() */


Once you have this pointer, you can then use it as follows:

    lpMSRShadow = GetCommMSRShadow( nCid );

    bCTS = (*lpMSRShadow) & MSR_CTS;

    if ( bCTS )
        /** CTS is high **/
    else
        /** CTS is low **/


Pretty cool, eh?


If you require any further assistance, please do not hesitate to submit
another service request.  I would be more than happy to help you further.

