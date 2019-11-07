/************************************************************************
 *
 *      Module:  wstdarg.h
 *
 *     Remarks:  stdarg.h for use with DLLs
 *
 ************************************************************************/

typedef char _far *wva_list ;

#define wva_start( ap, v )  (ap = (wva_list) &v + sizeof( v ))
#define wva_arg( ap, t )    (((t _far *)(ap += sizeof( t )))[-1])
#define wva_end( ap )       (ap = NULL)

/************************************************************************
 * End of File: wstdarg.h
 ************************************************************************/


