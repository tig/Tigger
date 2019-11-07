program convert_font;

uses Crt;

type c_type = array [0..255, 1..8, 1..14] of boolean;

var row, pad, col, c_num, i : integer;
    c : char;
    charset : c_type;
    infile, con : text;
    fontname, st : string [80];



  Procedure ReadFont;

    begin
      write ('Reading ', fontname, ' into memory...');
      for c_num := 0 to 255 do
      begin
        write (c_num:3, ^H, ^H, ^H);
        for row := 1 to 14 do
          begin
            for i := 1 to 3 do
              read (infile, c);
              for col := 1 to 8 do
                begin
                  read (infile, c);
                  if (c = '1') then
                    charset [c_num, col, row] := true
                  else
                    charset [c_num, col, row] := false;
                end;
            readln (infile, c);
          end;
        readln (infile);
      end;
    end;




begin
  writeln;
  if paramcount <> 0 then
    fontname := paramstr (1)
  else
    fontname := 'font.chr';
  assign (infile, fontname);
  reset (infile);
  assign (con , '');
  rewrite (con);
  ReadFont;
  writeln;
  write ('Enter the character you want for each pixel: ');
  c := ReadKey;
  if (c = ^M) then c := chr (219);
  writeln (c);
  writeln;
  repeat
    st[1] := ^M;
    write ('Enter string you want: '); readln (st);
    if (st[1] <> ^M) then
      begin
        pad := (10 - length (st)) div 2;
        for i := 1 to pad-1 do
          st := concat (' ', st);
        for row := 1 to 14 do
          begin
            for i := 1 to length (st) do
              begin
                if i < 10 then
                  for col := 1 to 8 do
                    if (charset [ord(st[i]), col, row]) then
                      write (con, c)
                    else
                      write (con, ' ');
              end;
            writeln (con);
          end
        end;
  until (st[1] = ^M);
  close (infile);
end.

