Program Attack;

Uses
  Graph,
  Crt;

Type ImagePtr = pointer;

Var
  GraphDriver,
  GraphMode,
  ErrorCode,
  MaxX, MaxY
  : integer;

  ImageSze : word;
  Castle : array [1..2] of ImagePtr;

  Procedure DrawBattleField;
    Const Field : array [1..5] of PointType =
                  ((x:90; Y:250),
                   (x:30; Y:320),
                   (x:570;Y:320),
                   (x:620;Y:250),
                   (x:90; Y:250));

    begin
      SetColor (Green);
      SetFillStyle (WideDotFill, Green);
      FillPoly (5, Field);
    end;




  Procedure DrawCastle (X, Y, CastleColor : word);

    const Xrad = 18;
          Yrad = 6;

    var i,j : integer;

    begin
      SetColor (CastleColor);
      for i := 6 downto 1 do
        Ellipse (X,Y-(i), 0,   359, Xrad, Yrad);
      for i := 1 to 10 do
        Ellipse (X,Y+(i), 180, 0,   Xrad-(Xrad div 4), Yrad-(Yrad div 4));
      SetColor (Black);
      Ellipse (X,Y, 180, 0, Xrad, Yrad);
      Ellipse (X,Y-7, 180, 0, Xrad, Yrad);
      SetColor (CastleColor);
      Ellipse (X,Y-7, 0, 180, Xrad, Yrad);
      SetColor (Black);
      for j := 9 to 18 do
        Ellipse (X, Y-4, j*20, j*20+10, Xrad, Yrad);
    end;


  Procedure StartupScreen;
    begin
      SetColor (LightMagenta);
      SetLineStyle (4, 34861, ThickWidth);
      Rectangle (0,0,MaxX, MaxY);
      SetTextJustify (CenterText, CenterText);
      SetTextStyle (4, HorizDir, 7);
      SetColor (LightGreen);
      MoveTo (MaxX div 2, (MaxY div 8)-10);
      OutText ('Castle Attack');
      SetTextStyle (3, HorizDir, 4);
      MoveTo (MaxX div 2, MaxY div 4);
      SetColor (LightRed);
      OutText ('(C) 1988, KindelCo Software Systems');
      MoveTo (MaxX div 2, MaxY div 3);
      OutText ('All Rights Reserved');

      DrawCastle (90, 280, LightCyan);
      ImageSze := ImageSize (70,270,110,295);
      GetMem (Castle[1], ImageSze);
      GetImage (70, 265, 110, 295, Castle[1]^);

      DrawCastle (570, 280, LightBlue);
      GetMem (Castle[2], ImageSze);
      GetImage (550, 265, 590, 295, Castle[2]^);
      DrawBattleField;
      PutImage (70,265,Castle[1]^,NormalPut);
      PutImage (550,265, Castle[2]^, NormalPut);
    end;



begin
  GraphDriver := Detect;
  InitGraph (GraphDriver, GraphMode, '.');
  ErrorCode := GraphResult;
  if ErrorCode <> grOK then
    begin
      TextColor (LightRed);
      ClrScr;
      Writeln ('Graphics Error: ', GraphErrorMsg (ErrorCode));
      Writeln ('Program aborted...');
      Halt (1);
    end;
  MaxX := GetMaxX;
  MaxY := GetMaxY;
  StartupScreen;
  Readln;
  CloseGraph;
  Writeln ('ErrorCode= ', ErrorCode);
end.