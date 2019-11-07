Program Envelope;

uses
  tpcrt, printer;

const
  VER = 1.0;
  ESC = #27;
  FF  = #12;

var
  ret : array [1..6] of string;
  addr: array [1..6] of string;
  i : integer;
  key : char;
  Line43 : boolean;

begin
  if CurrentDisplay >= EGA then
    if Font8x8Selected then
      begin
        Line43 := True;
        SelectFont8x8 (false);
      end
        else
          Line43 := false;
  DirectVideo := true;

  clrscr;
  for i := 1 to 6 do
    begin
      ret[i] := '';
      addr[i] := '';
    end;
  ret [1] := 'Charles E. Kindel, Jr.';
  ret [2] := '1509 E. Second Street';
  ret [3] := 'Tucson, AZ  85719';
  TextBackGround (Blue);
  TextColor (White);
  writeln ('  Envelope Printer  -  (c) KindelCo Software Systems 1988  -  Version ', VER:4:2,' ':5);
  textbackground (Black);
  TextColor (Yellow);
  writeln;
  write ('Use default return address ([ESC] to exit)?');
  key := upcase(readkey);
  textcolor (LightMagenta);
  if Key = ESC then halt;
  if (key = 'N') then
    begin
      writeln ('Enter return address below (blank line to end)');
      writeln;
      i := 1;
      repeat
        write ('Return Address Line ',i:1,': '); Readln (Ret[i]);
        inc (i);
      until ((i = 7) or (Ret[i-1] = ''));
    end;
  writeln;
  writeln ('Enter address below (blank line to end)');
  writeln;
  i := 1;
  repeat
    write ('Address Line ', i:1,': '); Readln (addr[i]);
    inc (i);
  until ((i = 7) or (Addr[i-1] = ''));

  clrscr;
  TextColor (LightGreen);
  for i := 1 to 6 do
    writeln (ret[i]);
  for i := 1 to 6 do
    writeln;
  for i := 1 to 6 do
    writeln (' ':40, addr[i]);
  writeln;
  writeln;
  TextColor (White);
  writeln ('-----------------------------------------------------------------------------');
  writeln;
  TextColor (Yellow);
  write ('Print this envelope?');
  key := upcase(readkey);
  if key = 'Y' then
    begin
      TextColor (LightRed);
      ClrScr;
      writeln ('Insert Envelope in printer so that top of envelope is even');
      writeln ('with print head.  Press any key when ready.');
      key := readkey;
      if (key = ESC) then halt;
      writeln ('Printing...');
      write (lst, ESC, 'I2', ESC, '8', ESC, 'C', chr(0), chr(4));
      for i := 1 to 6 do
        writeln (lst, ret[i]);
      for i := 1 to 6 do
        writeln (lst);
      for i := 1 to 6 do
        writeln (lst, ' ':40, addr[i]);
      writeln (lst, FF);
      writeln ('Done!');
    end;
    if Line43 then
      SelectFont8x8 (true);
end.


