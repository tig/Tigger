{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$I-}    {I/O checking on}
{$N-}    {No numeric coprocessor}

Program Towers_of_Hanoi4;

Uses
  Crt, Dos;

Const
       Disc_Char = #220;

   Var
       Num_Discs, err
       : integer;

       H1, H2, M1, M2, S1, S2, S1001, S1002 : word;

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

    1:
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
  if Paramcount <> 0 then
    Val (ParamStr (1), Num_Discs, Err)
  else
    repeat
      write ('Enter the number of discs you want to move:');
      readln (Num_Discs);
    until (Num_Discs <= 100);
  Writeln;
  Writeln ('Computing The Towers of Hanoi for ', Num_Discs, ' discs.');
  Writeln ('Using Non-Recusive algorithm.');
  GetTime (H1, M1, S1, S1001);
  Solve_Hanoi (1, 3, 2, Num_Discs);
  GetTime (H2, M2, S2, S1002);
  Writeln;
  Writeln ('Elapsed running time = ', H2-H1, ':', M2-M1,':',S2-S1,'.',S1002-S1001);
  Writeln;
end.

