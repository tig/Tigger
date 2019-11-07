{----------------------------------------------------------------------------}
{                                                                            }
{                      Simultaneous Equation Solver                          }
{                                  by                                        }
{                          Charles E. Kindel, Jr.                            }
{                                 for                                        }
{                         KindelCo Software Systems                          }
{                                                                            }
{                Special thanks to Mike Brehm, Mark Jenkins                  }
{                  Karl Friedrich Gauss and Sid Yakowitz                     }
{                                                                            }
{ VERSION 2.0                                                                }
{    7/13/1988                                                               }
{----------------------------------------------------------------------------}
Program Matrix;

  uses
       TpString,
       TpCrt,
       TpEdit,
       TpErrHan,
       printer;

  const  Row = 100;
         Col = 101;
         Date = 1988;
         Year = 1988;
         Version = '2.0';
         MaxRow = 25;
         MaxCol = 80;

  type   matrixtype = array [1..Row, 1..Col] of real;
         namestring = string [12];

  var    NUM_EQS,       {Number of equations and unknowns}
         NUM_SYS,       {Number of systems (right hand side vectors)}
         RES,           {NUM_EQS + NUM_SYS}
         i
         : integer;

         A
         : matrixtype;

         EPS
         : real;         {machine epsilon}

         ERR,
         DONE,
         Line43,
         Escaped
         : boolean;

         CHARACTER
         : char;

         FIL
         : text;

