
Program Demo; {This is the object program to demonstrate sDefs.TPU }

    Uses
      CRT;

    Type
      DefRecType = Record
        St:           String[32];
        Yr,Mth,Day:   Word;
      end;

    Const
      idHeader:   String[8]  = 'XmtSpot ';{ << This is exactly what install}
      DefaultRec: DefRecType =            {program looks for.              }
        (
          St:  'Not important';
          Yr:  1986;
          Mth: 10;
          Day: 10
        );
      BgColor:  Byte = Blue;
      FgColor:  Byte = Yellow;
      LastByte: Byte = 00;                {Be consistent with declarations }
                                          {in installation program.        }
  BEGIN
    TextColor(FgColor);
    TextBackground(BgColor);
    WriteLn;
    WriteLn('Demonstration string: ',DefaultRec.St);
    With DefaultRec do
      WriteLn('Written: ',Mth,'-',Day,'-',Yr);
    Delay(4000);
END.