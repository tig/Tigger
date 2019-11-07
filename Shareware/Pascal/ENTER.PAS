Program Enter;

  type  fieldtype = string [255];

  Var  Pos, Status : integer;
       Field       : fieldtype;
       InsertMode  : boolean;

  Procedure GetField (    X, Y,                  {Screen Coords}
                          MaxLen   : integer;    {Maximum length of field}
                      var Pos      : integer;    {Cursor position in field}
                      var Field    : fieldtype;  {string to hold field}
                      var Status   : integer);   {return status (for Fkeys etc...)}

    Const  ESC = ^[;             CR  = ^M;
           DEL = #83;            DELEOL = #117;
           DELFLD = #119;        BKSPC = ^H;
           HOME = #71;           ENDD= #79;
           LEFT = #75;           RIGHT = #77;
           INS  = #82;

    Var    Len : integer;   {current length of field}
           Key : char;

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
      begin
        if (InsertMode) and (Len < MaxLen) then        {if the string is not too long}
           begin
             Insert (Key, Field, Pos); {insert the character}
             Write (Copy (Field, Pos, Len+1));
             if (Pos < Maxlen) then
               Pos := Pos + 1;
           end
        else
          begin
            Delete (Field, Pos, 1);
            Insert (Key, Field, Pos);
            if (Pos < MaxLen) then
              Pos := Pos + 1;
            write (Key);
          end
      end;

    begin
      Status := 0;                               {Init status to 0}
      GotoXY (X, Y);                             {go to field on screen}
      Write (Field);
      GotoXY (X+ (Pos-1), Y);
      repeat                                     {until Status <> 0}
         GotoXY (X, Y-2);
         ClrEol;
         write (field);
         GotoXY (1,1);
         if InsertMode then
            write (chr(219))
         else
            write (' ');
         GotoXY (X+ (Pos-1), Y);
         Len := Length (Field);                  {How long is the string?}
         read (Kbd, Key);                        {get a keypress}
         case Key of
           ESC :   if KeyPressed then            {if it was a function key}
                     begin
                       read (Kbd, Key);          {read rest of function}
                       case Key of
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
                          INS    : InsertMode := Not (InsertMode);
                       else
                          Status := Ord (Key);      {get F-key value}
                       end {case}
                     end
                   else
                     Status := Ord (ESC);        {it was the ESC key}
           CR    : Status := Ord (CR);           {it was a RETURN}
           BKSPC : BackSpace;        else
           PrintableChar;
        end {case}
      until (Status <> 0);
    end;

begin
  ClrScr;
  InsertMode := True;
  Field := ' ';
  Pos := 1;
  Status := 0;
  GotoXY (5, 9);
  Write ('1234567890123456789012345678901234567890');
  GetField (5, 10, 40, Pos, Field, Status);
  GotoXY (1, 20);
  Writeln ('Pos = ', Pos, ' Field = ', Field, ' Status = ', Status);
end.


