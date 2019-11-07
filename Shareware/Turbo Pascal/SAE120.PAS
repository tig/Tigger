{
                          ‰AE DataBase Program
                 Copyright (C) Charles E. Kindel, 1989
                          KindlCo Consulting
                          All Rights Reserved

}
Program SAE;


  Uses Calendar,
       TpString,
       TpDos,
       TpCrt,
       TpEdit,
       TpWindow,
       printer;


  Const
        Version   = '1.20';
        VersionDate = 'May 2, 1989' ;
        FileName  = '‰AE.DAT';
        kQuit      = 16;
        kNew       = 49;
        kOption    = 24;
        kEdit      = 18;
        kEnter     = 28;
        kDel       = 32;
        kESC       = 1;
        kUp        = 72;
        kDown      = 80;
        kRight     = 77;
        kLeft      = 75;
        kHelp      = 59;
        kFind      = 33;
        kPrt       = 25;
        kSave      = 31;
        kScan      = 38 ;

        cESC       = #27 ;
        cFormFeed  = ^L;
        cBold      = cESC+'G' ;
        cNorm      = cESC+'H' ;

        NameLen   = 20;
        PinLen    = 6;
        PledgeLen = 11;
        DateLen   = 11;
        StreetLen = 50;
        CityLen   = 25;
        StateLen  = 2;
        ZipLen    = 5;
        PhoneLen  = 13;
        MajorLen  = 30;
        SSNlen    = 11;
        UnitsLen  = 4;
        StatusLen = 17;
        GPALen    = 6;
        InvalidClass = $FFFF;
        LastPrompt     = 'Last Name: ';
        FirstPrompt    = 'First Name: ';
        MIPrompt       = 'Middle Initial: ';
        PinPrompt      = 'Pin Number: ';
        PledgePrompt   = 'Pledge Class: ';
        SSNPrompt      = 'Social Security Number: ';
        BirthPrompt    = 'Birth Date: ';
        HomePrompt     = 'Home Address';
        SchoolPrompt   = 'School Address';
        StreetPrompt   = 'Street: ';
        CityPrompt     = 'City: ';
        StatePrompt    = 'State: ';
        ZipPrompt      = 'Zip: ';
        PhonePrompt    = 'Phone: ';
        ParentPrompt   = 'Parents Names: ';
        MajorPrompt    = 'Major: ';
        CumGPAPrompt   = 'Cumulative GPA: ';
        PrevGPAPrompt  = 'GPA from previous semester: ';
        StatusPrompt   = 'House Status: ';

        PrtMenuNum     = 2 ;
        PrtMenu       : array [1..PrtMenuNum] of string[20] =
                        ('Phone List', 'Full List') ;
        PageLen        = 56 ;
        NumCmds        = 10 ;
        Commands       : array [1..NumCmds] of string[10] =
                       (#24,#25, 'Edit','Delete','New','Find','Look',
                       'Print','Save','Quit') ;

        LastX    = 3;  LastY    = 3;
        FirstX   = 3;  FirstY   = 4;
        MIX      = 3;  MIY      = 5;
        SSNX     = 3;  SSNY     = 6;
        DateX    = 42; DateY    = 3;
        PinX     = 42; PinY     = 4;
        PledgeX  = 42; PledgeY  = 5;

        MajorX   = 3;  MajorY   = 8;
        CumX     = 3;  CumY     = 9;
        PrevX    = 40;  PrevY    = 9;
        StatusX  = 3;  StatusY  = 10;

        HomeX    = 34;  HomeY    = 13;
        SchoolX  = 33;  SchoolY  = 19;
        StreetX  = 10;
        CityX    = 10;
        StateX   = 43;
        ZipX     = 54;
        PhoneX   = 10;
        ParentX  = 10; ParentY  = 17;

        MainBG = BlaCK;
        MainFG = Yellow;
        PromptFG = Black;
        PromptBG = LightGray;
        DataFG = LightMagenta;
        DataBG = MainBG;
        CommandFG = White;
        CommandBG = Blue;

  Type
       nametype   = string [nameLen];
       streettype = string[StreetLen];
       Citytype   = string[CityLen];
       Statetype  = string[StateLen];
       Ziptype    = string[ZipLen];
       Phonetype  = string[PhoneLen];
       Majortype  = string[majorlen];
       SSNtype    = string[SSNlen];

       statustype = (NoStatus, InGoodStanding, InBadStanding, InActive, Pledge, Alumni,
                     GradSenior, Social);
       pledgeclasstype = word;
       addresstype = record
                       Street : Streettype;
                       City   : citytype;
                       State  : Statetype;
                       Zip    : Ziptype;
                       Phone  : Phonetype;
                     end;
       statusarray = array [StatusType] of string [StatusLen];
       memberptr = ^membertype ;
       membertype = record
                      Last  : nametype;
                      First : nametype;
                      MI    : char;
                      BirthDate : DateNum;      { type defined in Calandar unit }
                      PinNumber : LongInt;
                      PledgeClass : pledgeclasstype;
                      SSN : SSNtype;
                      HomeAddr : addresstype;
                      ParentName : nametype;
                      SchoolAddr : addresstype;
                      Major : majortype;
                      CumGPA : real;
                      PrevGPA : real;
                      HouseStatus : statustype;
                      pFlink : memberptr ;
                      pBlink : memberptr
                    end;


  Const
       StatusString : StatusArray = ('No Status', 'In Good Standing', 'In Bad Standing', 'Inactive',
                                     'Pledge', 'Alumni', 'Graduating Senior', 'Social');

  Var
       pHead, pCurrent : memberptr ;
       NumMembers : word;
       MemberFile : file of membertype;
       Key : word;
       Command : char absolute Key;
       Escaped, Modified :boolean;
       Line43  : boolean;
       Mem : Membertype;
       FindWindow, OptionWindow,ScanWindow,PrintWindow  : WindowPtr;





  Procedure DeleteMember (var pCur : memberptr);
    var pSave : memberptr ;
    begin
      pSave := pCur^.pBlink ;
      pSave^.pFlink := pCur^.pFlink ;
      pSave := pCur^.pFlink ;
      pSave^.pBlink := pCur^.pBlink ;
      Dispose (pCur) ;
    end;


  Procedure InsertRec (var pNewMember, pWhere : memberptr) ;

    var
      pPrev : memberptr ;
    begin
      pPrev := pWhere^.pBlink ;
      pNewMember^.pFlink := pWhere ;
      pNewMember^.pBlink := pPrev ;
      pPrev^.pFlink := pNewMember ;
      pWhere^.pBlink := pNewMember ;
    end ;



  Procedure InsertMember (mem : membertype);
    var pNewMember, pCur : memberptr ;

    begin
      pCur := pHead^.pFlink ;
      while ((pCur <> pHead) and
             (StUpCase (mem.last) > StUpCase (pCur^.last))) do
         pCur := pCur^.pFlink ;

      New (pNewMember) ;

      pNewMember^ := mem ;
      pCurrent := pNewMember ;
      InsertRec (pNewMember, pCur);
    end;

  Procedure ReadFile;

    var ch :char;
        mem : membertype;

    begin
      GotoXY (1,25) ;
      TextColor (White) ; TextBackground (Blue) ; ClrEol ;
      If ExistFile (FileName) then
        begin
          Assign (MemberFile, FileName);
          Reset (MemberFile);
          write ('reading...',FileName);
          while not Eof (MemberFile) do
            begin
              read (MemberFile, Mem);
              InsertMember (mem);
            end;
          Close (MemberFile);
        end
      else
        begin
          Write (FileName, ' not found, create? ');
          ch := upcase (readkey);
          if ch = 'Y' then
            begin
              GotoXY (1,25) ; ClrEOl ;
              Write ('Creating ',FileName,'...');
              Assign (MemberFile, FileName);
              ReWrite (MemberFile);
              Close (MemberFile);
            end
          else
            begin
              Write ('Exiting!');
              halt;
            end;
        end;
      Modified := FALSE ;
    end;


  Procedure SaveFile;
    var i : word;
        pCur : memberptr ;

    begin
      TextBackGround (Blue) ;TextColor (White) ;
      Assign (MemberFile, FileName);
      Rewrite (MemberFile);
      GotoXY (1,25) ;
      ClrEol ;
      Write (' Writing file...');
      pCur := pHead^.pFlink ;
      while pCur <> pHead do
        begin
          write (MemberFile, pCur^) ;
          pCur := pCur^.pFlink ;
        end ;
      Close (memberFile);
      Modified := FALSE ;
    end;

  Procedure Options;
    var c : word;
    begin
      if not DisplayWindow (OptionWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      ClrScr ;
      FastWriteWindow (Center('Sorry, no help available!',68), 1, 1, Yellow) ;
      FastWriteWindow (Center('Memory Available = '+Long2Str (memAvail),68), 5,1, LightMagenta);
      FastWriteWindow (Center('Version '+Version+', '+VersionDate,68), 8,1, LightMagenta);
      FastWriteWindow (Center('Press any key ',68), 10,1, Yellow);
      c := readkeyWord;
      OptionWindow := EraseTopWindow;
    end;


  Function Status2Str (S : statustype) : string;
    begin
      Status2Str := StatusString [S];
    end;



  Function PledgeClassStr (N : pledgeclasstype) : string;
  { returns a string (Spring 1900) when presented N=1 }
  { returns a blank string if N =0                    }

    var Year : integer;
    begin
      if (N = InvalidClass) then
          PledgeClassStr := '*Invalid*'
      else
        if (N > 0) then
          begin
            Year := round (N/2) - 1 + 1900;
            if Odd (N) then
              PledgeClassStr := 'Spring ' + Long2str (Year)
            else
              PledgeClassStr := 'Fall ' + Long2str (Year);
          end
        else
          PledgeClassStr := '';
    end;


  Function PledgeClassNum (S : string) : pledgeclasstype;

    var year : real;
        code : integer;
        err : boolean;

    begin
      code := 0;
      Err := false;
      if (upcase (S[1]) = 'S') then
        begin
          val (copy (S, pos (' ', S)+1, Length (S)), year, code);
          if year >= 1900 then
            year := (year - 1900);
          PledgeClassNum :=  round ((year*2)+1);
        end
      else
        if (upcase (S[1]) = 'F') then
          begin
            val (copy (S, pos (' ', S)+1, Length (S)), year, code);
            if year >= 1900 then
              year := (year - 1900);
            PledgeClassNum := round (year*2)+2;
          end
        else
          Err := True;
      if (Err or (code <> 0)) then
        PledgeClassNum := InvalidClass ;
    end;

  Procedure HorizLine (Y, Attr : byte);
    begin
      FastWrite (chr(199)+CharStr(chr(196),78)+chr(182), Y, 1, Attr);
    end;


  Procedure ClearMember (var mem : membertype);
    begin
      with mem do
        begin
          Last  := '';
          First := '';
          MI := ' ';
          BirthDate := 0;
          PinNumber := 0;
          PledgeClass := 0;
          SSN := '';
          HomeAddr.street := '';
          HomeAddr.City   := '';
          HomeAddr.State  := '';
          HomeAddr.Zip    := '';
          HomeAddr.Phone  := '';
          ParentName :='';
          SchoolAddr.street := '';
          SchoolAddr.City   := '';
          SchoolAddr.State  := '';
          SchoolAddr.Zip    := '';
          SchoolAddr.Phone  := '';
          Major := '';
          CumGPA := 0;
          PrevGPA := 0;
          HouseStatus := NoStatus;
          pFlink  := Nil ;
          pBlink := Nil ;
        end;
    end;



      Function GetName (X,Y : byte; Old : nametype) : nametype;
        var temp : string;
        begin
          temp := old;
          ReadString ('', Y, X, NameLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16, Escaped, temp);
          FastWrite (CharStr(' ',NameLen), Y,X,DataFG+(DataBG*16));
          FastWrite (temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetName := temp;
        end;

      Function GetMI (X,Y : byte; Old : char) : char;
        var temp : string;
        begin
          temp := old;
          Readstring ('', Y, X, 1, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (Temp[1], Y,X,DataFG+(DataBG*16));
          if temp[1] <> old  then
            Modified := TRUE ;
          GetMI:= temp[1];
        end;


      Function GetBirthDate (X,Y : byte; Old : DateNum) : DateNum;
        var Datestring, temp : string;
        begin
          DateString := LotusDate (Old);
          temp := DateString ;
          ReadString ('', Y, X, DateLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, DateString);
          if temp <> DateString then
             Modified := TRUE ;
          Old := FromLotusDate (DateString);
          if (Old = InvalidDate) then
            DateString := 'n/a' ;
          FastWrite (CharStr(' ',DateLen), Y,X,DataFG+(DataBG*16));
          FastWrite (LotusDate(Old),Y,X,DataFG+(DataBG*16));
          GetBirthDate := Old;
        end;

      Function GetPinNumber (X,Y : byte; Old : longint) : longint;
        var temp : longint ;
        begin
          temp := old ;
          FastWrite (CharStr(' ',PinLen+4), Y,X,DataFG+(DataBG*16));
          ReadLongInt ('', Y,X, PinLen, Cyan, PromptFG+PromptBG*16, 0, 500000, Escaped, Old);
          FastWrite (CharStr(' ',PinLen), Y,X,DataFG+(DataBG*16));
          if Old < 18000 then
            FastWrite ('*Invalid*', Y,X,DataFG+(DataBG*16))
          else
            FastWrite (Long2Str (Old),Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetPinNumber := Old;
        end;

      Function GetSSN (X,Y : byte; Old : SSNtype) : SSNtype;
        var temp : string;
        begin
          temp := old;
          ReadString ('', Y,X, SSNLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16, Escaped, temp);
          FastWrite (CharStr (' ', SSNLen), Y,X, DataFG+(DataBG*16));
          FastWrite (Temp, Y,X, DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetSSN := temp;
        end;

      Function GetPledgeClass (X,Y : byte; Old : PledgeClasstype) : pledgeclasstype;
        var temp, temp1 : string;
        begin
          temp := PledgeClassStr (Old);
          temp1 := temp ;
          ReadString ('', Y, X, PledgeLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          Old := PledgeClassNum (temp);
          FastWrite (CharStr(' ',PledgeLen), Y,X,DataFG+(DataBG*16));
          FastWrite (PledgeClassStr (Old),Y,X,DataFG+(DataBG*16));
          if temp <> temp1 then
            Modified := TRUE ;
          GetPledgeClass := Old;
        end;

      Function GetStreet (X,Y : byte; Old : streettype) : streettype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('', Y,X, StreetLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',StreetLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetStreet := temp;
        end;

      Function GetCity (X,Y : byte; Old : citytype) :citytype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('',Y,X, CityLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped,temp);
          FastWrite (CharStr(' ',CityLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetCity := temp;
        end;

      Function GetState (X,Y : byte; Old : statetype) : statetype;
        var temp : string;
        begin
          temp := Old;
          ForceUpper := True;
          ReadString ('', Y, X, StateLen, Cyan, PromptFG+PromptBG*16,PromptFG+PromptBG*16, Escaped, temp);
          ForceUpper := False;
          FastWrite (CharStr(' ',StateLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetState := temp;
        end;


      Function GetZip (X,Y : byte; old : ziptype) : ziptype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('', Y,X, ZipLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',ZipLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetZip := temp;
        end;

      Function GetPhone (X,Y : byte; old : phonetype) :phonetype;
        var temp : string;
        begin
          temp := old;
          ReadString ('',Y,X, PhoneLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',PhoneLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetPhone := temp;
        end;

      Function GetMajor (X,Y : byte; old : majortype) :majortype;
        var temp : string;
        begin
          temp := old;
          ReadString ('',Y,X, MajorLen, Cyan, PromptFG+PromptBG*16,  PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',MajorLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetMajor := temp;
        end;

      Function GetUnits (X,Y : byte; old : integer) : integer;
        var temp : integer ;
        begin
          temp := old ;
          ReadInteger ('', Y,X, UnitsLen, Cyan,PromptFG+PromptBG*16, 0, 1000, Escaped,Old);
          FastWrite (CharStr(' ',UnitsLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Long2Str (Old),Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetUnits := Old;
        end;

      Function Real2Str (R : real) : string;
        var ST : string;
        begin
          Str (R:GPAlen:GPALen-2,ST);
          Real2Str := ST;
        end;


      Function GetGPA (X,Y:byte; old : real) : real;
        var temp : real ;
        begin
          temp := old ;
          ReadReal ('', Y,X, GPALen, Cyan, PromptFG+PromptBG*16, 4, 0, 4, Escaped, old);
          FastWrite (CharStr(' ',GPALen), Y,X,DataFG+(DataBG*16));
          FastWrite (Real2Str (Old),Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetGPA := old;
        end;

      Function GetStatus (X,Y : byte; old : statustype) : statustype;
        var
            ch : word;
            temp : statustype ;
        begin
          temp := old ;
          repeat
            FastWrite (CharStr(' ',StatusLen), Y,X, PromptFG+PromptBG*16);
            FastWrite (Status2Str (Old), Y,X, PromptFG+PromptBG*16);
            GotoXY (X,Y);
            ch := readkeyword;
            if not ((char(Lo(ch)) in [^M, #27]) or ((char(lo(ch)) = #0) and (hi(ch) in [kUp,kDown]))) then
              if (old = Social) then
                Old := NoStatus
              else
                Inc (Old);
          until ((char(Lo(ch)) in [^M, cESC]) or ((char(lo(ch)) = #0) and (hi(ch) in [kUp,kDown])));
          if char(Lo(ch)) = #27 then
            Escaped := True;
          FastWrite (charstr(' ',StatusLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Status2Str (Old),Y,X,DataFG+(DataBG*16));
          if temp <> old then
            Modified := TRUE ;
          GetStatus := Old;
        end;


  Procedure DisplayPrompts;
    var Col : byte;
    begin
      TextColor (MainFG);
      TextBackGround (MainBG);
      Window (2,3,79,11);
      ClrScr;
      Window (2,13, 79,23);
      ClrScr;
      Window (1,1,80,25);
      if MapColors then
        col := LightGray
      else
        col := MainFG+(MainBG*16);
      FastWrite (LastPrompt, LastY, LastX,Col);
      FastWrite (FirstPrompt, FirstY, FirstX, Col);
      FastWrite (MIPrompt, MIY, MIX, Col);
      FastWrite (SSNPrompt, SSNY, SSNX, Col);
      FastWrite (BirthPrompt, DateY, DateX, Col);
      FastWrite (PinPrompt, PinY, PinX, Col);
      FastWrite (PledgePrompt, PledgeY, PledgeX, Col);

      FastWrite (MajorPrompt, MajorY, MajorX, Col);
      FastWrite (CumGPAPrompt, CumY, CumX, Col);
      FastWrite (PrevGPAPrompt, PrevY, PrevX, Col);
      FastWrite (StatusPrompt, StatusY, StatusX, Col);

      FastWrite (HomePrompt, HomeY, HomeX, Col);
      FastWrite (StreetPrompt, HomeY+1, StreetX, Col);
      FastWrite (CityPrompt, HomeY+2, CityX, Col);
      FastWrite (StatePrompt, HomeY+2, StateX, Col);
      FastWrite (ZipPrompt, HomeY+2, ZipX, Col);
      FastWrite (PhonePrompt, HomeY+3, PhoneX, Col);
      FastWrite (ParentPrompt, ParentY, parentX, Col);

      FastWrite (SchoolPrompt, SchoolY, SchoolX, Col);
      FastWrite (StreetPrompt, SchoolY+1, StreetX, Col);
      FastWrite (CityPrompt, SchoolY+2, CityX, Col);
      FastWrite (StatePrompt, SchoolY+2, StateX, Col);
      FastWrite (ZipPrompt, SchoolY+2, ZipX, Col);
      FastWrite (PhonePrompt, SchoolY+3, PhoneX, Col);
    end;


  Procedure MainDisplay;

    begin
      TextColor (MainFG);
      TextBackGround (MainBG);
      ClrScr;
      FrameWindow (1,2,80,24, MainFG+(MainBG*16), LightGreen+(MainBG*16),
                   ' Main Screen - Press F1 for help ');
      HorizLine (12,MainFG+(MainBG*16));
      DisplayPrompts;
    end;


  Procedure DisplayMember (Mem : membertype);
    var Col : byte;
    begin
      col := DataFG+(DataBG*16);
      FastWrite (CharStr (' ', NameLen), lastY,LastX+Length(LastPrompt),Col);
      FastWrite (Mem.Last, LastY, LastX+Length(LastPrompt),Col);
      FastWrite (CharStr (' ',NameLen), FirstY, FirstX+Length(FirstPrompt), Col);
      FastWrite (Mem.First, FirstY, FirstX+Length(FirstPrompt), Col);
      FastWrite (' ', MIY, MIX+Length(MIPrompt), Col);
      FastWrite (Mem.MI, MIY, MIX+Length(MIPrompt), Col);
      FastWrite (CharStr (' ', SSNlen), SSNY, SSNX+Length (SSNPrompt), Col);
      FastWrite (Mem.SSN, SSNY, SSNX+Length (SSNPrompt), Col);
      FastWrite (CharStr (' ', DateLen), DateY, DateX+Length(BirthPrompt), Col);
      FastWrite (LotusDate (Mem.BirthDate), DateY, DateX+Length(BirthPrompt), Col);
      FastWrite (Charstr(' ',PinLen+4), PinY, PinX+Length(PinPrompt), Col);
      if mem.PinNumber < 18000 then
        FastWrite ('*Invalid*', PinY, PinX+Length(PinPrompt), Col)
      else
        FastWrite (Long2str(mem.PinNumber),PinY, PinX+Length(PinPrompt), Col);
      FastWrite (CharStr(' ',PledgeLen), PledgeY, PledgeX+Length(PledgePrompt), Col);
      FastWrite (PledgeClassStr(Mem.PledgeClass), PledgeY, PledgeX+Length(PledgePrompt), Col);

      FastWrite (CharStr (' ',MajorLen), MajorY, MajorX+Length(majorPrompt), Col);
      FastWrite (Mem.Major, MajorY, MajorX+Length(majorPrompt), Col);
      FastWrite (charStr (' ',GPALen), CumY, CumX+Length(CumGPAPrompt), Col);
      FastWrite (Real2Str (Mem.CumGPA), CumY, CumX+Length(CumGPAPrompt), Col);
      FastWrite (CharStr (' ',GPALen), PrevY, PrevX+Length(PrevGPAPrompt), Col);
      FastWrite (Real2Str (Mem.PrevGPA), PrevY, PrevX+Length(PrevGPAPrompt), Col);
      FastWrite (CharStr (' ',StatusLen), StatusY, StatusX+Length(StatusPrompt), Col);
      FastWrite (Status2Str (Mem.HouseStatus), StatusY, StatusX+Length(StatusPrompt), Col);

      FastWrite (CharStr (' ',streetLen), HomeY+1, StreetX+Length(StreetPrompt), Col);
      FastWrite (Mem.HomeAddr.street, HomeY+1, StreetX+Length(StreetPrompt), Col);
      FastWrite (CharStr (' ',CityLen), HomeY+2, CityX+Length(CityPrompt), Col);
      FastWrite (Mem.HomeAddr.City, HomeY+2, CityX+Length(CityPrompt), Col);
      FastWrite (CharStr (' ',stateLen), HomeY+2, StateX+Length(StatePrompt), Col);
      FastWrite (Mem.homeAddr.state, HomeY+2, StateX+Length(StatePrompt), Col);
      FastWrite (CharStr (' ',ziplen), HomeY+2, ZipX+length(zipPrompt), Col);
      FastWrite (Mem.homeAddr.zip, HomeY+2, ZipX+length(zipPrompt), Col);
      FastWrite (CharStr (' ',PhoneLen), HomeY+3, PhoneX+Length(PhonePrompt), Col);
      FastWrite (Mem.HomeAddr.Phone, HomeY+3, PhoneX+Length(PhonePrompt), Col);
      FastWrite (CharStr (' ',NameLen),ParentY, ParentX+Length(ParentPrompt),Col);
      FastWrite (Mem.ParentName,ParentY, ParentX+Length(ParentPrompt),Col);

      FastWrite (CharStr (' ',streetLen), SchoolY+1, StreetX+Length(StreetPrompt), Col);
      FastWrite (Mem.SchoolAddr.street, SchoolY+1, StreetX+Length(StreetPrompt), Col);
      FastWrite (CharStr (' ',CityLen), SchoolY+2, CityX+Length(CityPrompt), Col);
      FastWrite (Mem.SchoolAddr.City, SchoolY+2, CityX+Length(CityPrompt), Col);
      FastWrite (CharStr (' ',stateLen), SchoolY+2, StateX+Length(StatePrompt), Col);
      FastWrite (Mem.SchoolAddr.state, SchoolY+2, StateX+Length(StatePrompt), Col);
      FastWrite (CharStr (' ',ziplen), SchoolY+2, ZipX+length(zipPrompt), Col);
      FastWrite (Mem.SchoolAddr.zip, SchoolY+2, ZipX+length(zipPrompt), Col);
      FastWrite (CharStr (' ',PhoneLen), SchoolY+3, PhoneX+Length(PhonePrompt), Col);
      FastWrite (Mem.SchoolAddr.Phone, SchoolY+3, PhoneX+Length(PhonePrompt), Col);
    end;

  Procedure GetMember (var Mem : membertype);
    var Field,i : byte;
        line : string ;
    begin
      TextColor (CommandFG);
      TextBackGround (COmmandBG);
      GotoXY (1,25);
      ClrEol;
      write (' '+#24,', ',#25,' to change fields, [ENTER] to accept field , or [ESC] to terminate Edit');

      Field := 1;
      repeat
        Case Field of
          1 : mem.last := GetName (LastX+length(LastPrompt), LastY, mem.last);
          2 : mem.first := GetName (firstX+length(FirstPrompt), FirstY, mem.First);
          3 : mem.MI := GetMI (MIX+length(MIPrompt), MIY, mem.MI);
          4 : mem.SSN := GetSSN (SSNX+Length(SSNPrompt), SSNY, mem.SSN);
          5 : mem.BirthDate := GetBirthDate (DateX+Length(BirthPrompt), DateY, mem.BirthDate);
          6 : mem.PinNumber := GetPinNumber (PinX+length(PinPrompt), PinY, mem.PinNumber);
          7 : mem.PledgeClass := GetPledgeClass (PledgeX+length(PledgePrompt), PledgeY, mem.PledgeClass);

          8 : mem.Major := GetMajor (MajorX+Length(MajorPrompt), MajorY, mem.Major);
          9 : mem.CumGPA := GetGPA (CumX+Length(CumGPAPrompt), CumY, mem.CumGPA);
          10: mem.PrevGPA := GetGPA (PrevX+Length(PrevGPAPrompt), PrevY, Mem.PrevGPA);
          11: mem.HouseStatus := GetStatus (StatusX+Length(StatusPrompt), StatusY, mem.Housestatus);

          12: mem.HomeAddr.street := GetStreet (StreetX+length(StreetPrompt), HomeY+1, mem.HomeAddr.Street);
          13: mem.HomeAddr.City := GetCity (CityX+length(CityPrompt), HomeY+2, mem.HomeAddr.City);
          14: mem.HomeAddr.state := GetState (StateX+length(StatePrompt), HomeY+2, mem.HomeAddr.State);
          15: mem.HomeAddr.zip := Getzip (ZipX+length(ZipPrompt), HomeY+2, mem.HomeAddr.Zip);
          16: mem.HomeAddr.Phone := GetPhone (PhoneX+length(PhonePrompt), HomeY+3, mem.HomeAddr.Phone);
          17: mem.ParentName := GetName (ParentX+length(parentPrompt), ParentY, mem.Parentname);

          18:mem.SchoolAddr.street := GetStreet (StreetX+length(StreetPrompt), SchoolY+1, mem.SchoolAddr.Street);
          19:mem.SchoolAddr.City := GetCity (CityX+length(CityPrompt), SchoolY+2, mem.SchoolAddr.City);
          20:mem.SchoolAddr.state := GetState (StateX+length(StatePrompt), SchoolY+2, mem.SchoolAddr.State);
          21:mem.SchoolAddr.zip := Getzip (ZipX+length(ZipPrompt), SchoolY+2, mem.SchoolAddr.Zip);
          22:mem.SchoolAddr.Phone := GetPhone (PhoneX+length(PhonePrompt), SchoolY+3, mem.SchoolAddr.Phone);
        end;
        Case Char (Lo(RSchWord)) of
          #0 : case (Hi(RSChWord)) of
                kUp   : if Field = 1 then
                         Field := 22
                       else
                         Field := Pred(Field);
                kDown : if Field = 22 then
                         Field := 1
                       else
                         Inc (Field);
               end;
          ^M : if Field = 22 then
                 Field := 1
               else
                 Inc(Field);
        end;
      If Modified then
        FastWrite (' Database has been modified ', 24, 25, LightRed) ;
      until (Escaped);
      line := '' ;
      for i := 1 to 15 do
         line:= line + chr(205) ;
      FastWrite (line, 2,3, Yellow) ;
    end;


  Procedure NewMember;
    var i :integer;
        Mem :membertype;
        line : string ;
    begin
      ClearMember (Mem);
      DisplayPrompts;
      Fastwrite ('New Member',2,3, LightBlue+16*MainBG);
      GEtMember (Mem);
      InsertMember (Mem);
      line := '' ;
      for i := 1 to 11 do
         line:= line + chr(205) ;
      FastWrite (line, 2,3, Yellow) ;
    end;

  Procedure FindMember;
    var c : word;
        name : nametype;
        pCur : memberptr ;

    begin
      if not DisplayWindow (FindWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      ClrScr;
      name := '';
      fastwritewindow (Center('Enter LAST name to search for: ',38), 1,1, LightBlue);
      name := GetName (29, 17, name);
      pCur := pHead^.pFlink ;
      while ((pCur <> pHead) and (StUpCase (Name) <> StUpCase (pCur^.last))) do
        pCur := pCur^.pFlink ;
      if (StUpCase(pCur^.last) = StUpcase(Name)) then
        pCurrent := pCur
      else
        begin
          FastWriteWindow (Center(name+' Not Found!',38), 2,1,LightMagenta+Blink);
          FastWriteWindow (Center('Press any key',38), 3,1, Yellow);
          c := readkeyWord;
        end ;
      FindWindow := EraseTopWindow;
    end;



  Procedure Scan;
    var
        i : integer;
        pCur,pBot,pTop : memberptr ;
        temp : string ;
        c: word;

    begin
      if not DisplayWindow (ScanWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      ClrScr;
      HiddenCursor ;
      if pHead^.pFlink <> pHead then
        begin
          TextColor (CommandFG);
          TextBackGround (COmmandBG);
          GotoXY (1, 25) ;
          ClrEol ;
          FastWrite (' '+#24+', '+#25+' to move, [ENTER] or [ESC] to select.', 25,1, CommandFG+16*CommandBG);
          pTop := pCurrent ;
          for i := 1 to 6 do
            begin
              pTop := pTop^.pBlink ;
              if pTop = pHead then
                pTop := pHead^.pBlink ;
            end ;
          pBot := pCurrent ;
          for i := 1 to 6 do
            begin
              pBot := pBot^.pFlink ;
              if pBot = pHead then
                pBot := pHead^.pFlink ;
            end ;
          pCur := pTop ;
          i := 1 ;
          repeat
            if (pCur <> pHead) then
              begin
                temp := pCur^.last+', '+pCur^.first ;
                FastWriteWindow (pad (temp, 43), i,1, Black+16*LightGray);
                inc (i) ;
              end ;
            pCur := pCur^.pFlink ;
            if ((pCur = pHead) and (pCur <> pBot^.pFlink)) then
              pCur := pHead^.pFlink ;
          until (i = 14) {pCur = pBot^.pFlink)} ;
          repeat
            temp := pCurrent^.last+', '+pCurrent^.first ;
            FastWriteWIndow (pad (temp, 43), 7,1, CommandFG+16*CommandBG) ;
            c := readkeyword ;
            Case Char (Lo(c)) of
              #0 : case (Hi(c)) of
                    kUp   : begin
                              temp := pCurrent^.last+', '+pCurrent^.first ;
                              FastWriteWIndow (pad (temp, 43), 7,1, Black+16*LightGray) ;
                              ScrollWindowDown (31, 10, 73, 22, 1) ;
                              pTop := pTop^.pBlink ;
                              if pTop = pHead then
                                PTop := pHead^.pBlink ;
                              pBot := pBot^.pBlink ;
                              if pBot = pHead then
                                pBot := pHead^.pBlink ;
                              temp := pTop^.last+', '+pTop^.first ;
                              FastWriteWindow (pad (temp, 43), 1,1, Black+16*LightGray) ;
                              pCurrent := pCurrent^.pBlink ;
                              if pCurrent = pHead then
                                pCurrent := pHead^.pBlink ;
                            end ;
                    kDown : begin
                              temp := pCurrent^.last+', '+pCurrent^.first ;
                              FastWriteWIndow (pad (temp, 43), 7,1, Black+16*LightGray) ;
                              ScrollWindowUp (31, 10, 73, 22, 1) ;
                              pTop := pTop^.pFlink ;
                              if pTop = pHead then
                                PTop := pHead^.pFlink ;
                              pBot := pBot^.pFlink ;
                              if pBot = pHead then
                                pBot := pHead^.pFlink ;
                              temp := pBot^.last+', '+pBot^.first ;
                              FastWriteWindow (pad (temp, 43), 13,1, Black+16*LightGray) ;
                              pCurrent := pCurrent^.pFlink ;
                              if pCurrent = pHead then
                                pCurrent := pHead^.pFlink ;
                            end ;

                   end ;
            end;
          until (Char(Lo(c)) in [cESC,^M]);
        end
      else
        begin
          temp := ' ' ;
          for i := 1 to 13 do
            FastWriteWindow (pad (temp, 43), i,1, Black+16*LightGray) ;
          temp := 'No Entries in DataBase' ;
          FastWriteWIndow (pad (temp, 43), 7,1, White+16*LightBlue) ;
          c := readkeyword ;
        end ;

      ScanWindow := EraseTopWindow;
      NormalCursor ;
    end;



  Procedure Print ;
    var LineNum, PageNum, i:word;
        c:char ;
        Header, Footer : string ;


    Procedure PrintLine (var line:string) ;
      var temp : string ;
      begin
        inc(LineNum) ;
        if LineNum = PageLen then
          begin
            Writeln (lst) ;
            writeln (lst) ;
            writeln (lst, cBold) ;
            writeln (lst, ' ':70,'Page ',long2Str(PageNum),cNorm) ;
            Inc(PageNum) ;
            write (Lst, cFormFeed) ;
            writeln (lst, cBold) ;
            writeln (lst, Header, cNorm) ;
            writeln (lst) ;
            writeln (lst) ;
            writeln (lst) ;

            LineNum := 1 ;
          end ;
          if line = '' then
            writeln (lst)
          else
            if length (line) > 66 then
              begin
                temp := copy(line,1,65);
                PrintLine (temp) ;
                writeln (lst, ' ':9,copy(line,65,length(line))) ;
                temp := '' ;
              end
            else
              writeln (lst,' ':6,line) ;
      end ;


    Procedure PhoneList ;
      var temp : string ;
          pCur : memberptr ;
      begin
        temp := '' ;
        ReadString ('Enter title for Phone list (i.e. Fall 1990):',
                    17, 8, 20, Cyan, PromptFG+PromptBG*16,
                    PromptFG+PromptBG*16,Escaped, temp);
        FastWriteWindow (Center('Printing...',68),10,1,LightMagenta+Blink) ;
        Header := Center ('Arizona Alpha - Sigma Alpha Epsilon - Phone List - '+temp, 79) ;
        LineNum := 1 ;
        PageNum := 1 ;
        writeln (lst) ;
        writeln (lst, cBold, header,cNorm) ;
        writeln (lst) ;
        writeln (lst) ;
        writeln (lst) ;
        pCur := pHead^.pFlink ;
        while (pCur <> pHead) do
          begin
            temp := pCur^.last + ', '+ pCur^.first ;
            while (Length (temp) < 51) do
              temp := temp + '.' ;
            temp := StUpCase (temp + pCur^.HomeAddr.phone) ;
            PrintLine (temp) ;
            temp := StUpCase ('  '+pCur^.HomeAddr.Street+'  '+pCur^.HomeAddr.City+'  '+
                    pCur^.HomeAddr.State+'  '+pCur^.HomeAddr.Zip) ;
            PrintLine (temp) ;
            temp := '' ;
            pCur := pCur^.pFlink ;
          end ;
        temp := '' ;
        While (LineNum < PageLen+4) do
          begin
            inc(LineNum) ;
            Writeln (lst, '') ;
          end ;
        write (lst, cBold, ' ':70,'Page ',long2Str(PageNum),cNorm, cFormFeed) ;
      end ;

    Procedure FullList ;
      begin
      end ;

    begin
      if not DisplayWindow (PrintWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      repeat
        ClrScr;
        FastWriteWindow (Center('Press number of report to print, [ESC] to exit',68),3,1,LightRed) ;
        for i := 1 to PrtMenuNum do
          FastWriteWindow (center(long2str (i)+' - '+PrtMenu[i], 68), i+4, 1, Yellow) ;
        ReadCharacter ('Select: ',13,35,LightCyan,['1','2',cESC], c) ;
        case c of
          '1' : PhoneList ;
          '2' : FullList ;
        end ;
      until c = cESC ;

      PrintWindow := EraseTopWindow;
    end ;




  Procedure GetCommand;
    var i: byte ;

    begin
      TextBackGround (CommandBG);
      TextColor (CommandFG);
      GotoXY (1,25);
      ClrEol;
      Write ('Command (') ;
      for i := 1 to NumCmds-1 do
        begin
          TextColor (LightMagenta) ;
          write (copy (Commands[i],1,1)) ;
          TextColor (CommandFG) ;
          write (copy (Commands[i],2,Length(Commands[i])), ',') ;
        end ;
      TextColor (LightMagenta) ;
      write (copy (Commands[i+1],1,1)) ;
      TextColor (CommandFG) ;
      write (copy (Commands[i+1],2,Length(Commands[i+1])), ') ') ;
      Key := readkeyword;
      GotoXY (1,25);
      ClrEol;
    end;


  Procedure DoCommand;
    var    ch : char;
           memb : membertype ;
    begin
      Case Hi(Key) of
        kHelp : Options;
        kNew : NewMember;
        kEdit,kEnter: if pCurrent <> pHead then
                        begin
                          memb := pCurrent^ ;
                          DeleteMember (pCurrent) ;
                          pCurrent := pHead^.pFlink ;
                          GetMember (memb) ;
                          InsertMember (memb) ;
                        end
                     else
                       NewMember;
        kDel : begin
                if pCurrent <> pHead then
                  begin
                    TextColor (CommandFG);
                    TextBackGround (CommandBG);
                    GotoXY (1,25); ClrEol;
                    Write ('Delete this record? [y/N]');
                    ch := upcase (readkey);
                    if (ch in ['Y',^M]) then
                      begin
                       DeleteMember (pCurrent);
                       pCurrent := pHead^.pFlink ;
                      end ;
                    GotoXY (1,25);
                    Clreol;
                  end
                else
                  write (^G);
              end;
        kUp  : if (pCurrent <> pHead) then
                  if (pCurrent^.pBlink = pHead) then
                    pCurrent := pHead^.pBlink
                  else
                    pCurrent := pCurrent^.pBlink
              else
                write (^G);
        kDown : If (pCurrent <> pHead) then
                 if (pCurrent^.pFlink = pHead) then
                   pCurrent := pHead^.pFlink
                 else
                   pCurrent := pCurrent^.pFlink
               else
                  write (^G);
        kFind : FindMember ;
        kPrt  : Print ;
        kScan : Scan ;
        kSave : if Not Modified then
                  begin
                    TextColor (CommandFG);
                    TextBackGround (COmmandBG);
                    GotoXY (1, 25) ;
                    ClrEol ;
                    FastWrite ('No modifications since last change...not saving.', 25,1, CommandFG+16*CommandBG);
                    delay (1000) ;
                  end
                else
                  savefile ;
        kEsc  : {halt} ;
      end;
    end;



begin { main program }
  if CurrentDisplay >= EGA then
    if Font8x8Selected then
      begin
        Line43 := True;
        SelectFont8x8 (false);
      end
    else
      Line43 := false;
  DirectVideo := true;
  CursorToEnd := False;
  InsertbyDefault := False;
  AllowExtraKeys := True;
  SetFrameChars ('∫', 'Õ','º','ª','»','…');

  if not MakeWindow (ScanWindow, 30,9, 74,23, True, True, True, Black, MapColor(Magenta+16*LightGray),
                     MapColor(Red+16*LightGray), ' Look Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;


  if not MakeWindow (OptionWindow, 5,5, 74,19, True, True, True, Black, LightMagenta,
                     LightRed, ' Option Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;

  if not MakeWindow (FindWindow, 20,15, 60,19, True, True, True, Black, LightGreen,
                     yellow, ' Find Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;

  if not MakeWindow (PrintWindow, 5,5, 74,19, True, True, True, Black, LightGreen,
                     LightCyan, ' Print Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;


  MainDisplay;
  FastWrite ('  ‰AE Chapter Membership Program V'+Version+' - (C) Copyright 1989, Charles E. Kindel  ',
              1,1, MapColor(Red+16*LightGray));

  New (pHead) ;
  ClearMember (pHead^) ;
  pHead^.pFlink := pHead ;
  pHead^.pBlink := pHead ;
  pHead^.last := 'Header' ;
  ReadFile;
  pCurrent := pHead^.pFlink ;
  repeat
    If Not Modified then
      FastWrite ('»'+CharStr('Õ',78)+'º', 24, 1, MainFG);
    if (pCurrent = pHead) then
      begin
        Fastwrite (' No Members in file ', 2, 3, LightBlue+16*MainBG);
        ClearMember (mem);
      end
    else
      Mem := pCurrent^ ;
    DisplayMember (Mem);
    GetCommand;
    DoCommand;
  until (Hi(Key) in [kQuit]);
  if Modified then
    SaveFile;
  TextColor (lightGreen);
  TextBackGround (Black);
  if Line43 then
    SelectFont8x8 (true);
  ClrScr;
  DisposeWindow (OptionWindow) ;
  DisposeWindow (FindWindow) ;
  DisposeWindow (PrintWindow) ;
  DisposeWindow (ScanWindow) ;
end.

