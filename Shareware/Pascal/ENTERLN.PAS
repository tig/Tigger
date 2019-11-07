

  Procedure EnterLn (Var StringToEdit : StringFifty;
                     MaxLen : integer;
                     Var CursPos : integer;
                     Var EscKey : char);


    Type
        Alpha = set of ' '..'~';

    Const
        AlphaChars : Alpha = [' '..'~'];
        BkSpc = ^H;
        Del = #83;
        Ins = #82;
        Zap = ^Y;
        Esc = ^[;
        Endd = #116;
        Home = #115;
        Left = #75;
        Right = #77;

    Var
        Key,
        CopyChar
        : char;

        i,
        Len
        : integer;

        EscFlag,
        InsertMode
        : boolean;


    Procedure JumpToBeginning;

      begin
        For i := 1 to CursPos - 1 do
          Write (BkSpc);
        CursPos := 1;
      end;

    Procedure ClrEof;

      begin
        For i := CursPos to MaxLen do
          write (' ');
        for i := CursPos to MaxLen do
          write (BkSpc);
      end;

   Procedure FixLine;

     begin
       write (Copy (StringToEdit, CursPos, (Len - CursPos) + 1), ' ');
       for i := CursPos to Len + 1 do
         write (BkSpc)
     end;

   Procedure KeyPress;

      begin
        Case Key of
          Esc   : if KeyPressed then
                    begin
                      Read (Kbd, Key);
                      EscFlag := false;
                      Case Key of
                        Left  : if CursPos > 1 then
                                  begin
                                    CursPos := CursPos - 1;
                                    write (BkSpc);
                                  end;
                        Right : if CursPos - 1 <= MaxLen then
                                begin
                                  if (CursPos <= Len) or (CursPos -1 < MaxLen) then
                                    begin
                                      if (CursPos <= Len) then
                                        CopyChar := Copy (StringToEdit, CursPos, 1)
                                      else
                                        begin
                                          CopyChar := ' ';
                                          StringtoEdit := StringToEdit + ' '
                                        end;
                                      write (CopyChar);
                                      CursPos := CursPos + 1;
                                    end;
                                end;
                        Del   : if CursPos <= Len then
                                begin
                                  Delete (StringToEdit, CursPos, 1);
                                  Len := Len - 1;
                                  FixLine;
                                end;
                        Ins   : InsertMode := not InsertMode;
                        Home  : JumpToBeginning;
                        Endd  : begin
                                  JumpToBeginning;
                                  write (StringToEdit);
                                  CursPos := Len + 1;
                                end
                      Else
                        EscFlag := true;
                      end; {Case}
                  end;
          BkSpc : if CursPos > 1 then
                    begin
                      CursPos := CursPos - 1;
                      write (BkSpc);
                      if CursPos <= Len + 1 then
                        begin
                          Delete (StringToEdit, CursPos, 1);
                          Len := Len - 1;
                          FixLine
                        end
                    end;
           Zap  : if len > 0 then
                    begin
                      JumpToBeginning;
                      StringToEdit := '';
                      Len := 0;
                      ClrEof;
                    end;
           CR   : begin
                    Delete (StringToEdit, CursPos, (Len - CursPos) + 1);
                    ClrEof;
                    EscKey := CR;
                    EscFlag := true;
                  end;
         ' '..'~' : if (CursPos = Len + 1) and (Len < MaxLen) then
                      begin
                        StringToEdit := StringToEdit + Key;
                        write (Key);
                        CursPos := CursPos + 1;
                      end
                    else
                      if (Not InsertMode) or (Len = MaxLen) then
                        begin
                          if (Len < MaxLen) or (CursPos < Len + 1) then
                            begin
                              Delete (StringToEdit, CursPos, 1);
                              Insert (Key, StringToEdit, CursPos);
                              CursPos := CursPos + 1;
                              write (Key);
                            end;
                        end
                      else
                        if InsertMode and
                           (CursPos <= Len) and
                           (Len < MaxLen) then
                          begin
                            Insert (Key, StringToEdit, CursPos);
                            write (Key);
                            Len := Len + 1;
                            CursPos := CursPos + 1;
                            FixLine;
                          end;

      end; {case}
    end;

  begin
    EscFlag := false;
    InsertMode := false;
    Len := Length (StringToEdit);
    write (StringToEdit);
    For i := 1 to (Len - CursPos + 1) do
      write (BkSpc);
    repeat
      Read (Kbd, Key);
      KeyPress;
      Len := Length (StringToEdit);
    until EscFlag;
  end;

