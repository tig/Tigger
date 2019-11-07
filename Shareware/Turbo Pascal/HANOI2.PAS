{$R-}    {Range checking off}
{$B+}    {Boolean complete evaluation on}
{$I-}
{$N-}    {No numeric coprocessor}

Program Towers_of_Hanoi2;

Uses
  Crt, Dos;

Const
       Disc_Char = #220;

   Var
       Num_Discs, err
       : integer;
       H1, H2, M2, M1, S1, S2, S1001, S1002 : word;

  Procedure Solve_Hanoi (From_Peg,
                         To_Peg,
                         Temp_Peg,              {the three pegs}
                         N         :  integer); {the number of discs}

  {This procedure moves N discs from From_Peg to To_Peg using Temp_Peg as}
  {a temporary holding peg.}

  begin
    if N <> 1 then
      begin
        Solve_Hanoi (From_Peg, Temp_Peg, To_Peg, N-1);
        Solve_Hanoi (Temp_Peg, To_Peg, From_Peg, N-1);
      end;
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
  Writeln ('Using the Recusive algorithm.');
  GetTime (H1, M1, S1, S1001);
  Solve_Hanoi (1, 3, 2, Num_Discs);
  GetTime (H2, M2, S2, S1002);
  Writeln;
  Writeln ('Elapsed running time = ', H2-H1, ':', M2-M1,':',S2-S1,'.',S1002-S1001);
  Writeln;
end.