{$I Frame.pro}
{$I \tp\source\Kindelco.pro}
{$I \tp\source\Num\Epsilon.pas}
{$I \tp\source\Num\NEWGAUSS.PAS}

  Procedure Title;
    begin
      ClrScr;
      TextColor (MapColor (Yellow));
      writeln (Center (Concat (chr (240), chr (240),
               ' Simultaneous Equation Solver - V', VERSION, ' ', chr (240), chr (240)),currentwidth-1));
      writeln (Center ('by Charles E. Kindel',currentwidth-1));
      writeln;
      writeln (Center ('Gaussian elimination by Sid Yakowitz',currentwidth-1));
      writeln (Center ('Many thanks to Mark Jenkins and Mike Brehm for inspiration',currentwidth-1));
      writeln;
      TextColor (MapColor(LightRed));
      KindelCo;
      writeln;
      TextColor (MapColor(LightBlue));
      writeln (Center ('This program is FreeWare',currentwidth-1));
      writeln (Center ('If you find it useful please send a donation ($5) to',currentwidth-1));
      writeln;
      writeln (Center ('Charles E. Kindel, KindelCo Software Systems',currentwidth-1));
      writeln (Center ('1509 E. 2nd St.',currentwidth-1));
      writeln (Center ('Tucson, AZ  85719',currentwidth-1));
    end;

  Procedure Clear_Matrix (var Matrix    : matrixtype);

    var x, y : integer;

    begin
      for x := 1 to row do
        for y := 1 to col do
          Matrix[x,y] := 0;
    end;

  Procedure Draw_Brackets (SIZE : integer);

    var i,j : integer;
        S : string[20];

    begin

      Window (1,4, 132{SIZE*8+21}, 4+SIZE*2);
      ClrScr;

      for i := 1 to NUM_EQS do
        begin
          for j := 1 to NUM_EQS do
            begin
              str (A[i,j] :6:2, S);
              fastwrite (S, i*2+3, j*8-5, LightGray);
            end;
          str (A[i,RES] :6:2, S);
          fastwrite (S, i*2+3, RES*8+4, LightGray);
        end;

      fastwrite (chr(218), 4,1, Red);
      for i := 1 to (SIZE*2 - 1) do
        fastwrite (chr(179), 4+i, 1, Red);
      fastwrite (chr(192), 4+size*2,1, Red);

      fastwrite (chr(191), 4, 8*SIZE+2, Red);
      for i := 1 to (SIZE*2 - 1) do
        fastwrite (chr(179), 4+i, 8*SIZE+2, Red);
      fastwrite (chr(217), 4+size*2, 8*SIZE+2, Red);

      fastwrite (chr(218), 4, 8*SIZE+4, Red);
      for i := 1 to (SIZE*2 - 1) do
        fastwrite (chr(179), 4+i, 8*SIZE+4, Red);
      fastwrite (chr(192), 4+size*2, 8*SIZE+4, Red);

      for i := 1 to SIZE do
        fastwrite ('X'+Long2str (i), 3+2*i, SIZE*8+6, MapColor(LightBlue));

      fastwrite (chr(191), 4, SIZE*8+9, Red);
      for i := 1 to (SIZE*2 - 1) do
        fastwrite (chr(179), 4+i, SIZE*8+9, Red);
      fastwrite (chr(217), 4+SIZE*2, SIZE*8+9, Red);

      fastwrite ('=', 4+SIZE, SIZE*8+10, Red);

      fastwrite (chr(218), 4, SIZE*8+11, Red);
      for i := 1 to (SIZE*2 - 1) do
        fastwrite (chr (179), 4+i, SIZE*8+11, Red);
      fastwrite (chr (192), 4+SIZE*2, SIZE*8+11, Red);

      fastwrite (chr (191), 4, SIZE*8+19, Red);
      for i := 1 to SIZE*2 -1 do
        fastwrite (chr(179), 4+i, SIZE*8+19, Red);
      fastwrite (chr(217), 4+SIZE*2, SIZE*8+19, Red);

      Window (1,1,132,25);

    end;


  Procedure Get_Matrix (var A       : matrixtype;
                            NUM_EQS : integer;
                            RES     : integer);

    var   i, j : integer;
          S : string[20];

      Procedure Goto_Field (x, y, RES : integer);

           var k : integer;
               escaped : boolean;
               BufPtr : pointer;

        begin
          CursorToEnd := false;
          if y = RES then
            begin
              if not SaveWindow (y*8+3, x*2+3, y*8+13, x*2+3, True, BufPtr) then
                halt;
              ReadReal ('', x*2+3, y*8+3, 10, Black, MapColor(113), 2, 0, 0, Escaped, A[x,y]);
              RestoreWindow (y*8+3, x*2+3, y*8+13, x*2+3, True, BufPtr);
              Str (A[x,y] :6:2, S);
              fastwrite (S, x*2+3, y*8+4, White);
            end
          else
            begin
              if not SaveWindow (y*8-5, x*2+3, y*8+5, x*2+3, True, BufPtr) then
                halt;
              ReadReal ('', x*2+3, y*8-5, 10, Black, MapColor(113), 2, 0, 0, Escaped, A[x,y]);
              RestoreWindow (y*8-5, x*2+3, y*8+5, x*2+3, True, BufPtr);
              str (A[x,y] :6:2, S);
              fastwrite (S, x*2+3, y*8-5, White);
            end;
          if Escaped then
            begin
              if Line43 then
                SelectFont8x8 (true);
              Title;
              halt;
      end;
    end;  {Goto_Field}


    begin
      Draw_Brackets (NUM_EQS);
      fastwrite ('Enter values or [ESC] to quit, [ENTER] keeps previous value',
                  5+NUM_EQS*2, 1, LightMagenta);
      for i := 1 to NUM_EQS do
        begin
          for j := 1 to NUM_EQS do
            begin
              Goto_Field (i,j, RES);
            end;
          Goto_Field (i, RES,RES);
        end;
    end;

  Procedure Get_Num_Eqs (var NUM  : integer;
                         var FLAG : boolean);

    begin
      FLAG := false;
      repeat
        Gotoxy (1,22);
        NUM := 0;
        CursorToEnd := false;
        ReadInteger ('Enter number of Eqations/Unknowns [ESC] to quit program: ',
                      WhereY, WhereX, 3, MapColor (LightGreen), MapColor(White), 0, Row, Escaped, NUM);
        If NUM = 0 then FLAG := true;
        Gotoxy (1,22);
        ClrEol;
      until ((NUM_EQS > 1) and (NUM_EQS < 14)) or (DONE);
    end;


  Procedure Batch_Read (var Matrix   : matrixtype;
                        var EQS      : integer;
                        var SYS      : integer;
                            FILENAME : namestring);
     var i, j : integer;

     begin
       writeln ('Reading ', ParamStr(1), '...');
       SYS := 1;
       readln (fil, EQS);
       for i := 1 to EQS do
         for j := 1 to EQS+SYS do
           read (fil, Matrix[i,j]);
     end;

  Procedure Print_Matrix (NUM_EQS, NUM_SYS : integer;
                                    var  A : matrixtype;
                                       EPS : real;
                                       ERR : boolean);
    var i,j : integer;
        B : matrixtype;

    begin
      for i := 1 to NUM_EQS do
        for j := 1 to NUM_EQS+1 do
          B[i,j] := A[i,j];
      TextColor (MapColor(LightBlue));
      Writeln ('Computing matrix...');
      Gauss (NUM_EQS, NUM_SYS, B, EPS, ERR);
      if ERR then
        writeln (chr (7),'ERROR...Matrix is singular or otherwise invalid')
      else
        begin
          writeln;
          ClrEol;
          TextColor (MapColor(LightMagenta));
          Write ('Do you want output to go to printer? [Y/n]: ');
          character := upcase(readkey);
          clrscr;
          TextColor (MapColor(LightCyan));
          for i := 1 to num_Eqs do
            writeln ('X', i, ' = ', B[i, NUM_EQS+NUM_SYS] :8:6, '    ');
          If character in ['Y'] then
            begin
              for i := 1 to num_eqs do
                writeln (lst, 'X', i, ' = ', B[i, NUM_EQS+NUM_SYS] :8:6, '    ');
              writeln (lst, chr(12));
            end;
          Writeln;
        end;
    end; {print}

  {$F+}
  function RecoverError : boolean;
  { invoked when a runtime error occurs }
  var i : word;

    begin
      I := IoResult;
      Assign (Output, '');
      Rewrite (output);
      if ExitCode = 200 then
        RecoverError := True
      else
        begin
          if Line43 then
            SelectFont8x8 (true);
          ClrScr;
          Writeln (chr(7), 'Runtime Error!  Please write down the following line');
          writeln ('and submit it to the author of this program:');
          writeln;
          RecoverError := False;
        end;
    end;
    {$F-}


