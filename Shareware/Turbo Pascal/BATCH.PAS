{---------------------------------------------------------------------------}
{ BATCH.PAS - Calculates a Confidence Interval on simulation data using     }
{             BATCH MEANS.                                                  }
{                                                                           }
{ Copyright (C) 1989, Charles E. Kindel, Jr.                                }
{ All rights reserved.                                                      }
{                                                                           }
{ Created on September 10, 1989                                             }
{---------------------------------------------------------------------------}
{                                                                           }
{ This program was written in Turbo Pascal 5.5.                             }
{---------------------------------------------------------------------------}

{$N+}   { This compiler directive indicates that 80x87 co-processing is to  }
        { be used.                                                          }
{$E+}   { This directive tells the linker to link in 80x87 emulation        }
        { libraries.  If an 80x87 is not detected on the host machine, the  }
        { emulator will be used.                                            }

program BatchMean ;

  const
       TOTPTS = 6000 ;
       TOTPTSALLOWED = 5000 ;
       T = 2.228 ;

  type
       datatype = array [1..TOTPTS] of extended ;

  var
       tInFile,
       tOutFile   : text ;

       i,                     { length of batch (i = lag*2) }
       iBatch,                { current batch counter }
       iPoint,                { current point counter }
       iTotalPts,             { total number of customers in input file }
       m         : integer ;  { number of batches }

       eData     : datatype ; { array [1..TOTPTS] of extended }
       eBatchMean : array [1..200] of extended ;

       eSum,
       eGrandBatchMean,
       eGrandBatchVariance,
       eConfidenceInterval,
       eGrandBatchSum,
       eGrandBatchSumSq : extended ;

  function iAutoLag (eData : datatype; n : integer) : integer ;
    begin
      iAutoLag := 500 ;
    end ;


begin
  { If there are parameters on the command line then use the first }
  { parameter as the input filename.  Otherwise prompt user for    }
  { file name.                                                     }
  if paramcount < 2 then
    begin
      writeln ('BATCH: usage = batch <inputfile> <outputfile>') ;
      halt ;
    end ;
  writeln ('Reading from ', paramstr(1), '.') ;
  writeln ('Writing to ', paramstr(1), '.') ;

  { Open files }
  assign (tInFile, paramstr(1)) ;
  reset (tInFile) ;
  assign (tOutFile, paramstr(2)) ;
  rewrite (tOutFile) ;

  { Read input file into an array (eData).  Determine the total    }
  { number of customers.                                           }
  iTotalPts := 0 ;
  while not eof(tInFile) do
    begin
      inc (iTotalPts) ;
      read (tInFile, eData[iTotalPts]) ;
    end ;

  { Calculate batch size (i) }
  i := iAutoLag (eData, iTotalPts) ;

  { Calculate total number of batches.  Note that we subtract 1 in }
  { order to throw away 1st batch.                                 }
  iTotalPts := iTotalPts - i ;
  iTotalPts := iTotalPts - (iTotalPts - TOTPTSALLOWED) ;
  m := trunc((iTotalPts / i) - 1) ;

  { Calculate individual batch means.  Note that iBatch (the batch }
  { counter) runs from 1 to the number of batches minus the first  }
  { batch, thus throwing out the first batch.  In calculating the  }
  { index for the current data point [(iBatch * i) + iPoint] we    }
  { ignore the first i data points (or first batch).               }
  eGrandBatchSum   := 0 ;
  eGrandBatchSumSq := 0 ;
  for iBatch := 1 to m do
    begin
      eSum := 0 ;
      for iPoint := 1 to i do
        eSum := eSum + eData[(iBatch * i) + iPoint] ;
      eBatchMean[iBatch] := eSum / i ;

      eGrandBatchSum := eGrandBatchSum + eBatchMean[iBatch] ;
      eGrandBatchSumSq := eGrandBatchSumSq + sqr(eBatchMean[iBatch]) ;
    end ;

  eGrandBatchMean := eGrandBatchSum / m ;

  eGrandBatchVariance := 1 / (m-1) * (eGrandBatchSum - (sqr(eGrandBatchSum) / m)) ;

  eConfidenceInterval := T * sqrt(eGrandBatchVariance)/sqrt(m) ;

  writeln (tOutFile, 'Batch Means results:') ;
  writeln (tOutFile) ;
  writeln (tOutFile, '   Number of data points:', iTotalPts:10) ;
  writeln (tOutFile, '              Batch Size:', i:10) ;
  writeln (tOutFile, '       Number of batches:', m:10) ;
  writeln (tOutFile) ;



  writeln ('Done!') ;


end.