{
                 queue.pas - Copyright (C) Charles E. Kindel, 1989
                               KindlCo Consulting

                                 QueueAnalysist
                     An Open/Closed Queuing Network Analyzer


Author:   Charles E. Kindel, Jr.

Started:  April 23, 1989
Version:  1.0


DESCRIPTION
   QueueAnalysist will perform an analysis of open and closed queueing networks
   given the following assumptions:
      1) Interarrival times to the system are independent and identically 
         distributed exponential random varaibles.
      2) Service Times are independent exponential random variables, with a 
         parameter that may vary with the station in the network.
   Systems can have as many as 50 nodes (this number may be increased by modifying 
   the MAXNODES constant if there is enough memory).  

   QueueAnalysist can get its input from either a disk file or from the keyboard.
   Output can either be viewed on the screen or printed to the printer.  The data
   that needs to be supplied by the user includes the following:  The number of nodes
   in the system, the arrival rate to each node from outside the system, the
   service time at each node, and the node transition probabilities.  


USAGE INSTRUCTIONS
   QueueAnalysist can be run by typing QUEUE <filename> at the DOS prompt, where
   <filename> refers to an optional file containing the specifications of a queue.
   The format of this file will be described later.

   Once QueueAnalysist is running, the user will be presented with a prompt asking
   for a filename.  The default filename will be the one included on the command 
   line (as <filename>).  If no file was specified on the command line, this prompt 
   will be bypassed.  If the user wishes to use the default file he just presses
   ENTER at the prompt or the filename can be changed appropriately.  If the user
   wishes to enter the network specifications interactively, then just press ENTER 
   with nothing at the prompt.  

   If interactive input is active, the user will be prompted for the specifications 
   of his system.  The first specification is the number of nodes.  Enter any 
   number between 1 and MAXNODES (50).  After the number of nodes has been
   successfully entered, the user will be prompted for the arrival rates
   from outside the system.  For a closed system these should all be zeros.
   The program will next ask for the service times for each node, followed by the 
   node transition probability matrix.  The probability are input as follows:
   P[i,j] = The probability of going from node i to node j.

   If the user chooses to use a file for input, the file must be in the following 
   format:
      - The order of the data is very important.  The data MUST be ordered as 
        follows:  Network Name, Number of Nodes, Arrival Rates, Service Rates,
        Transition Probabilites.

      - Lines beginning with a semi-colon (";") will be treated as comments, all
        text on such a line will be ignored by the program.

      - The first line of the input file (after any comment lines) must contain
        the name of the network.
        The name of the network can contain any numbers or characters, but must
        not exceed 40 characters in length.
      
      - The number of nodes (n) must be alone (starting in the first column) on 
        a line.  It must be an integer value between 2 and MAXNODES (50).

      - The Arrival rates for each of the n nodes must each be on a separate
        line starting in the first column.  Comments may be interspersed
        between lines.

      - The Service Rates for each of the n nodes follow the same rules as those
        for the Arrival rates.

      - The Transistion Probabilities must be entered with n rows and n columns.
        If P[i,j] = The probability of going from node i to node j, then the
        rows are indexed by i and the columns are indexed by j.  See example below
        for more detail.

   Below is an example of an Open Network Queuing System:
         Open Network: Problem 1, set 9
         ; these are comments
         ; you can put a comment on any line of the file as long
         ; as the first character of the line is a semi-colon (;)
         ; We now specify the number of nodes, n
         ; n must be on a line by itself
         3
         ; Next comes the arrival rates for each of the n nodes
         ; each arrival rate must be on it's own line:
         5
         ; now for node 2.
         10
         ; and node 3
         15
         ; Next comes the service rates for each node.
         ; each service rate must be on a separate line
         10
         50
         100
         ; The next set is the transition probabilities.  These are placed
         ; row by row, in matrix form.  Separate each probability in a row with
         ; one or more spaces DO NOT USE TABS!
         0.0     0.3333    0.3333
         0.0     0.0       1
         0.0     0.5       0.0
         ; This is all the data that can be entered via a file.
         ; End of File!

   Any network that has at least one non-zero Arrival rate will be considered an
   open system.  Any network with no non-zero Arrival rate will be considered a
   Closed System.  

