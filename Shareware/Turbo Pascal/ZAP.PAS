Program Zap;

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

  Procedure StartupScreen;
    begin
      SetColor (LightMagenta);
      SetLineStyle (4, 34861, ThickWidth);
      Rectangle (0,0,MaxX, MaxY);
      SetTextJustify (CenterText, CenterText);
      SetTextStyle (4, HorizDir, 7);

      ImageSze := ImageSize (70,270,110,295);
      GetMem (Castle[1], ImageSze);
      GetImage (70, 265, 110, 295, Castle[1]^);
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