PROGRAm strip;

const CR = ^M;
      LF = #115;
      TAB = ^I;
      EXP = '     ';

var infil, outfil : text;
    character : char;

begin
  assign (outfil, 'by-laws.out');
  rewrite (outfil);
  assign (infil, 'by-laws.asc');
  reset (infil);
  while not EOF(infil) do
    begin
      read (infil, character);
      case character of
        CR : begin
               writeln (OutFil);
               writeln ('LineFeed');
             end;
        TAB : begin
                  write (OutFil, EXp);
                  writeln (Exp);
              end;
        else
            begin
              write (outfil, character);
              write (character);
            end;
      end;
    end;
  close (infil); close (outfil);
end.