{---------------------------------------------------------------------------}
{ MONTE.PAS - Monte Carlo Simulation (For SIE 431 Computer Assignment 1)    }
{                                                                           }
{ Copyright (C) 1989, Charles E. Kindel, Jr.                                }
{ All rights reserved.                                                      }
{                                                                           }
{ Created on September 10, 1989                                             }
{---------------------------------------------------------------------------}
{ This program performs a Monte Carlo simulation on a statistical           }
{ experiment.  The expermiment is to roll a 6-sided die 100 times, record   }
{ when the last 1 was rolled, then roll the die until another 1 comes up.   }
{ Find the run-length between these rolls.                                  }
{ The experiment is repeated 1000 times and a 95% confidence interval on the}
{ mean run-length is computed.                                              }
{                                                                           }
{ This program was written in Turbo Pascal 5.5.                             }
{---------------------------------------------------------------------------}

{$N+}   { This compiler directive indicates that 80x87 co-processing is to  }
        { be used.                                                          }
{$E+}   { This directive tells the linker to link in 80x87 emulation        }
        { libraries.  If an 80x87 is not detected on the host machine, the  }
        { emulator will be used.                                            }

Program MonteCarlo ;

  const NUMREPS    = 1000 ;            {Number of repetitions}
        NUMROLLS   = 100 ;             {Number of rolls per rep}
        SIDESOFDIE = 6 ;               {Number of sides to the die}
        T          = 1.96 ;            {T for confidence interval}

  var
        n, n1,
        x,
        Sum,
        SumSq : longint ;

        seed : longint ;

        Average,
        Variance,
        ConfidenceInterval : extended;

  {-------------------------------------------------------------------------}
  { function U (var seed : longint) : extended ;                            }
  {                                                                         }
  { Multiplicative Linear Congruential Generator -                          }
  {   Given an initial seed this function will generate a random number     }
  {   between 0 and 1.                                                      }
  {                                                                         }
  { Adapted from the FORTRAN function RAND on page 227 of "Simulation       }
  { Modeling and Analysis" by Law and Kelton (McGraw Hill, 1982)            }
  {-------------------------------------------------------------------------}
  function U (var seed : longint) : extended ;
    var a, p, b15, b16, xhi, xalo, leftlo, fhi, k : longint;

    begin
      a := 16807 ;
      b15 := 32768 ;
      b16 := 65536 ;
      p := 2147483647 ;
      xhi := trunc (seed / b16) ;
      xalo := (seed-xhi*b16)*a ;
      leftlo := trunc (xalo / b16) ;
      fhi := xhi*a+leftlo ;
      k := trunc (fhi / b15) ;
      seed := (((xalo-leftlo*b16)-p)+(fhi-k*b15)*b16)+k ;
      if (seed < 0) then
        seed := seed + p ;
      U := seed*4.656612875E-10 ;
    end ;

  {-------------------------------------------------------------------------}
  { function experiment : longint ;                                         }
  {                                                                         }
  { Simulates the experiment described in the main program header.          }
  {   Returns the run-length from one replication of the experiment.        }
  {-------------------------------------------------------------------------}
  function experiment : longint ;

    var  RollNum,
         Outcome,
         Last1 : longint ;

    begin
      Last1 := 0 ;
      for RollNum := 1 to NUMROLLS do             {Roll die NUMROLLS times}
        begin                                     { remembering last 1}
          Outcome := trunc(SIDESOFDIE*U(Seed))+1 ;
          if Outcome = 1 then
            Last1 := RollNum
        end ;
      RollNum := NUMROLLS + 1 ;
      Outcome := trunc(SIDESOFDIE*U(Seed))+1 ;    {Continue rolling die until}
      while Outcome > 1 do                        { we get another 1 }
        begin
          outcome := trunc(SIDESOFDIE*U(Seed))+1 ;
          inc(RollNum) ;
        end ;
      experiment := RollNum - Last1 ;
    end ;

begin {Main}
  Sum := 0 ;
  SumSq := 0 ;
  Seed := 498449 ;   { my matric number }
  { Note that there are two for loops here.  This allows the breaking up }
  { of the output into 100 lines of 10 replications.                     }
  for n := 1 to (NUMREPS div 10) do
    begin
      write (n:4,': ') ;
      for n1 := 1 to 10 do
        begin
          x := experiment ;
          write (x:6) ;
          Sum := Sum + x ;
          SumSq := SumSq + (x*x) ;
        end ;
      writeln ;
    end ;
  Average := Sum / NUMREPS ;
  Variance := 1 / (NUMREPS - 1) * (SumSq - (Sum*Sum/NUMREPS)) ;
  ConfidenceInterval := T*sqrt(Variance)/sqrt(NUMREPS) ;
  writeln ;
  writeln ('A 95% confidence interval on the mean run length is ',
           Average:8:4, ' ñ', ConfidenceInterval:8:4,'.') ;
  writeln ('The average is ', average:8:4) ;
  writeln ('The variance was ', Variance:8:4) ;
  writeln ('Done.') ;
end.

