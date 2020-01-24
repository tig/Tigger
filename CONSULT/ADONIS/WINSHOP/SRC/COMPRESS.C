/*************************************************************\
 ** FILE:  compress.c
 **
 ** Copyright (c) Adonis Corporation. 1990
 **
 ** DESCRIPTION:
 **  This originated from the WS database editor which got it from
 **  makex which got it from PAKTOOLS.
 **
 ** HISTORY:   
 **   Doug Kent  June 12, 1990 (created)
 **
\*************************************************************/
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <types.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <string.h>
#include <time.h>
#include "windows.h"
#include "stdinc.h"
#include "compress.h"
#include "winundoc.h"
#include "misc.h"

#define BUFFER_SIZE  32768

static int init_buffers(void);
WORD
gfree(void FAR *ptr);
void FAR *
gmalloc(DWORD size);

/* for asm routines? */
extern _stklen = 8000;

void     FAR *variables=NULL;
unsigned  variable_size;
char     FAR *input_buffer=NULL;
char     FAR *output_buffer=NULL;

static int 
compress_file(int input_handle, int output_handle)
 {
  long     total_input;      /* total bytes processed */
  long     total_output;     /* total bytes processed */
  unsigned input_remaining;  /* data remaining in current input block */
  unsigned input_size;       /* data processed in this pass */
  unsigned buf_pos;          /* position in input block */
  unsigned output_size;      /* data output in this pass */
  int      done;             /* boolean flag when entirely done */
  int      end_of_input;     /* boolean flag for last input block */
  int      temp_handle;      /* temporary file to hold first pass */

  if ((temp_handle = _lcreat("tempfile.tmp", 0)) < 0) {
#ifdef INTERNAL
    AuxPrint("could not create temporary file");
#endif
    return FAIL;
   }
  total_output = 0;   /* set counters to 0 */
  total_input = 0;
#ifdef INTERNAL
  AuxPrint("bytes compressed / pass 1: ");
#endif
  init_Distill(variables);      /* initialize compression */
  do {  /* repeat for each block of input */
    /* read block from file */
    input_remaining = _lread(input_handle, input_buffer, BUFFER_SIZE);
    buf_pos = 0;       /* set pointer to beginning of block */
    end_of_input = eof(input_handle);  /* flag if this was last block */
      /* calculate CRC value */
    //crc_value = crc(input_buffer, input_remaining, crc_value);
    do {  /* repeat for each block of output */
      input_size = input_remaining;   /* maximum input = input remaining */
      output_size = BUFFER_SIZE;      /* max output = size of buffer */
      done = Pre_Distill(input_buffer + buf_pos, &input_size,
                         output_buffer, &output_size,
                         variables, end_of_input);   /* compress block */
      total_output += output_size;  /* increment output count */
      total_input += input_size;     /* increment input count */
      buf_pos += input_size;         /* move input pointer */
      input_remaining -= input_size; /* decrement input remaining in block */
#ifdef INTERNAL
      AuxPrint("%7li",total_input);
#endif
      _lwrite(temp_handle, output_buffer, output_size); /* write output */
     } while(input_remaining);
   } while (!done);
#ifdef INTERNAL
  AuxPrint("\n%li bytes output", total_output);
#endif
  total_output = 0;   /* set counters to 0 */
  _llseek(temp_handle, 0L, SEEK_SET);
  total_input = 0;
#ifdef INTERNAL
  AuxPrint("bytes compressed / pass 2: ");
#endif
  do {  /* repeat for each block of input */
    /* read block from file */
    input_remaining = _lread(temp_handle, input_buffer, BUFFER_SIZE);
    buf_pos = 0;       /* set pointer to beginning of block */
    end_of_input = eof(temp_handle);  /* flag if this was last block */
      /* calculate CRC value */
    do {  /* repeat for each block of output */
      input_size = input_remaining;   /* maximum input = input remaining */
      output_size = BUFFER_SIZE;      /* max output = size of buffer */
      done = Distill(input_buffer + buf_pos, &input_size,
                     output_buffer, &output_size,
                     variables, end_of_input);   /* compress block */
      total_output += output_size;  /* increment output count */
      total_input += input_size;     /* increment input count */
      buf_pos += input_size;         /* move input pointer */
      input_remaining -= input_size; /* decrement input remaining in block */
#ifdef INTERNAL
      AuxPrint("%7li",total_input);
#endif
      _lwrite(output_handle, output_buffer, output_size); /* write output */
     } while(input_remaining);
   } while (!done);
#ifdef INTERNAL
  AuxPrint("\n%li bytes output", total_output);
#endif
  _lclose(temp_handle);
  unlink("tempfile.tmp");
  return OK;
 }

static int 
init_buffers(void)
 {
  variable_size = Distill_work_size();   /* allocate area for compression */
#ifdef INTERNAL
  AuxPrint("free memory = %ld",GlobalCompact((DWORD)variable_size));
#endif

  if ((variables = (void FAR *)gmalloc((DWORD)variable_size)) == NULL) 
  {
#ifdef INTERNAL
    AuxPrint("Not enough memory for Crushing variables");
#endif
    return FAIL;
  }
  if ((input_buffer = (char FAR *) gmalloc(BUFFER_SIZE)) == NULL) 
  {
#ifdef INTERNAL
    AuxPrint("Not enough memory for input buffer");
#endif
    return FAIL;
  }
  if ((output_buffer = (char FAR *) gmalloc(BUFFER_SIZE)) == NULL) 
  {
#ifdef INTERNAL
    AuxPrint("Not enough memory for output buffer");
#endif
    return FAIL;
  }

  return OK;
 }

void 
free_buffers(void)
 {
  gfree(variables);
  gfree(input_buffer);
  gfree(output_buffer);
 }


int 
CompressFile(char *input_name,int output_handle)
 {
    int input_handle;    /* handles */
    int nRetval=OK;
    long size;

  if ((nRetval = init_buffers()) != OK)  
  /* allocate buffers and variable space for Crushing */
  {
	ErrMsg(ES_LOWMEMMSG);
    END(FAIL)
  }

     /* --- open input file --- */
  if ((input_handle = _lopen(input_name,READ)) < 0) {
#ifdef INTERNAL
    AuxPrint("could not open %s\n",input_name);
#endif
    END(FAIL)
   }

     /* --- compress file --- */
  nRetval = compress_file(input_handle, output_handle);

  end:
  free_buffers();
  _lclose(input_handle);
  return nRetval;
 }



void FAR *
gmalloc(DWORD size)
{
    HANDLE h;
#ifdef INTERNAL
    AuxPrint("allocating %ld bytes",size);
#endif
    h = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,size);
    if (h == NULL)
        return NULL;
#ifdef INTERNAL
    AuxPrint("locking %lx handle",h);
#endif

    return (void FAR *)GlobalLock(h);
}


WORD
gfree(void FAR *ptr)
{
    WORD seg;
    HANDLE h;

    if (NOT ptr)
        return NULL;

    seg = ((DWORD)ptr & 0xFFFF0000L) >> 16;
    h = LOWORD(GlobalHandle(seg));
    GlobalUnlock(h);
#ifdef INTERNAL
    AuxPrint("freeing %x",h);
#endif
    return (WORD)GlobalFree(h);
}


