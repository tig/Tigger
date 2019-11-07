
Unit sDefs; { TPU for easy contruction of default installation programs.}

            { The program you plan to use SDEFS on must have a constant
              placed were it can serve as a locator for your default
              variable block.  The installation program you build with
              this TPU must have the exact same declarations as the target
              program.  See the SETDEFS and DEMO programs included in this
              archive.
            }
                 {This source code and software is released to the public}
                 {domain for non-commercial users, 1988 John Majkrzak    }
                 {All Rights Reserved.  Request permission for commercial}
                 {use or reprint.                                        }
                 {  John Majkrzak           CIS 76617,264                }
                 {  1880 Todd Dr.           Ph# (612)636-9761            }
                 {  Arden Hills, MN 55112                                }

  Interface
    Uses
      DOS;

    VAR
      ErSt:       String;     { << Contains error report if FALSE return }
                              { from a function seen below.}

      Ptr2DefRec: Pointer;    { << This pointer is set after the call to  }
                              { DefRecFound().  A record pointer could    }
                              { point to this, otherwise use the r/write  }
                              { memory block procedures as in SETDEFS.    }



  Function  PrgFileLoaded(fName: String): Boolean;
  Function DefRecFound(idPtr: Pointer; idSize: byte): Boolean;
  Procedure ReadMemBlk(ToBlk: Pointer; BlkSize: Word);
  Procedure WriteMemBlk(FromBlk: Pointer; BlkSize: Word);
  Function  PrgFileSaved(fName: String): Boolean;

  Implementation

    Const
      MaxVal        = 32766; {32766 value for block r/write and search.}
    Type
      PtrAryType    = Array[1..$FFFF] of byte; {Subscripts for block area.}
    Var
      J:              Word;       {Global iteration variable.}
      fRec:           SearchRec;
      FileSize:       LongInt;    {Program size.}
      cFilePos:       LongInt;
      AmtRemain:      LongInt;    {Amount of block left to process.}
      cFilePtr:       ^PtrAryType;
      bResult:        Integer;
      bSize:          Integer;    {Block size currently being processed.}
      bPtr:           Pointer;
      ioError:        Integer;
      PrgFtime:       LongInt;    {Original time gets rewritten to new file.}
      FileStart:      Pointer;
      WorkFile:       File;

  Function ExistR(Var InFile: File; Name: String): Boolean;
    Var
      Dummy: Word;
    Begin                 {$I-}
    Assign(InFile,Name);
    Reset (InFile, 1);
    If ioResult <> 0 then begin
        Close(InFile);    {$I+}
        ExistR:= False;
      end
    else ExistR:= True;
    Dummy:= ioResult;
  end;

  Function ExistW(Var WorkFile: File; Name: String): Boolean;
    Var
      Dummy: Word;
    Begin
    Assign (WorkFile,Name);  {$I-}
    Rewrite(WorkFile, 1);
    If ioResult <> 0 then begin
        Close(WorkFile);     {$I+}
        ExistW:= False;
      end
    else ExistW:= True;
    Dummy:= ioResult;
  end;

  Function CloseFile(VAR WorkFile: File; ioError: Integer): Boolean;
    Begin            {$I-}
    Close(WorkFile); {$I+}
    ioError:= ioResult;
    If ioError = 0 then CloseFile:= True else CloseFile:= False;
  end;

  Function AnError(VAR ioError: Integer): Boolean;
    Var
      Dummy: Word;
    Begin
    ioError:= ioResult;
    AnError:= False;
    If ioError = 0 then EXIT;
    AnError:= True;  {$I-}
    Close(WorkFile); {$I+}
    Dummy:= ioResult;
  end;

  Function bRead(VAR bFile:    File;
                      bPtr:    Pointer;
                      bSize:   Integer;
                 VAR  bResult: Integer): Boolean;
    Begin
    bRead:= False;                           {$I-}
    BlockRead(bFile, bPtr^, bSize, bResult); {$I+}
    If AnError(ioError) then EXIT;
    bRead:= True;
  end;

  Function bWrite(VAR bFile:    File;
                       bPtr:    Pointer;
                       bSize:   Integer;
                  VAR  bResult: Integer): Boolean;
    Begin
    bResult:= 0;
    bWrite:= False;                           {$I-}
    BlockWrite(bFile, bPtr^, bSize, bResult); {$I+}
    If AnError(ioError) OR (bResult <> bSize) then EXIT;
    bWrite:= True;
  end;

  Function PrgFileLoaded(fName: String): Boolean;
    Var
      DumTF: Boolean;
    Begin
    PrgFileLoaded:= False;
    FindFirst(fName, AnyFile, fRec);
    If DosError <> 0
      then begin ErSt:= 'Unable to locate file.'; EXIT; end;
    If NOT ExistR(WorkFile, fName)
      then begin ErSt:= 'Unable to open file for read.'; EXIT; end;
    GetFtime(WorkFile, PrgFTime);
    FileSize:= fRec.Size;
    bSize:= 4096;{Nice round figure}
    cFilePos:= 0;
    GetMem(FileStart, bSize);
    Inc(cFilePos,bSize);
    While cFilePos < FileSize do begin
      If MaxAvail < bSize then begin
        DumTF:= CloseFile(WorkFile, ioError);
        ErSt:= 'Not enough room on heap.';
        EXIT;
      end;
      GetMem(cFilePtr, bSize);
      Inc(cFilePos,bSize);
    end;
    bSize:= MaxVal;
    cFilePos:= 0;
    cFilePtr:= FileStart;
    bResult:= 0;
    Repeat
      bPtr:= @cFilePtr^[Succ(bResult)];
      cFilePtr:= bPtr;
      If NOT bRead(WorkFile, bPtr, bSize, bResult)
        then begin ErSt:= 'Unable to read file.'; EXIT; end;
      Inc(cFilePos, bResult);
    until (bResult <> bSize) OR (cFilePos >= FileSize);
    If cFilePos <> FileSize
      then begin ErSt:= 'Error, incorrect file read.'; EXIT; end;
    If NOT CloseFile(WorkFile, ioError)
      then begin ErSt:= 'Unable to close file after read.'; EXIT; end;
    PrgFileLoaded:= True;
  end;

  Function PrgFileSaved(fName: String): Boolean;

    Begin
    PrgFileSaved:= False;
    If NOT ExistW(WorkFile, fName)
      then begin ErSt:= 'Unable to open file for save.'; EXIT; end;
    cFilePos:= 0;
    cFilePtr:= FileStart;
    bResult:= 0;
    Repeat
      AmtRemain:= FileSize - cFilePos;
      If AmtRemain > MaxVal then bSize:= MaxVal else bSize:= AmtRemain;
      bPtr:= @cFilePtr^[Succ(bResult)];
      cFilePtr:= bPtr;
      If NOT bWrite(WorkFile, bPtr, bSize, bResult)
        then begin ErSt:= 'Unable to write file.'; EXIT; end;
      Inc(cFilePos,bResult);
    until (cFilePos >= FileSize);
    If cFilePos <> FileSize
      then begin ErSt:= 'Error, incorrect file save.'; EXIT; end;
    SetFtime(WorkFile, PrgFtime);
    If NOT CloseFile(WorkFile, ioError)
      then begin ErSt:= 'Error on close after write.'; EXIT; end;
    PrgFileSaved:= True;
  end;

  (* -----------------------------------------------------------------------
    FUNCTION BlockCompare(): Boolean, true if both blocks same.
      PARAMETERS
        Blk1, Blk2:  Pointers to start of blocks we are comparing.
        BlkSize:     Size of blocks being compared.
      CALLS
        NONE.
  ----------------------------------------------------------------------- *)
  Function BlockCompare(Blk1, Blk2: Pointer; BlkSize: Word): Boolean;
    Type
      Ary = Array[1..4096] of byte; {4096 is an arbitrary maximum.}
    Var
      J: Word;
      P1, P2: ^Ary;
    Begin
    P1:= Blk1; {Point the dynamic arrays to the blocks to be compared.}
    P2:= Blk2; {Subscripts sure are handy.}
    BlockCompare:= False;
    For J:= 1 to BlkSize do If P1^[J] <> P2^[J] then EXIT;
    BlockCompare:= True;
  end;

  (* -----------------------------------------------------------------------
    FUNCTION BlkFound(): Boolean, true if found what we were looking for.
      Searches for a block within a block.
      PARAMETERS
        sBlk:  Start of block we are looking for.
        sSize: Size of search block area.
        bBlk:  Start of block we are looking in.
        bSize: Size of block we are looking in.
        InPtr: Returns pointer to what we want.
      CALLS
        BlockCompare;
  ----------------------------------------------------------------------- *)

  Function BlkFound(    sBlk:  Pointer; sSize: Word;
                        bBlk:  Pointer; bSize: Word;
                 VAR InPtr: Pointer): Boolean;
    Type
      PtrAryType    = Array[$1..$FFFF] of byte;
    Var
      J: Word;
      PtrAry: ^PtrAryType;
    Begin
    J:= 0;
    PtrAry:= bBlk; {Search area gets subscripts.}
    BlkFound:= True;
    While J < bSize do begin
      Inc(J);
      InPtr:= @PtrAry^[J];
      If BlockCompare(sBlk, InPtr, sSize) then EXIT;
    end;
    BlkFound:= False;
  end;

  (* -----------------------------------------------------------------------
    FUNCTION DefRecFound(): Boolean, true if found what we were looking for.
      Searches for a block within a block.
      PARAMETERS
        idPtr: The identification pointer.  You might use `@MyString'.
        idSize: Use SizeOf() to satisfy this parameter whenever possible.
      CALLS
        BlockFound;
  ----------------------------------------------------------------------- *)

  Function DefRecFound(idPtr: Pointer; idSize: byte): Boolean;
    Var
      AmtRemain: LongInt;
    Begin
    DefRecFound:= True;
    cFilePos:= 0;
    cFilePtr:= FileStart;
    Repeat
      AmtRemain:= FileSize - cFilePos;
      If AmtRemain < MaxVal then bSize:= AmtRemain else bSize:= MaxVal;
      If BlkFound(idPtr, idSize,
                  cFilePtr, bSize,
                  Ptr2DefRec) then EXIT;
      bPtr:= @cFilePtr^[Succ(bSize)];
      cFilePtr:= bPtr;
    Until AmtRemain < MaxVal;
    ErSt:= 'Default record not found.';
    DefRecFound:= False;
  end;

  {$F+}
  Procedure ReadMemBlk(ToBlk: Pointer; BlkSize: Word);
    Begin
    Move(Ptr2DefRec^, ToBlk^, BlkSize);
  end;

  Procedure WriteMemBlk(FromBlk: Pointer; BlkSize: Word);
    Begin
    Move(FromBlk^, Ptr2DefRec^, BlkSize);
  end;

END.