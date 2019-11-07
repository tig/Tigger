{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$S+}    {Stack checking on}
{$I+}    {I/O checking on}
{$N-}    {No numeric coprocessor}

Program Towers_of_Hanoi;

Uses
  Crt;

Const
       Disc_Char = #220;

   Var
       Num_Discs,
       A,
       B,
       C,
       i,
       j
       : integer;

{$I Center.pro}


const
     ArraySize = 100;

type
     stacktype = array [1..ArraySize] of integer;


  procedure Push (    ITEM  : integer;
                  var STACK : stacktype;
                  var TOP   : integer;
                  var FULL  : boolean);

    begin
      if TOP = ArraySize then
        FULL := true
      else
        begin
          FULL := FALSE;
          TOP := TOP + 1;
          STACK [TOP] := ITEM;
        end;
    end;

  procedure Pop (var ITEM  : integer;
                 var STACK : stacktype;
                 var TOP   : integer;
                 var EMPTY : boolean);

    begin
      if TOP = 0 then
        EMPTY := TRUE
      else
        begin
          EMPTY := FALSE;
          ITEM  := STACK [TOP];
          TOP   := TOP - 1;
        end;
    end;



  Procedure Move_Disc ( Disc_Num,
                           SOURCE,
                           DESTINATION     : integer);

    var
        i
        : integer;

    begin
      textcolor (black);
      Case SOURCE of
        1 : begin
              GotoXY ((14 - Disc_Num), 22 - A);
              for i := 1 to 2*Disc_Num  do
                write (' ');
              A := A - 1;
            end;
        2 : begin
              GotoXY ((41 - Disc_Num), 22 - B);
              for i := 1 to (2*Disc_Num)  do
                write (' ');
              B := B - 1;
            end;
        3 : begin
              GotoXY ((67 - Disc_Num), 22 - C);
              for i := 1 to (2*Disc_Num)  do
                write (' ');
              C := C - 1;
            end;
      end;
      textcolor (disc_num);
      Case DESTINATION of
        1 : begin
              GotoXY ((14 - Disc_Num), 22 - A - 1);
              for i := 1 to (2*Disc_Num) do
                write (Disc_Char);
              A := A + 1
            end;
        2 : begin
              GotoXY ((41 - Disc_Num), 22 - B - 1);
              for i := 1 to (2*Disc_Num) do
                 write (Disc_Char);
              B := B + 1
            end;
        3 : begin
              GotoXY ((67 - Disc_Num), 22 - C - 1);
              for i := 1 to (2*Disc_Num) do
                 write (Disc_Char);
              C := C + 1
            end;
      end;
    end;


  Procedure Solve_Hanoi (SOURCE,
                         DESTINATION,
                         INTERMEDIATE,              {the three pegs}
                         N         :  integer); {the number of discs}

  {This procedure moves N discs from SOURCE to DESTINATION using INTERMEDIATE as}
  {a temporary holding peg.}

  label 1, 2, 3, 10;

  var NSTACK, STACK, LSTACK : stacktype;
      TOP, L, LTOP,NTOP, TEMP : integer;
      EMPTY, FULL : boolean;


  begin
   TOP := 0;
   LTOP := 0;
   NTOP := 0;

   3 : if N = 1 then
         begin
           Move_Disc (N, SOURCE, DESTINATION);
           pop (N, NSTACK, NTOP, EMPTY);
           pop (INTERMEDIATE, STACK, TOP, EMPTY);
           pop (DESTINATION, STACK, TOP, EMPTY);
           pop (SOURCE, STACK, TOP, EMPTY);
           pop (L, LSTACK, LTOP, EMPTY);
           if EMPTY then
             goto 10
           else
             case L of
               1: goto 1;
               2: goto 2;
             end;
         end;

      push (N, NSTACK, NTOP, FULL);
      push (SOURCE, STACK, TOP, FULL);
      push (DESTINATION, STACK, TOP, FULL);
      Push (INTERMEDIATE, STACK, TOP, FULL);

      N := N-1;
      TEMP := INTERMEDIATE;
      INTERMEDIATE := DESTINATION;
      DESTINATION := TEMP;

      push (1, LSTACK, LTOP, FULL);

      goto 3;

    1: Move_disc (N, SOURCE, DESTINATION);
       push (N, NSTACK, NTOP, FULL);
       push (SOURCE, STACK, TOP, FULL);
       push (DESTINATION, STACK, TOP, FULL);
       push (INTERMEDIATE, STACK, TOP, FULL);

       N := N-1;
       TEMP := INTERMEDIATE;
       INTERMEDIATE := SOURCE;
       SOURCE := TEMP;

       Push (2, LSTACK, LTOP, FULL);

       goto 3;

    2:
       pop (N, NSTACK, NTOP, EMPTY);
       pop (INTERMEDIATE, STACK, TOP, EMPTY);
       pop (DESTINATION, STACK, TOP, EMPTY);
       pop (SOURCE, STACK, top, EMPTY);
       pop (L, LSTACK, LTOP, EMPTY);
       if EMPTY then
         goto 10
       else
         case L of
           1 : goto 1;
           2 : goto 2;
         end;

    10:

  end;


begin
  ClrScr;
  center (2, 'The Towers of Hanoi');
  GotoXY(14, 22); write ('1');
  GotoXY(41, 22); write ('2');
  GotoXY(67, 22); writeln ('3');
  repeat
    GotoXY (1, 24);
    write ('Enter the number of discs you want to move:');
    read (Num_Discs);
  until (Num_Discs <= 13);
  A := 0;
  B := 0;
  C := 0;
  i := Num_Discs-1;
  While (i > 0) do
    begin
      textcolor (i+1);
      GotoXY ((14 - i), 22 - A - 1);
      for j := 1 to (2*i) do
        write (Disc_Char);
      A := A + 1;
      i := i-1;
    end;
  A := Num_Discs;
  Solve_Hanoi (1, 3, 2, Num_Discs);
  GotoXY (1,1);
end.
