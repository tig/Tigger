
  procedure Frame(UpperLeftX, UpperLeftY, LowerRightX, LowerRightY: Integer);
  var  I : Integer;

    begin {Frame}
      GotoXY(UpperLeftX, UpperLeftY);
      Write(chr(218));
      for I := (UpperLeftX + 1) to (LowerRightX - 1) do
        Write(chr(196));
      Write(chr(191));
      for I := (UpperLeftY + 1) to (LowerRightY - 1) do
        begin
          GotoXY(UpperLeftX , I);  Write(chr(179));
          GotoXY(LowerRightX, I);  Write(chr(179));
        end;
      GotoXY(UpperLeftX, LowerRightY);
      Write(chr(192));
      for I := (UpperLeftX + 1) to (LowerRightX - 1) do
        Write(chr(196));
      Write(chr(217));
    end; {Frame}