IMPLEMENTATION
   QueueAnalysist was developed using Borland International's Turbo Pascal 5.0 
   and TurboPower Associates' Turbo Professional 4.0. 
   It was written on an ALR 386/220 (a 20MHz 80386 IBM compatible).
   The program was designed to be used with a color monitor, but it will work 
   fine on a monochrome.


REVISIONS
   DATE              VERSION     CHANGE
   April 22, 1989    0.01        Initial prototyping
   April 23, 1989    0.10        Interactive input routines
   April 25, 1989    0.20        File input routines
   April 27, 1989    0.60        Open System calculations
   April 27, 1989    0.70        Initial Closed system routines
   April 27, 1989    0.75        Closed System calculations
   April 29, 1989    0.90        Documentation
   April 30, 1989    0.95        Printer routines
   April 30, 1989    1.00        Final debugging.

}


Program Queue ;

Uses
    TpString,                             { These are TurboPascal 5.0 "Units", they are required }
    TpCrt,                                { to do such things as access the printer, etc...      }
    TpEdit,
    TpWindow,
    printer ;

const
    MAXNODES = 100 ;          { define MAXNODES to be the maximum number of nodes in the net    }
    OPENSYSTEM = TRUE ;      { OPEN/CLOSEDSYTEM are used to keep track of which kind of system }
    CLOSEDSYSTEM = FALSE ;   { we're dealing with.                                             }

type
    openclosetype = boolean ;
    vectortype = array [1..maxnodes] of single ;
    matrixtype = array [1..maxnodes, 1..maxnodes+1] of single;
    { matrixtype is required by the Gaussian elimiation routine }

var
    Escaped : boolean ;                      { If the user presses ESC at any input halt program }
    SysType : boolean ;                      { open or closed system ?                           }
    QuitFlag: boolean ;
    InFile  : text    ;                      { Input file if batch mode is used                  }
    MenuWind : WindowPtr ;

    { The following variables contain the "meat" of the network data                             }
    n       : integer ;                      { number of nodes in network                        }
    r       : vectortype ;                   { Arrival rates for open nets, pi for closed nets   }
    mu      : vectortype ;                   { Service rates at each server                      }
    Lambda  : vectortype ;                   { Used only for Open nets                           }
    P       : matrixtype ;                   { Transition probability matrix                     }
    NetName : string ;                       { Name of network                                   }

    Storage : vectortype ;                   { Used to get around recursive procedures during    }
                                             { closed system calculations                        }



{$I \TP\SOURCE\NUM\NEWGAUSS.PAS}

{NEWGAUSS.PAS is a procedure that solves a system of simulatnous linear}
{equations using Guassian elimiation                                   }
{It was supplied by Sid Yakowitz during SIE 270                        }

{$I \TP\SOURCE\NUM\EPSILON.PAS}
{EPSILON.PAS is a procedure to calculate machine epsilon...this is used}
{by NEWGAUSS to determine if the system of equations is singlular or not}


  function Power(x,n:real): real;
  {Power - this function calculates x to the power of n.                 }
     begin
       Power:= Exp(n*Ln(x));
     end; {of Power}



  Function Str2Real (S : string; var R : real ) : boolean ;
  { This function converts a string to a real value }
      var Code : integer ;
    begin
      val (S, R, Code) ;
      if Code <> 0 then
        Str2Real := FALSE
      else
        Str2Real := TRUE ;
    end ;



  Procedure InputSystem;
  { InputSystem - This procedure does all the user input of data.  If the user puts a filename }
  {               on the command line (i.e. C:\QUEUE <filename>) then this procedure will read }
  {               the program data from <filename>.  The syntax for the data file is defined in}
  {               the program documentation.                                                   }
  {               If the user does NOT put a filename on the command line, then the program    }
  {               will run in "interactive" mode, all input will be taken from the keyboard.   }

    var
       i, j, x : integer ;
       FileName, Comment, SubStr : string ;
       ch : char ;

    begin
      ClrScr ;
      { if there is no file name on the command line then get all data interactively }
      { else get it from the file                                                    }
      FileName := ParamStr(1) ;
      SysType := CLOSEDSYSTEM ;
      ReadString ('Enter input file name, or [ENTER] for interactive input: ', 3,1,
                      12, LightCyan, White, 0,escaped, FileName);
      if Escaped then halt ;
      ClrScr ;
      if FileName = '' then
        begin
          TextColor (LightCyan) ;
          n := 0 ;
          ReadInteger ('Enter Number of nodes in the system: ', 3, 1, 4, LightRed,
                     113, 1, MAXNODES, Escaped, n) ;
          if Escaped then halt ;
          Writeln ;
          for i := 1 to n do                 {Get Arrival Rates from keyboard }
            begin
              r[i] := 0 ;
              Writeln ;
              Write ('Enter arrival rate for node ', i:2, ': ') ;
              ReadReal ('', WhereY+2, WhereX, 15, LightRed,
                         113, 4, 0, $FFFF, Escaped, r[i]) ;
              if r[i] <> 0 then
                SysType := OPENSYSTEM ;
              if Escaped then halt ;
            end ;
          clrscr ;
          Writeln ;
          for i := 1 to n do                 { Get Service rates from the keyboard }
            begin
              mu[i] := 0 ;
              Writeln ;
              Write ('Enter Serice rate for node ', i:2, ': ') ;
              ReadReal ('', WhereY+2, WhereX, 15, LightRed,
                         113, 4, 0, $FFFF, Escaped, mu[i]) ;
              if Escaped then halt ;
            end ;
          ClrScr ;
          Writeln ;
          for i := 1 to n do                 { Get Probabilities.  By rows }
            begin
              for j := 1 to n do
                begin
                  P[i,j] := 0 ;
                  Writeln ;
                  write ('Enter probability, P[',i:0,',',j:0,'] = ') ;
                  ReadReal ('', WhereY+2, WhereX, 15, LightRed,
                             113, 4, 0, $FFFF, Escaped, P[i,j]) ;
                  if Escaped then halt ;
                end ;
              writeln ;
            end ;
        end  { Interactive input }
      else
        begin { file input }
          TextColor (LightRed) ;
          Writeln ('Reading data from ', FileName, '...') ;
          Assign (InFile, FileName) ;
          Reset (InFile) ;
          Readln (Infile, NetName) ;
          Readln (Infile, Comment) ;
          While (Comment[1] = ';') do
            Readln (Infile, Comment) ;
          if Not Str2Int (Comment, n) then
            begin
              TextColor (LightMagenta) ;
              Writeln ;
              Writeln ('ERROR in ', FileName, ': invalid number of nodes.') ;
              writeln ('  Aborting....') ;
              halt ;
            end ;
          Writeln ('System has ', n, ' nodes.') ;
          Writeln ('Arrival rates from outside system: ') ;
          TextColor (LightCyan) ;
          Readln (Infile, Comment) ;
          while Comment[1] = ';' do
            readln (Infile, Comment) ;
          i := 1 ;
          while (i <= n) do
            begin
              If Not Str2Real (Comment, r[i]) then
                begin
                  TextColor (LightMagenta) ;
                  Writeln ;
                  Writeln ('ERROR in ', FileName, ': invalid arrival rate.') ;
                  writeln ('  Aborting....') ;
                  halt ;
                end ;
              Writeln ('r[',i,'] = ', r[i]:6:2) ;
              if r[i] <> 0 then
                SysType := OPENSYSTEM ;
              if i < n then
                begin
                  readln (InFile, Comment) ;
                  while Comment[1] = ';' do
                    readln (Infile, Comment) ;
                end ;
              inc(i) ;
            end ;
          TextColor (LightRed) ;
          Writeln ('Service rates: ') ;
          TextColor (LightCyan) ;
          readln (Infile,Comment) ;
          while Comment[1] = ';' do
            readln (Infile, Comment) ;
          i := 1 ;
          while (i <= n) do
            begin
              If Not Str2Real (Comment, mu[i]) then
                begin
                  TextColor (LightMagenta) ;
                  Writeln ;
                  Writeln ('ERROR in ', FileName, ': invalid service rate.') ;
                  writeln ('  Aborting....') ;
                  halt ;
                end ;
              Writeln ('mu[',i,'] = ', mu[i]:6:2) ;
              if i < n then
                begin
                  readln (InFile, Comment) ;
                  while Comment[1] = ';' do
                    readln (Infile, Comment) ;
                end ;
              inc(i) ;
            end ;
          TextColor (LightRed) ;
          Writeln ('Transition probabilties:') ;
          TextColor (LightCyan) ;
          readln (Infile, Comment) ;
          while Comment[1] = ';' do
            readln (Infile, Comment) ;
          i := 1 ;
          while (i <= n) do
            begin
              x := 1 ;
              Comment := Comment + ' ';
              for j := 1 to n do
                begin
                  Comment := TrimLead (Copy (Comment, x, Length(Comment)+1)) ;
                  SubStr := Trim (copy (Comment, 1, Pos(' ', Comment))) ;
                  x := Pos (' ', Comment) ;
                  If Not Str2Real (SubStr, P[i,j]) then
                  begin
                    TextColor (LightMagenta) ;
                    Writeln ;
                    Writeln ('ERROR in ', FileName, ': invalid Probability.') ;
                    writeln ('  Aborting....') ;
                    halt ;
                  end ;
                  Write (P[i,j]:6:2) ;
                end ;
              if i < n then
                begin
                  readln (InFile, Comment) ;
                  while Comment[1] = ';' do
                    readln (Infile, Comment) ;
                end ;
              Writeln ;
              inc(i) ;
            end ;
          Close (Infile) ;
        end ;
    end ;


  Procedure CalcOpen ;
  { CalcOpen - This procedure calculates the Lambda's for an open system.  It calls Gauss      }
  {            which is a routine that solves simltaneous linear equations using guassian      }
  {            elimiation.  Before it calls gauss, it builds the simulanous equations          }
  {            according to equation (4.3) on Page 328 of Ross.                                }

     var
      i,j :integer ;              { counting variables }
      ERR : boolean ;             { if Guass finds the system to be singular it will set ERR to true }
      A : matrixtype ;            { A contains the system of equations for gauss Ax=b }

    begin
      for i := 1 to n do
        begin
          A[i,n+1] := -r[i] ;                      { Gauss uses A[x, n+1] for the b vector in Ax=b }
          for j := 1 to n do                       { we put -r there                               }
            begin
              if (i=j) then
                A[i,j] := P[i,i] - 1
              else
                A[i,j] := P[j,i] ;
            end ;
        end ;
        Gauss (n, 1, A, Epsilon, ERR) ;
        if ERR then
          begin
            writeln ('Error solving equations') ;
            halt;
          end
        else
          for i := 1 to n do
            Lambda[i] := A[i, n+1] ;             { Gauss puts the solution in A[x,n+1], we put them in Lambda}
    end ;


  Function L : real ;
  {  This function calculates the average number of customers in an open system (L) using }
  {  the equation given on page 329 of Ross                                               }

    var i:integer ;
        sum : real ;
    begin
      sum := 0 ;
      for i := 1 to n do
        sum := sum + (Lambda[i]/(Mu[i]-Lambda[i])) ;
      L := sum ;
    end ;


  Function W : real ;
  {  This function calculates the average amount of time a customer spends in the system  }
  {  (W) using the equation given on page 329 of Ross                                     }

    var i:integer ;
        sum :real ;
    begin
      sum := 0 ;
      for i := 1 to n do
        sum := sum + r[i] ;
      W := L / sum ;
    end ;

  Function ES : real ;
  { Calculates Expected service time for system (time in service }
    var i : integer ;
        sum : real ;
    begin
      sum := 0 ;
      for i := 1 to n do
        sum := sum + mu[i] ;
      ES := 1/sum ;
    end ;

  Function Pnj (n,j : integer) : real ;
  { This function calculates the probability of n customers at server j }
    begin
      Pnj := Power (Lambda[j]/mu[j], n) * (1 - Lambda[j]/mu[j]) ;

    end ;

  Procedure OutputOpen ;
  { This procedure simply outputs the calculated open system to the screen }
    var i : integer ;
    begin
      ClrScr ;
      for i := 1 to n do
        writeln ('Lambda[',i:0,'] = ', Lambda[i]:5:2) ;
      Writeln;
      writeln ('L = ', L:6:2) ;
      Writeln;
      writeln ('W = ', W:6:2) ;
    end ;



  Procedure CalcClosed ;
  { This procedure calculates the pi values for a Closed system using equations (4.5) on page 331 of Ross }
  { Like CalcOpen it uses Gauss to solve a system of simultaneous linear equations.                       }

  var i, j : integer ;
        A : matrixtype ;
        ERR : boolean ;

    begin
      for i := 1 to n do
        begin
          A[i,n+1] := 0 ;
          for j := 1 to n do
            begin
              if (i=j) then
                A[i,j] := P[i,i] - 1
              else
                A[i,j] := P[j,i] ;
            end ;
        end ;
        For i := 1 to n+1 do                { Replace 1st equation with sum of pi's = 1}
          A[1,i] := 1 ;
        Gauss (n, 1, A, Epsilon, ERR) ;
        if ERR then
          begin
            writeln ('Error solving equations') ;
            halt;
          end
        else
          for i := 1 to n do
            r[i] := A[i, n+1] ;      { r[i] holds the pi values }
    end ;

  Function Wj (j, m : integer) : real ;
  { This function simply returns the value of Wj(m) given j and m.  The array Storage contains Wj }
  { and Lambda(m) values so that (m-1) values do not need to be recalculated each time.           }

    begin
      Storage [m,j] := 1/mu[j]*(1+Storage[m-1,n+1]*r[j]*Storage[m-1,j]) ;
      Wj := Storage [m,j] ;
    end ;

  Function LambdaRecursive (m : integer) : real ;
  { This Function is not really recursive.  It was in a previous version, but I was able to get }
  { around recursion (and it's inherent slowwwwwness) by utilizing the Storage array to store   }
  { previously calculated Wj's and Lambdas.  This function calculates the Lambdas for a closed  }
  { system using the recursive procedure given on pages 331-335 of Ross.                        }
     var sum : real ;
         i   : integer ;
    begin
      sum := 0 ;
      for i := 1 to n do
        sum := sum + r[i]*Wj(i,m) ;
      Storage [m, n+1] := m / sum ;
      LambdaRecursive := Storage[m,n+1];
    end ;

  Procedure CalcLambda (m : integer) ;
  { This Procedure drives the function LambdaRecursive to calculate the Lambdas and Wjs for a }
  { closed system.  m is the number of recursive iterations.                                  }
    var i,j : integer ;
        sum : real ;
    begin
      sum := 0 ;
      for i := 1 to n do
        begin
          for j := 1 to n+1 do
            Storage[i,j] := -1 ;
          Storage[1,i] := 1/mu[i] ;
          sum := sum + r[i]*Storage[1,i] ;
        end ;
      Storage [1,n+1] := 1 / sum ;
      for i := 2 to m+1 do
        Storage[i,n+1] := LambdaRecursive (i) ;
    end ;





  Procedure OutputClosed ;
  { This procedure outputs the calculated closed system to the screen }
    var i,j : integer ;
        m : integer ;
        Lamd,Sum : real ;
        ch   : char ;
    begin
      ClrScr ;
      for i := 1 to n do
        writeln ('pi[',i:0,'] = ', r[i]:5:2) ;
      writeln ;
      writeln ;
      m := 2 ;
      ReadInteger ('Enter desired m value: ',
                 WhereY+1, WhereX, 4, LightRed, White, 2, 5000, Escaped, m) ;
      if escaped then halt ;
      CalcLambda (m) ;
      write (' m      Lambda(m-1)') ;
      for i := 1 to n do
        write ('     W',i:2,'(m) ') ;
      writeln ;
      for i := 2 to m do
        begin
          write (i:3, Storage[i-1,n+1]:15:4);
          for j := 1 to n do
            write (Storage[i,j]:12:4) ;
          writeln ;
        end ;
      writeln ;
      writeln ('The system throughput rate when there are ',m:0,' parts ') ;
      writeln ('in the system is = ', Storage[m,n+1]:10:4) ;
    end ;

  Procedure PrintOpen ;
  { This procedure prints an Open System and all its calculated values to the printer }
    var i, j : integer ;
        sum  : real ;
    begin
      Writeln (lst, #27,'N',#12) ;
      writeln (lst) ;
      writeln (lst, Center('QueueAnalysist - KindlCo Consulting - (C) Charles E. Kindel, 1989', 75)) ;
      Writeln (lst, Center('Open Queuing System - '+NetName,75)) ;
      writeln (lst, ^O) ;
      Writeln (lst) ;
      writeln (lst, 'System specifications...') ;
      writeln (lst) ;
      writeln (lst, '    Arrival Rates    Service Times') ;
      Writeln (lst, '       r''s               mu''s') ;
      Writeln (lst, '    ------------------------------') ;
      for i := 1 to n do
        writeln (lst, r[i]:15:5, mu[i]:15:5) ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, '    Transistion probabilities (i.e. Probability of going from node i to j') ;
      writeln (lst, '     ', ' ':Round((n/2)*10),'j', ' ':Round((n/2)*10), ' Exit') ;
      for i := 1 to n do
        begin
          if (Round(n/2) = i) then
            write (lst, '  i  ' )
          else
            write (lst, '     ') ;
          sum := 0 ;
          for j := 1 to n do
            begin
              write (lst, P[i,j]:10:5) ;
              sum := sum + P[i,j] ;
            end ;
          writeln (lst, 1 - sum:10:5) ;
        end ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, 'System calculations...') ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, '    Arrival rates into each node (i.e. Lambda[1] = total arrival rate into node 1)...') ;
      for i := 1 to n do
        begin
          writeln (lst, '          Lambda[',i:0,'] = ', Lambda[i]:5:2) ;
          if (Lambda[i]/mu[i] >= 1) then
            begin
            writeln (lst, '          The system is unstable because Lambda[',i:0,']/mu[',i:0,'] is') ;
            writeln (lst, '             not less than 1 ') ;
            end ;
        end ;
      Writeln (lst) ;
      writeln (lst, '    Average Number of customers in the system (L) = ', L:6:2) ;
      writeln (lst, '    Average time a customer spends in the system (W) = ', W:6:2) ;
      writeln (lst, '    Average time a customer spends waiting (non-service) = ', W-ES:6:2) ;
      writeln (lst) ;
      writeln (lst, '    Probability that there are at least n customers at server j: ') ;
      writeln (lst) ;
      write (lst, '     n') ;
      for i := 1 to n do
        write (lst, i:10) ;
      writeln (lst, '   In system') ;
      for i := 1 to 10 do
        begin
          write (lst, i:7, '   ') ;
          sum := 0 ;
          for j := 1 to n do
            begin
              write (lst, Pnj(i,j):10:5) ;
              sum := sum + Pnj (i,j) ;
            end ;
          writeln (lst, 1 - sum:10:5) ;
        end ;
      writeln (lst, ^R, ^L, #27, 'O') ;
    end ;

  Procedure PrintClosed ;
  { This procedure prints a Closed System and all its calculated values to the printer }
    var m, i, j : integer ;
        sum : real ;
    begin
      m := 2 ;
      ClrScr ;
      Writeln ;
      Writeln ;
      ReadInteger ('Enter desired number of parts (m): ',
                 WhereY+1, WhereX, 4, LightRed, White, 2, 5000, Escaped, m) ;
      if escaped then halt ;
      CalcLambda (m) ;
      Writeln (lst, #27,'N',#12) ;
      writeln (lst) ;
      writeln (lst, Center('QueueAnalysist - KindlCo Consulting - (C) Charles E. Kindel, 1989', 75)) ;
      Writeln (lst, Center('Closed Queuing System - '+NetName,75)) ;
      writeln (lst) ;
      Writeln (lst, ^O) ;
      writeln (lst, 'System specifications...') ;
      writeln (lst) ;
      writeln (lst, '    Service Times') ;
      Writeln (lst, '         mu''s') ;
      Writeln (lst, '    ---------------') ;
      for i := 1 to n do
        writeln (lst, mu[i]:15:5) ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, '    Transistion probabilities (i.e. Probability of going from node i to j') ;
      writeln (lst, '     ', ' ':Round((n/2)*10),'j', ' ':Round((n/2)*10), ' Exit') ;
      for i := 1 to n do
        begin
          if (Round(n/2) = i) then
            write (lst, '  i  ' )
          else
            write (lst, '     ') ;
          sum := 0 ;
          for j := 1 to n do
            begin
              write (lst, P[i,j]:10:5) ;
              sum := sum + P[i,j] ;
            end ;
          writeln (lst, 1 - sum:10:5) ;
        end ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, 'System calculations...') ;
      writeln (lst) ;
      writeln (lst) ;
      writeln (lst, '    Stationary probabilities (pi''s)') ;
      for i := 1 to n do
        writeln (lst, '        pi[',i:0,'] = ', r[i]:5:2) ;
      writeln (lst) ;
      writeln (lst, '    Average Service Rates (Lambda), Number of Customers (L), and') ;
      writeln (lst, '    Waiting Time (W).  (m = ', m:0, ' iterations.)') ;
      write (lst, '  m  Lambda(m-1)') ;
      for i := 1 to n do
        write (lst, '  W',i:2,'(m) ') ;
      writeln (lst) ;
      for i := 2 to m do
        begin
          write (lst, i:3, Storage[i-1,n+1]:12:4);
          for j := 1 to n do
            write (lst, Storage[i,j]:9:4) ;
          writeln (lst) ;
        end ;
      writeln (lst) ;
      writeln (lst, '     The system throughput rate when there are ',m:0,' parts in the system is = ', Storage[m,n+1]:8:4) ;
      writeln (lst, ^R, ^L, #27, 'O') ;
    end ;



  Function Menu : char ;
  { This function puts a menu inside of a window on the screen, giving the user his options }
    var ch : char ;
    begin
      if not MakeWindow (MenuWind, 45, 3, 75,9, True, True, False,
                         White, Yellow, LightRed, ' Menu ') then
        begin
          Writeln ('ERROR: MakeWindow') ;
          halt ;
        end
      else
        if not DisplayWindow (MenuWind) then
          begin
            writeln ('ERROR: DisplayWindow') ;
            halt ;
          end
        else
          begin
            Window (50, 4, 75, 9) ;
            TextColor (LightMagenta) ;
            Write ('P'); TextColor (LightCyan) ;
            Writeln ('rint report') ;
            TextColor (LightMagenta) ;
            Write ('V');
            TextColor (LightCyan) ;
            Writeln ('iew results') ;
            TextColor (LightMagenta) ;
            Write ('I') ;
            TextColor (LightCyan) ;
            writeln ('nput new network');
            TextColor (lightMagenta) ;
            Write ('Q') ; TextColor (LightCyan) ;
            Writeln ('uit program.') ;
            Write ('Select [P, V, I, or Q]') ;
            HiddenCursor ;
            repeat
              ch := upcase (readkey) ;
            until ch in ['P','V','I','Q',chr(27)] ;
            Menu := ch ;
            MenuWind := EraseTopWindow ;
            DisposeWindow (MenuWind) ;
            NormalCursor ;
          end ;
    end ;



begin
  TextColor (LightGreen) ;
  TextBackground (Black) ;
  ClrScr ;
  writeln ('QueueAnalysist - Copyright (c) Charles Kindel, KindlCo Consulting, 1989') ;
  gotoxy (1,25) ;
  TextColor (Yellow) ;
  write ('Press [ESC] at any time to quit program') ;
  Window (1,3,80,23) ;

  CursorToEnd := False ;
  InputSystem ;
  Window (1,1,80,25) ;
  GotoXY (1, 2) ;
  clreol ;
  TextColor (White) ;
  Write ('Network Name: ', NetName) ;
  Window (1,3,80,23) ;
  QuitFlag := FALSE ;
  Repeat
    case Menu of
      'P' : if SysType = OPENSYSTEM then
              begin
                CalcOpen ;
                PrintOpen
              end
            else
              begin
                CalcClosed ;
                PrintClosed ;
              end ;
      'V' : if SysType = OPENSYSTEM then
              begin
                ClrScr ;
                CalcOpen ;
                OutputOpen ;
              end
            else
              begin
                ClrScr ;
                CalcClosed ;
                OutputClosed ;
              end ;
      'I' : begin
              InputSystem ;
              Window (1,1,80,25) ;
              GotoXY (1, 2) ;
              TextColor (White) ;
              clreol ;
              Write ('Network Name: ', NetName) ;
              Window (1,3,80,23) ;
            end;

      'Q' : QuitFlag := TRUE ;
      chr(27) : QuitFlag := TRUE ;
      else
        write (^G) ;
    end ;
  Until (QuitFlag) ;
end .
