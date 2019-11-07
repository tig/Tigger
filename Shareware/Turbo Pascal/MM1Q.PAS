{---------------------------------------------------------------------------}
{ MM1Q.PAS - M/M/1 Queue simulation (For SIE 431 Computer Assignment 2)     }
{                                                                           }
{ Copyright (C) 1989, Charles E. Kindel, Jr., Joseph J. Mattera             }
{ All rights reserved.                                                      }
{                                                                           }
{ Created on September 30, 1989                                             }
{---------------------------------------------------------------------------}
{                                                                           }
{ This program was written in Turbo Pascal 5.5.                             }
{---------------------------------------------------------------------------}

{$N+}   { This compiler directive indicates that 80x87 co-processing is to  }
        { be used.                                                          }
{$E+}   { This directive tells the linker to link in 80x87 emulation        }
        { libraries.  If an 80x87 is not detected on the host machine, the  }
        { emulator will be used.                                            }

Program MM1_Queue;

  const
        OneOverLambda = 30.0 ;
        OneOverMu     = 27.0 ;

  var
        seed : longint ;
        i    : longint ;
        ArriveTime,
        BeginServ,
        EndServ     : array [1..2] of extended ;
        WaitTime : extended ;
        name : string ;
        outfile : text ;
        NumCusts : longint ;

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


  function exp_RV (v : extended) : extended ;
    begin
      exp_RV := - v * ln(U(seed)) ;
    end ;


  function max (x1, x2 : extended) : extended ;
    begin
      if (x1 > x2) then
        max := x1
      else
        max := x2 ;
    end ;

begin
  write ('Enter seed: ') ;
  readln(seed) ;
  write ('Enter number of customers in this run: ') ;
  readln (NumCusts) ;
  write ('Enter output file name: ') ;
  readln (name) ;
  assign (outfile, name) ;
  rewrite (outfile) ;
  ArriveTime[1] := 0 ;
  ArriveTIme[2] := 0 ;
  BeginServ[1] := 0 ;
  BeginServ[2] := 0 ;
  EndServ[1] := 0 ;
  EndServ[2] := 0 ;

  for i := 1 to NumCusts do
    begin
      ArriveTime[2] := ArriveTime[1] + exp_RV(OneOverLambda) ;
      BeginServ[2] := max(ArriveTime[2], EndServ[1]) ;
      EndServ[2] := BeginServ[2] + exp_RV(OneOverMu) ;
      ArriveTime[1] := ArriveTime[2] ;
      BeginServ[1] := BeginServ[2] ;
      EndServ[1] := EndServ[2] ;

      WaitTime := BeginServ[2] - ArriveTime[2] ;
      writeln (outfile, WaitTime:15:10) ;
    end ;

end.