begin
  window (1,1,currentWidth,CurrentHeight+1);
  ErrorRecovery (True, @RecoverError);
  if CurrentDisplay >= EGA then
    if Font8x8Selected then
      begin
        Line43 := True;
        SelectFont8x8 (false);
      end
    else
      Line43 := false;
  DirectVideo := true;

  Done := false;
  EPS := EPSILON;
  if paramcount > 0 then
    begin
      assign (fil, ParamStr(1));
      reset (fil);
      Clear_Matrix (A);
      Batch_Read (A, NUM_EQS, NUM_SYS, ParamStr(1));
      Print_Matrix (NUM_EQS, NUM_SYS, A, EPS, ERR);
    end
  else
    begin
      Title;
      gotoxy (64, 25); write ('Press a key...');
      character := readkey;
      ClrScr;
      TextColor (mapColor(LightCyan));
      Writeln (Center ('Simultaneous Equation Solver - KindelCo Software Systems, (C) '
               +long2str (YEAR),currentwidth-1));
      writeln (Center ('Version '+version,currentwidth-1));
      Clear_Matrix (A);
      Get_Num_Eqs (NUM_EQS, DONE);
      while not DONE do
        begin
          Gotoxy (1,3);
          for i := 1 to 21 do
            begin
              ClrEol;
              writeln;
            end;
          NUM_SYS := 1;
          RES := NUM_EQS + NUM_SYS;
          Get_Matrix (A, NUM_EQS, RES);
          Gotoxy (1, 6+NUM_EQS*2);
          Print_Matrix (NUM_EQS, NUM_SYS, A, EPS, ERR);
          Get_Num_Eqs (NUM_EQS, DONE);
          ClrScr;
          TextColor (MapColor (LightCyan));
          Writeln (Center ('Simultaneous Equation Solver - KindelCo Software Systems, (C) '
               +long2str (YEAR),currentwidth-1));
          writeln (Center ('Version '+version,currentwidth-1));
        end;
      Title;
    end;
    if Line43 then
      SelectFont8x8 (true);
end.
