Program Tapes; {ver 3.20}

{$C-} {Controls Ctrl-C and S checking (speeds things up)}
{$I-} {Controls I/O checking}
{$R-} {index range check}

{-------------------------- Declarations --------------------------------}

   Type   fieldtype = string [50];
          string80 = string [80];

   Const    Version = 3.20;
            Year = 1987;
         {Printer Codes:}
            Tab = ^I;
            Esc = ^[;
            Comp_On = ^O;         {Compressed on: change to suit your printer}
            Comp_Off = ^R;        {Compressed off: ditto.}
            Dbl_On = 'G';         {Turns on double strike}
            Dbl_Off = 'H';        {Turns double strike off}
         {Graphics Characters:}
            Horiz = #196;          Vert  = #186;
            Right_T = #180;        Left_T = #195;
            Top_T = #210;          Bot_T = #208;
            Cross = #215;
         {Function Keys:}
            F1 = 314;               F2 = 315;
            F10 = 323;
            Up = 327;              Down = 335;
            PgUp = 328;            PgDn = 336;
            SWAP = 328;
            ESCKEY = 282;          CR  = 13;
            DEL = 338;             DELEOL = 372;
            DELFLD = 374;          BKSPC = 8;
            HOME = 326;            ENDD= 334;
            LEFT = 330;            RIGHT = 332;
            INS  = 337;            TOP = 387;
            BOTTOM = 373;


    Var   Song_Names               {Song Names}
          : array [1..40] of fieldtype;

          FieldNum,               {Field number on the screen}
          Pos,                    {Position of cursor on line}
          Htab,
          Vtab,
          i
          : integer;

          New_Page,               {Do we need to send a form feed?}
          Empty,                  {Is the data cleared?}
          InsertMode
          : boolean;

          Key
          : char;

{---------------------------- End Declarations -----------------------------}

{$IDing.pro}                       {Produces a small beep noise}

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

  Procedure Center (Y : integer; Line : string80);
    begin
      GotoXY (40 - round (length(line) / 2)-1, Y);
      write (Line);
    end;

  Procedure TitlePage;
    var  i : integer;
         VerS,
         YearS : string [5];

      begin
        ClrScr;
        HighVideo;
        TextColor (13);
        Center (5, 'T A P E S');
        str (Version:5:2, VerS );
        Center (6, 'Version'+VerS);
        Center (8, '- by -');
        Center (10, 'C.E. Kindel');
        Center (14,'K    I    N    D    E    L    C    O');
        str (Year : 5, YearS);
        Center (15,'Software Systems, copyright (c)' + YearS);
      end;

  Procedure PrintInsertMode;
    begin
      GotoXY (71, 25);
      TextBackground (red);
      TextColor (blue);
      if InsertMode then
        write (' Insert  ');
      if Not InsertMode then
        write ('Overwrite');
      TextBackground (Black);
      TextColor (white);
      LowVideo;
    end;

  Procedure StatLine;
    begin
      Gotoxy (1,24);
      ClrEol;
      LowVideo;
      TextColor (2);
      write ('                <F1> Print        <F2> Clear        <F10> Quit');
      PrintInsertMode;
    end;

  Procedure DataOut;
    var Side, Field, Song : integer;

    begin
      LowVideo;
      For Side := 0 to 1 do
        begin
          case Side of
            0 : begin
                  Window (2, 3, 39,22);
                  ClrScr;
                  GotoXY (1, 1); write (Song_Names[1]);
                end;
            1 : begin
                  Window (42, 3, 79, 22);
                  ClrScr;
                  GotoXY (1, 1); write (Song_Names[20]);
                end;
          end;
          GotoXY (1,2);
          For i := 1 to 38 do Write (Horiz);
          For Song := 2 to 19 do
            begin
              Field := Song + (Side*19);
              GotoXY (1, 1+Song);
              write ('  ', Song_Names[Field])
            end;
        end;
        Window (1, 1, 80, 25);
    end;

  Procedure ScreenOut;
  {outputs the data and the field numbers to the screen}

    Var   Row,
          Line,
          Htab,
          Vtab,
          Field
          : integer;

  Begin
      LowVideo;
      window (1, 1, 80, 25);
      ClrScr;
      TextColor (14);
      TextBackground (9);
      Frame (1, 2, 40, 23);
      Frame (41, 2, 80, 23);
      GotoXY (1, 4);
      Write (Left_T);
      GotoXY (40, 4);
      Write (Right_T, Left_T);
      GotoXY (80, 4);
      Write (Right_T);
      TextColor (3);
      TextBackground (black);
      GotoXY (1,1);
      write ('        Tape Printer V', Version : 4 : 2);
      write ('   - KindelCo Software Systems -   (c) ', Year, '          ');
      LowVideo;
      GotoXY (15, 2); write (' Side A ');
      GotoXY (55, 2); write (' Side B ');
      StatLine;
    end;

  Procedure CleanUp;
    var
       i
       : integer;

    begin
      for i := 1 to 38 do
        Song_Names [i] := '';
      Empty := true;
      FieldNum := 1;
      Pos := 1;
    end;

  Procedure Clear;
  {Clears array to nulls}

    var  Key
         : char;

    begin
      if not Empty then
        begin
          Key := 'N';
          Ding;
          HighVideo;
          GotoXY (1, 24);
          ClrEol;
          write (' Clear Data [y/N]? ');
          Read (Kbd, Key);
          if (UpCase (Key) = 'Y') then
            begin
              cleanup;
              DataOut;
            end;
          StatLine;
        end
    end;


  Procedure Quit (Var Code : integer);
    var  Key
         : char;

    begin
      Ding;
      GotoXY (1, 24);
      ClrEol;
      HighVideo;
      Key := 'N';
      write (' Quit [y/N]? ');
      Read (Kbd, Key);
      if (UpCase (Key) <> 'Y') then
        begin
          Code := 0;
          StatLine;
        end;
      LowVideo;
    end;


  Procedure Print;
    Var  i,
         Field : integer;


    Procedure PrinterSetUp;
      begin
        writeln (Lst, Esc, '0', Comp_On);
        writeln (Lst, Esc, 'D', Chr(2), Chr(36), Chr(68), Chr(0));
      end;

     Procedure PrintField;
       begin
         if Field = 1 then
           begin
             write (Lst, Esc, Dbl_On);
             write (Lst, '|', Tab, copy (Song_Names[1], 1, 32), Tab);
             writeln (Lst, copy(Song_Names[20], 1, 32), Tab, '|', Esc, Dbl_Off);
           end
         else
           begin
             write (Lst, '|', Tab, Song_Names[Field]);
             writeln (Lst, Tab, Song_Names[Field + 19], Tab, '|');
           end;
      end; {PrintField}

     Procedure PrintBlank;
       begin
         writeln (Lst, '|', Tab, Tab, Tab, '|');
       end;

     Procedure PrintLarge;
       begin
         write (Lst, Esc, 'E', Esc, '2');
         writeln (Lst, '  ', Song_Names [1]);
         write (Lst, '  ', Song_Names [20]);
         writeln (Lst, Esc, 'F')
       end; {PrintLarge}

     begin  {Printer}
       HighVideo;
       GotoXY (1, 24);
       ClrEol;
       write (' Printing...');
       Delay (1000);
       PrinterSetUp;
       for i := 1  to 69 do
         write (Lst, '-');
       writeln (Lst);

       for Field := 1 to 19 do
         begin
           PrintField;
           if Field = 1 then PrintBlank;
         end;

       Writeln (Lst, Comp_Off);
       PrintLarge;
       writeln (Lst, Esc, '0', Comp_On);
       for i := 1 to 4 do PrintBlank;
       for i := 1 to 69 do
         write (Lst, '-');
       for i := 1 to 3 do
         writeln (Lst);
       if New_Page then writeln (Lst, chr(12));
     end;

   Function GetKeyCode : integer;

     var code : integer;
         key : char;

     begin
       code := 0;
       read (kbd, Key);
       if (keypressed and (Key = ESC)) then
          begin
            read (kbd, key);
            code := 255;
          end;
       GetKeyCode := code + ord (key);
     end;


   Function GetField (    X, Y,                  {Screen Coords}
                          MaxLen   : integer;    {Maximum length of field}
                      var Pos      : integer;    {Cursor position in field}
                      var Field    : fieldtype   {string to hold field}
                     ) : integer;


    Var    Len, KeyCode : integer;   {current length of field}

    Procedure DelC;
      begin
        if (Len > 0) and (Pos <= Len) then
          begin
            Delete (Field, Pos, 1);
            Write (Copy (Field, Pos, Len), ' ');
          end;
      end;

    Procedure RightC;
      begin
        if (Pos < MaxLen) then
          begin
            Pos := Pos + 1;
            if (Pos > Len) then
              Field := Concat (Field, ' ');
          end;
      end;

    Procedure LeftC;
      begin
        if (Pos > 1) then
            Pos := Pos -1;
      end;

    Procedure BackSpace;
      begin
        if (Pos > 1) then             {if we can backspace}
          begin
            if (Pos = MaxLen) and (Pos = Len) then
               Pos := Pos + 1;
            Pos := Pos - 1;
            Delete (Field, Pos, 1);   {Delete the character}
            GotoXY (X + (Pos-1), Y);
            Write (Copy (Field, Pos, Len), ' ');
          end;
     end;

    Procedure DelEndofLine;
      begin
        if (Len > 0) and (Pos <= Len) then
          begin
            Delete (Field, Pos, Len);
            write (' ':Len - Pos+1);
          end;
      end;

    Procedure DelField;
      begin
        Pos := 1;
        Field := ' ';
        GotoXY (X + (Pos-1), Y);
        write (' ':Len);
      end;

    Procedure PrintableChar;
      var c : char;
      begin
        c := chr (keycode);
        if (InsertMode) and (Len < MaxLen) then        {if the string is not too long}
           begin
             Insert (c, Field, Pos); {insert the character}
             Write (Copy (Field, Pos, Len+1));
             if (Pos < Maxlen) then
               Pos := Pos + 1;
           end
        else
          begin
            Delete (Field, Pos, 1);
            Insert (c, Field, Pos);
            if (Pos < MaxLen) then
              Pos := Pos + 1;
            write (c);
          end
      end;


    begin
      GotoXY (X, Y);                             {go to field on screen}
      Write (Field);
      repeat                                     {until Status <> 0}
         GotoXY (X+ (Pos-1), Y);
         Len := Length (Field);                  {How long is the string?}
         KeyCode := GetKeyCode;
         case KeyCode of
            DEL   : DelC;
            LEFT  : LeftC;
            RIGHT : RightC;
            HOME  : Pos := 1;
            ENDD  : if (Len < MaxLen) then
                      Pos := Len+1
                    else
                      Pos := Len;
            DELEOL : DelEndofLine;
            DELFLD : DelField;
            INS    : begin
                       InsertMode := Not (InsertMode);
                       PrintInsertMode;
                     end;
            CR     : KeyCode := DOWN;           {it was a RETURN}
            BKSPC  : BackSpace;
            else
              if (KeyCode < 256) then
                 PrintableChar;
        end {case}
      until (KeyCode > 255);
      GetField := KeyCode;
    end;


  Procedure WhatField (Var Num_Chars, FieldNum : integer);

    begin
      If FieldNum > 19 then         {if field is on the right side (B)}
        begin
          Htab := 46; Vtab := FieldNum - 16;
        end
      else                       {or if it is on the left side (A)}
        begin
          Htab := 6; Vtab := FieldNum + 3
        end;
      if (FieldNum = 1) or (FieldNum = 20) then  {it is a Title}
        begin
          Num_Chars := 37;                  {it can be 37 chars long}
          Htab := Htab - 4;                {titles are longer...}
          Vtab := 3;
        end
      else                                 {it is a song}
        Num_Chars := 31;                    {it can be 31 chars long}
    end;



  Procedure FKeys;

    Var
         Num_Chars,
         Status
         : integer;

         EscCode
         : char;

    begin
      repeat
        WhatField (Num_Chars, FieldNum);
        if (Pos > length (Song_Names [FieldNum])) then
          Pos := length (Song_Names [FieldNum]) + 1;
        Status := GetField (Htab, Vtab, Num_Chars, Pos, Song_Names [FieldNum]);
        If (Song_Names[FieldNum] <> ' ') then
           Empty := false;
        Case Status of
          DOWN     : if (FieldNum = 38) then        {or CR}
                        FieldNum := 1
                     else
                        FieldNum := FieldNum + 1;
          Up       : if (FieldNum = 1) then
                        FieldNum := 38
                     else
                        FieldNum := FieldNum - 1;
          SWAP,PgUp,PgDn
                   : if FieldNum < 20 then
                         FieldNum := FieldNum + 19
                     else
                         FieldNum := FieldNum - 19;
          TOP      : if FieldNum >= 20 then
                        FieldNum := 20
                     else
                        FieldNum := 1;
          BOTTOM   : if FieldNum >= 20 then
                        FieldNum := 38
                     else
                        FieldNum := 19;
          F1       : Begin
                       Print;
                       New_Page := Not New_Page;
                       StatLine;
                     end;
          F2       : Clear;
          F10      : Quit (Status);
        end {case}
      until (Status = ord(F10));
    end;

{-------------------------  Main Program  ---------------------------------}

Begin
  New_Page := false;
  Empty := false;
  InsertMode := True;
  CleanUp;
  TitlePage;
  i := 0;
  repeat
    i := i + 1;
  until ((KeyPressed) or (i > 15000));
  ScreenOut;
  DataOut;
  FKeys;
  Window (1, 1, 80, 25);
  TitlePage;
end.

