program calendar;


uses Calendar,
     TpString,
     TpCrt;

Type
     Day    = (Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday);

const
     CalFG = LightMagenta;
     CalBG = Black;
     DayStr : array [Day] of string = ('Sunday','Monday','Tuesday','Wednesday',
                                            'Thursday','Friday','Saturday');
var ch : char;
    CrtMode : byte;
    i : byte;
    S : string;


  Procedure HorizLine (Y : byte);
    var S : string;
        i : byte;
    begin
      S := CharStr (#196, CurrentWidth-5);
      S[1] := #195; S[CurrentWidth-5] := #180;
      for i := 1 to 6 do
        S [i*18+1] := #197;
      FastWrite (S, Y, 1, CalFG+(CalBG*16));
    end;

  Procedure VertBars (Y : byte);
    var S : string;
        i : byte;
    begin
      S := CharStr (' ', CurrentWidth-8);
      for i := 1 to 6 do
        S [i*18] := #179;
      fastwrite (S, Y, 2, CalFG+(CalBG*16));
    end;



  Procedure DrawCal;
    var x,y : byte;
        D   : day;
    begin
      SetFrameChars(#179, #196,#217, #191, #192,#218);
      FrameWindow (1,4,CurrentWidth-5, Succ(CurrentHeight), CalFG+(CalBG*16), CalFG,'');
      For y := 5 to CurrentHeight do
        VertBars (y);
      for y := 2 to 5 do
        HorizLine (y*8-4);
      for x := 1 to 6 do
        begin
          FastWrite (#194, 4, x*18+1, CalFG+(CalBG*16));
          FastWrite (#193, CurrentHeight+1, x*18+1, CalFG+(CalBG*16));
        end;
      for D := Sunday to Saturday do
        FastWrite (DayStr [D], 3, byte(D)*18+6, White);
    end;


begin
  CrtMode := GetCrtMode;
  if CurrentWidth = 132 then
    begin
      window (1,1,CurrentWidth,CurrentHeight+1);
      clrscr;
      DrawCal;

      ch := readkey;
    end
  else
    Writeln ('Video must be in 132x44 to run.');
end.