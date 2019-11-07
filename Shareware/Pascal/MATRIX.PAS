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
{----------------------------------------------------------------------------}

Program Matrix;

  const  Row = 103;
         Col = 104;
         Date = 1987;

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
         DONE
         : boolean;

         CHARACTER
         : char;

         FIL
         : text;

{$ICenter.pro}
{$IFrame.pro}
{$IKindelco.pro}
{$INum\Epsilon.pas}
{$INum\NEWGAUSS.PAS}

  Procedure Title;
    begin
      ClrScr;
      Center (1, Concat
             (chr (240), chr (240), ' Simultaneous Equation Solver ',
              chr (240), chr (240)));
      Center (3, 'by Charles E. Kindel');
      LowVideo;
      Center (5, 'Gaussian elimination by Sid Yakowitz');
      Center (6, 'Many thanks to Mark Jenkins and Mike Brehm for inspiration');
      HighVideo;
    end;

  Procedure Clear_Matrix (var Matrix    : matrixtype);

    var x, y : integer;

    begin
      for x := 1 to 100 do
        for y := 1 to 101 do
          Matrix[x,y] := 0;
    end;

  Procedure Draw_Brackets (SIZE : integer);

    var i : integer;

    begin
      Gotoxy (1, 4); write (chr(218));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (1, 4+i); write (chr(179));
        end;
      gotoxy (1, 4+SIZE*2); write (chr (192));

      Gotoxy (SIZE*8+2, 4); write (chr(191));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (SIZE*8+2, 4+i); write (chr(179));
        end;
      gotoxy (SIZE*8+2, 4+SIZE*2); write (chr (217));

      Gotoxy (SIZE*8+4, 4); write (chr(218));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (SIZE*8+4, 4+i); write (chr(179));
        end;
      gotoxy (SIZE*8+4, 4+SIZE*2); write (chr (192));

      for i := 1 to SIZE do
        begin
          gotoxy (SIZE*8+6, 3+2*i);
          write ('X', i:1);
        end;
      Gotoxy (SIZE*8+9, 4); write (chr(191));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (SIZE*8+9, 4+i); write (chr(179));
        end;
      gotoxy (SIZE*8+9, 4+SIZE*2); write (chr (217));
      gotoxy (SIZE*8+10, 4+SIZE);
      write ('=');
      gotoxy (SIZE*8+11, 4); write (chr(218));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (SIZE*8+11, 4+i); write (chr(179));
        end;
      gotoxy (SIZE*8+11, 4+SIZE*2); write (chr (192));
      gotoxy (SIZE*8+19, 4); write (chr(191));
      for i := 1 to (SIZE*2 - 1) do
        begin
          gotoxy (SIZE*8+19, 4+i); write (chr(179));
        end;
      gotoxy (SIZE*8+19, 4+SIZE*2); write (chr (217));
    end;


  Procedure Get_Matrix (var A       : matrixtype;
                            NUM_EQS : integer;
                            RES     : integer);

    var   i, j, k : integer;

      Procedure Goto_Field (x, y, RES : integer);

        begin
          if y = RES then
            begin
              HighVideo;
              TextBackGround (white);
              TextColor (black + blink);
              gotoxy (y*8+4, x*2+3);
              write (A[x, RES] :6:1);
              TextBackground (black);
              TextColor (White);
              gotoxy (1,5+NUM_EQS*2); ClrEol;
              write ('Enter value for B[',i,'] or [ENTER] to keep ', chr(26), ' ');
              readln (A[x,RES]);
              gotoxy (1,5+NUM_EQS*2); ClrEol;
              gotoxy ((y)*8+4, x*2+3);
              for k := 1 to 5 do
                write (' ');
              gotoxy ((y)*8+4,x*2+3);
              write (A[x, RES] :6:1);
            end
          else
            begin
              HighVideo;
              gotoxy (y*8-5, x*2+3);
              for k := 1 to 5 do
                write (' ');
              gotoxy (y*8-5,x*2+3);
              TextBackGround (white);
              TextColor (black + blink);
              write (A[x,y] :6:1);
              TextBackground (black);
              TextColor (White);
              gotoxy (1, 5+NUM_EQS*2); ClrEol;
              write ('Enter value for A[', x:1, ':',y:1, '] or [ENTER] to keep ', chr(26), ' ');
              readln (A[x,y]);
              gotoxy (1,5+NUM_EQS*2); ClrEol;
              HighVideo;
              gotoxy (y*8-5, x*2+3);
              for k := 1 to 5 do
                write (' ');
              gotoxy (y*8-5,x*2+3);
              write (A[x,y] :6:1);
            end;
         end;  {Goto_Field}


    begin
      for i := 1 to NUM_EQS do
        begin
          for j := 1 to NUM_EQS do
            begin
              gotoxy (j*8-5,i*2+3);
              write (A[i,j] :6:1);
            end;
            gotoxy ((RES)*8+4,i*2+3);
            write (A[i, RES] :6:1);
        end;
      for i := 1 to NUM_EQS do
        begin
          for j := 1 to NUM_EQS do
            Goto_Field (i,j, RES);
          Goto_Field (i, RES,RES);
        end;
    end;

  Procedure Get_Num_Eqs (var NUM  : integer;
                         var FLAG : boolean);

    begin
      HighVideo;
      FLAG := false;
      repeat
        Gotoxy (1,22);
        ClrEol;
        NUM := 0;
        Write ('Enter number of Eqations/Unknowns [ENTER to quit]: ');
        readln (NUM);
        If NUM = 0 then FLAG := true;
        Gotoxy (1,22);
        ClrEol;
      until ((NUM_EQS > 1) and (NUM_EQS < 8)) or (DONE);
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
                                         A : matrixtype;
                                       EPS : real;
                                       ERR : boolean);
    var i : integer;

    begin
      Writeln ('Computing matrix...');
      Gauss (NUM_EQS, NUM_SYS, A, EPS, ERR);
      if ERR then
        writeln ('ERROR...Matrix is singular')
      else
        begin
          writeln;
          ClrEol;
          Write ('Do you want output to go to printer? [Y/n]: ');
          read (kbd, Character);
          clrscr;
          for i := 1 to num_Eqs do
            writeln ('X', i, ' = ', A[i, NUM_EQS+NUM_SYS] :8:6, '    ');
          If character in ['Y', 'y', chr (13)] then
            begin
              for i := 1 to num_eqs do
                writeln (lst, 'X', i, ' = ', A[i, NUM_EQS+NUM_SYS] :8:6, '    ');
              writeln (lst, chr(12));
            end;
          Writeln;
        end;
    end; {print}


begin
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
      KindelCo (DATE);
      gotoxy (64, 25); write ('Press a key...');
      Read (Kbd, character);
      ClrScr;
      Center (1, 'Simultaneous Equation Solver - KindelCo Software Systems, (C) 1987');
      Center (2, 'by Charles Kindel');
      Gotoxy (1,22);
      write ('Wait...');
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
          Draw_Brackets (NUM_EQS);
          LowVideo;
          Get_Matrix (A, NUM_EQS, RES);
          HighVideo;
          Gotoxy (1, 6+NUM_EQS*2);
          Print_Matrix (NUM_EQS, NUM_SYS, A, EPS, ERR);
          Get_Num_Eqs (NUM_EQS, DONE);
        end;
      Title;
      KindelCo (DATE);
    end;
end.
