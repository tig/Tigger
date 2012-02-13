


Program SetDefs; {Sample `install' program for use with the DEMO.PAS     }
                 {program included in this archive.                      }
                 {This source code and software is released to the public}
                 {domain for non-commercial users, 1988 John Majkrzak.   }
                 {All Rights Reserved.  Request permission for commercial}
                 {use or reprint.                                        }
                 {  John Majkrzak           CIS 76617,264                }
                 {  1880 Todd Dr.           Ph# (612)636-9761            }
                 {  Arden Hills, MN 55112                                }

    {$M 4000, 66000, 88000}

    Uses
      CRT, DOS,
      sDefs;     { << This TPU does much of the hard work required to
                  get a install program put together.  Just use it as
                  you see it used in this sample installation program.
                 }

    Type
      DefRecType = Record
        St: String[32];
        Yr,Mth,Day:   Word;
      end;

       {------------------------------------------------------------------}
       {The constant block in your INSTALL program must match EXACT to the}
       {one in the actual program.  Use only one block of declarations all}
       {under one CONST statement.  Declare every variable type. Don't say}
       {'MyNumber = 144;' say, `MyNumber: Word = 144;'.                   }
       {The idHeader is used to identify where your variables are located.}
       {It must be a string.  This is one of the few changes you may need }
       {to make to your source code when adapting this INSTALL routine for}
       {your own use.                                                     }

       {These variables must be set AFTER you read in the existing        }
       {variable block from your program for any changes you want to keep.}
       {Only the current value of `idHeader' must exactly match the       }
       {current value of idHeader in the object program.                  }
       {------------------------------------------------------------------}

    Const
      idHeader:   String[8]  = 'XmtSpot ';
      DefaultRec: DefRecType =
        (
          St:  ' ';             { << These values do not matter since they}
          Yr:  1986;            { get overwritten when you `ReadMemBlk().'}
          Mth: 10;
          Day: 10
        );
      BgColor:  Byte = 0;
      FgColor:  Byte = 0;
      LastByte: Byte = 0;  {This is only used to mark last byte of block, }
                           {see it in use with vbSize at start of program.}

    Const
      LoadPrgName   = 'DEMO.EXE'; {Add a path designation or change the}
      SavePrgName   = 'DEMO.EXE'; {name for easier testing.}

    Var
      J:              Word; {Global iteration variable.}
      vbSize:         Word; {Size of CONST block of default variables.}
      Ch:             Char;

  Procedure Beep;
    Begin Sound(880); Delay(110); NoSound;
  end;

  Procedure PurgeKeyboard;
    Var
      Ch: Char;
    Begin While Keypressed do Ch:= ReadKey;
  end;

  Function AnsYN: Char;
    Const
      YNset: Set of char = ['Y','N'];
    Var
     Ch: Char;
    Begin
    PurgeKeyBoard;
    Repeat
      Ch:= UpCase(ReadKey); AnsYN:= Ch;
      PurgeKeyboard;
      If Ch IN YNset then EXIT else beep;
    until FALSE;
  end;

  Procedure ErExit;
    Begin
    GoToXY(1,WhereY); ClrEOL;
    Write(ErSt); {The error report string is provided by sDefs.TPU}
    delay(4000);
    Halt;
  end;

(* -------------------------------------------------------------------------
    EXPLANATION OF NUMBERED COMMENTS
 {1} vbSize is set equal to size in bytes of the block of variables in need
     of adjustments.
 {2} The program gets loaded into heapspace.
 {3} Locate the block within your program by having it search for the
     identifier string.
 {4} Copy block of `vbSize' bytes from program just loaded.
 {5} Make any needed changes to your variables.
 {6} Write the changed settings back to where the block came from.
 {7} Save the program.  The file date of original program is intact.
 ------------------------------------------------------------------------ *)

  Begin
  WriteLn;
{1} vbSize:= Ofs(LastByte)-Ofs(idheader); {Segments will be equal.}
    Write('Loading program file.');
{2} If NOT PrgFileLoaded(LoadPrgName) then ErExit;
    GoToXY(1,WhereY); ClrEOL;
    Write('Searching for default record block');
{3} If NOT DefRecFound(@idHeader, SizeOf(idHeader) ) then ErExit;
{4} ReadMemBlk(@idheader, vbSize);
    GoToXY(1,WhereY); ClrEOL;
    With DefaultRec do
      WriteLn('Current file written: ',Mth,'-',Day,'-',Yr);
    Repeat
{5}   BgColor:= Green;
      FgColor:= Red;     {When DEMO is compiled these are Yellow-on-Blue.}
      TextColor(FgColor);
      TextBackground(BgColor);
      WriteLn;
      WriteLn('Current demonstration string: ',DefaultRec.St);
      WriteLn;
      Write('New string? ');
{5}   ReadLn(DefaultRec.St);
      WriteLn;
      WriteLn('Prepare disk drive to receive file.');
      Write  ('Press any key to continue.');
      PurgeKeyboard;
      Ch:= ReadKey;
      With DefaultRec do GetDate(Yr, Mth, Day, J);
{6}   WriteMemBlk(@idHeader, vbSize);
{7}   If NOT PrgFileSaved(SavePrgName) then ErExit;
      WriteLn;
      Write('Do another, Y/N?');
      Ch:= AnsYN;
    until Ch = 'N';
    WriteLn;
    WriteLn('Finished OK');
 END.