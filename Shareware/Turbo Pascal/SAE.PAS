Program SAE;
{$R+}

  Uses Calendar,
       TpString,
       TpDos,
       TpCrt,
       TpEdit,
       TpWindow;


  Const
        Version   = 'b.01';
        MaxMembers = 180;
        FileName  = '‰AE.DAT';
        Quit      = 16;
        New       = 49;
        Option    = 24;
        Edit      = 18;
        Enter     = 28;
        Del       = 32;
        ESC       = 1;
        Up        = 72;
        Down      = 80;
        Right     = 77;
        Left      = 75;
        Help      = 59;
        Find      = 33;

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
                    end;
       newptr = ^newtype ;
       newtype  = record
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
                      flink : newptr ;
                      blink : newptr ;
                    end;
       statusarray = array [StatusType] of string [StatusLen];


  Const
       StatusString : StatusArray = ('No Status', 'In Good Standing', 'In Bad Standing', 'Inactive',
                                     'Pledge', 'Alumni', 'Graduating Senior', 'Social');

  Var
       members : array [1..MaxMembers] of membertype;
       Current, NumMembers : word;
       MemberFile : file of membertype;
       Key : word;
       Command : char absolute Key;
       Escaped :boolean;
       Line43  : boolean;
       Mem : Membertype;
       FindWindow, OptionWindow : WindowPtr;
       CommandPrompt : string;



  Procedure DeleteMember(i : integer);
    var j : integer;
    begin
      for j := i to NumMembers do
        Members[j] := Members [j+1];
      NumMembers := Pred (NumMembers);
    end;


  Procedure InsertRec (mem : membertype; i : Integer);
    var j : integer;
    begin
      if (i <= NumMembers) then
        For j := NumMembers downto i do
          Members [j+1] := Members [j];
      Members[i] := mem;
      Inc(NumMembers);
    end;



  Procedure InsertMember (mem : membertype);
    var Temp : membertype;
        i : integer;

    begin
      i := 1;
      if NumMembers > 0 then
        begin
          while ((StUpCase (mem.last) > StUpCase (Members[i].last)) and (i <= NumMembers)) do
            inc (i);
          InsertRec (mem, i);
        end
      else
        begin
          inc (NumMembers);
          Members [1] := Mem;
        end;
    end;

  Procedure ReadFile;

    var ch :char;
        mem : membertype;

    begin
      ClrScr;
      NumMembers := 0;
      If ExistFile (FileName) then
        begin
          Assign (MemberFile, FileName);
          Reset (MemberFile);
          writeln ('reading...');
          writeln;
          while not Eof (MemberFile) do
            begin
              read (MemberFile, Mem);
              writeln (Mem.last);
              InsertMember (mem);
            end;
          Close (MemberFile);
          writeln;
          writeln ('Done reading.');
        end
      else
        begin
          Write (FileName, ' not found, create? ');
          ch := upcase (readkey);
          if ch = 'Y' then
            begin
              Writeln ('Creating ',FileName,'...');
              Assign (MemberFile, FileName);
              ReWrite (MemberFile);
              Close (MemberFile);
            end
          else
            begin
              Writeln ('Exiting!');
              halt;
            end;
        end;
    end;


  Procedure SaveFile;
    var i : word;
        mem2 : newtype ;

    begin
      ClrScr;
      Assign (MemberFile, FileName);
      Rewrite (MemberFile);
      Writeln ('Writing file...');
      for i := 1 to NumMembers do
        begin
        with mem2 do
        begin
          Last  := Members[i].last;
          First := Members[i].first;
          MI := Members[i].MI;
          BirthDate := Members[i].birthdate;
          PinNumber := Members[i].pinnumber;
          PledgeClass := Members[i].pledgeclass;
          SSN := Members[i].SSN;
          HomeAddr.street := Members[i].homeaddr.street;
          HomeAddr.City   := Members[i].homeaddr.city;
          HomeAddr.State  := Members[i].homeaddr.state;
          HomeAddr.Zip    := Members[i].homeaddr.zip;
          HomeAddr.Phone  := Members[i].homeaddr.phone;
          ParentName := Members[i].parentname;
          SchoolAddr.street := Members[i].SchoolAddr.Street;
          SchoolAddr.City   := Members[i].SchoolAddr.City;
          SchoolAddr.State  := Members[i].SchoolAddr.State ;
          SchoolAddr.Zip    := Members[i].SchoolAddr.Zip;
          SchoolAddr.Phone  := Members[i].SchoolAddr.Phone;
          Major := Members[i].Major;
          CumGPA := Members[i].CumGPA;
          PrevGPA := Members[i].PrevGPA;
          HouseStatus := Members[i].HouseStatus;
          Flink := NIL ;
          bLINK  := nil ;
        end;
          write (MemberFile, Mem);
        end ;
      Close (memberFile);
      writeln ('done');
    end;

  Procedure Options;
    var c : word;
    begin
      if not DisplayWindow (OptionWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      FastWriteWindow ('Memory Available = '+Long2Str (memAvail), 5,5, LightMagenta);
      FastWriteWindow ('Press any key ', 1,1, Yellow);
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
        begin
          PledgeClassStr := '*Invalid*';
          exit;
        end;
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
        PledgeClassNum := InvalidClass;
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
        end;
    end;



      Function GetName (X,Y : byte; Old : nametype) : nametype;
        var temp : string;
        begin
          temp := old;
          ReadString ('', Y, X, NameLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16, Escaped, temp);
          FastWrite (CharStr(' ',NameLen), Y,X,DataFG+(DataBG*16));
          FastWrite (temp,Y,X,DataFG+(DataBG*16));
          GetName := temp;
        end;

      Function GetMI (X,Y : byte; Old : char) : char;
        var temp : string;
        begin
          temp := old;
          Readstring ('', Y, X, 1, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (Temp[1], Y,X,DataFG+(DataBG*16));
          GetMI:= temp[1];
        end;


      Function GetBirthDate (X,Y : byte; Old : DateNum) : DateNum;
        var Datestring : string;
        begin
          repeat
            DateString := LotusDate (Old);
            ReadString ('', Y, X, DateLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, DateString);
            Old := FromLotusDate (DateString);
            if (Old = InvalidDate) then
              write (^G);
          until (Old <> InvalidDate);
          FastWrite (CharStr(' ',DateLen), Y,X,DataFG+(DataBG*16));
          FastWrite (LotusDate(Old),Y,X,DataFG+(DataBG*16));
          GetBirthDate := Old;
        end;

      Function GetPinNumber (X,Y : byte; Old : longint) : longint;
        begin
          ReadLongInt ('', Y,X, PinLen, Cyan, PromptFG+PromptBG*16, 1, 500000, Escaped, Old);
          FastWrite (CharStr(' ',PinLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Long2Str (Old),Y,X,DataFG+(DataBG*16));
          GetPinNumber := Old;
        end;

      Function GetSSN (X,Y : byte; Old : SSNtype) : SSNtype;
        var temp : string;
        begin
          temp := old;
          ReadString ('', Y,X, SSNLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16, Escaped, temp);
          FastWrite (CharStr (' ', SSNLen), Y,X, DataFG+(DataBG*16));
          FastWrite (Temp, Y,X, DataFG+(DataBG*16));
          GetSSN := temp;
        end;

      Function GetPledgeClass (X,Y : byte; Old : PledgeClasstype) : pledgeclasstype;
        var temp : string;
        begin
          repeat
            temp := PledgeClassStr (Old);
            ReadString ('', Y, X, PledgeLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
            Old := PledgeClassNum (temp);
            if (Old = InvalidClass) then
              write (^G);
           until (Old <> InvalidClass);
           FastWrite (CharStr(' ',PledgeLen), Y,X,DataFG+(DataBG*16));
           FastWrite (PledgeClassStr (Old),Y,X,DataFG+(DataBG*16));
           GetPledgeClass := Old;
        end;

      Function GetStreet (X,Y : byte; Old : streettype) : streettype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('', Y,X, StreetLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',StreetLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          GetStreet := temp;
        end;

      Function GetCity (X,Y : byte; Old : citytype) :citytype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('',Y,X, CityLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped,temp);
          FastWrite (CharStr(' ',CityLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
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
          GetState := temp;
        end;


      Function GetZip (X,Y : byte; old : ziptype) : ziptype;
        var temp : string;
        begin
          temp := Old;
          ReadString ('', Y,X, ZipLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',ZipLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          GetZip := temp;
        end;

      Function GetPhone (X,Y : byte; old : phonetype) :phonetype;
        var temp : string;
        begin
          temp := old;
          ReadString ('',Y,X, PhoneLen, Cyan, PromptFG+PromptBG*16, PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',PhoneLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          GetPhone := temp;
        end;

      Function GetMajor (X,Y : byte; old : majortype) :majortype;
        var temp : string;
        begin
          temp := old;
          ReadString ('',Y,X, MajorLen, Cyan, PromptFG+PromptBG*16,  PromptFG+PromptBG*16,Escaped, temp);
          FastWrite (CharStr(' ',MajorLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Temp,Y,X,DataFG+(DataBG*16));
          GetMajor := temp;
        end;

      Function GetUnits (X,Y : byte; old : integer) : integer;
        begin
          ReadInteger ('', Y,X, UnitsLen, Cyan,PromptFG+PromptBG*16, 0, 1000, Escaped,Old);
          FastWrite (CharStr(' ',UnitsLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Long2Str (Old),Y,X,DataFG+(DataBG*16));
          GetUnits := Old;
        end;

      Function Real2Str (R : real) : string;
        var ST : string;
        begin
          Str (R:GPAlen:GPALen-2,ST);
          Real2Str := ST;
        end;


      Function GetGPA (X,Y:byte; old : real) : real;
        begin
          ReadReal ('', Y,X, GPALen, Cyan, PromptFG+PromptBG*16, 4, 0, 4, Escaped, old);
          FastWrite (CharStr(' ',GPALen), Y,X,DataFG+(DataBG*16));
          FastWrite (Real2Str (Old),Y,X,DataFG+(DataBG*16));
          GetGPA := old;
        end;

      Function GetStatus (X,Y : byte; old : statustype) : statustype;
        var
            ch : word;
        begin
          repeat
            FastWrite (CharStr(' ',StatusLen), Y,X, PromptFG+PromptBG*16);
            FastWrite (Status2Str (Old), Y,X, PromptFG+PromptBG*16);
            GotoXY (X,Y);
            ch := readkeyword;
            if not ((char(Lo(ch)) in [^M, #27]) or ((char(lo(ch)) = #0) and (hi(ch) in [Up,Down]))) then
              if (old = Social) then
                Old := NoStatus
              else
                Inc (Old);
          until ((char(Lo(ch)) in [^M, #27]) or ((char(lo(ch)) = #0) and (hi(ch) in [Up,Down])));
          if char(Lo(ch)) = #27 then
            Escaped := True;
          FastWrite (charstr(' ',StatusLen), Y,X,DataFG+(DataBG*16));
          FastWrite (Status2Str (Old),Y,X,DataFG+(DataBG*16));
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
      FastWrite (Charstr(' ',PinLen), PinY, PinX+Length(PinPrompt), Col);
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
    var Field : byte;
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
{          4 : mem.SSN := GetSSN (SSNX+Length(SSNPrompt), SSNY, mem.SSN);
          5 : mem.BirthDate := GetBirthDate (DateX+Length(BirthPrompt), DateY, mem.BirthDate);
          6 : mem.PinNumber := GetPinNumber (PinX+length(PinPrompt), PinY, mem.PinNumber);
          7 : mem.PledgeClass := GetPledgeClass (PledgeX+length(PledgePrompt), PledgeY, mem.PledgeClass);

          8 : mem.Major := GetMajor (MajorX+Length(MajorPrompt), MajorY, mem.Major);
          9 : mem.CumGPA := GetGPA (CumX+Length(CumGPAPrompt), CumY, mem.CumGPA);
          10: mem.PrevGPA := GetGPA (PrevX+Length(PrevGPAPrompt), PrevY, Mem.PrevGPA);
          11: mem.HouseStatus := GetStatus (StatusX+Length(StatusPrompt), StatusY, mem.Housestatus);
}
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
                Up   : if Field = 1 then
                         Field := 22
                       else
                         Field := Pred(Field);
                Down : if Field = 22 then
                         Field := 1
                       else
                         Inc (Field);
               end;
          ^M : if Field = 22 then
                 Field := 1
               else
                 Inc(Field);
        end;
      until (Escaped);
    end;


  Procedure NewMember;
    var Old :integer;
        Mem :membertype;
        line : string ;
    begin
      Old := Current;
      Current := NumMembers+1;
      ClearMember (Mem);
      DisplayPrompts;
      Fastwrite ('New Member',2,3, LightBlue+16*MainBG);
      GEtMember (Mem);
      InsertMember (Mem);
      line := '' ;
      for old  := 1 to 11 do
         line:= line + chr(205) ;
      FastWrite (line, 2,3, Yellow) ;

    end;

  Procedure FindMember;
    var c : word;
        name : nametype;
        i : integer;

    begin
      if not DisplayWindow (FindWindow) then
        begin
          write (^G, 'Error in DisplayWindow');
          halt;
        end;
      ClrScr;
      GotoXY (2,3);
      name := '';
      fastwritewindow ('Enter Name: ', 2,2, LightBlue);
      name := GetName (10,12, name);
      i := 1;
      while ((i <= NumMembers) and (StUpCase (Name) <> StUpCase (Members[i].last))) do
        inc (i);
      if (StUpCase(Members[i].last) = StUpcase(Name)) then
        current := i
      else
        writeln ('Not Found!');
      FastWriteWindow ('Press any key ', 10,1, Yellow);
      c := readkeyWord;
      FindWindow := EraseTopWindow;
    end;



  Procedure GetCommand;
    begin
      TextBackGround (CommandBG);
      TextColor (CommandFG);
      GotoXY (1,25);
      ClrEol;
      Write (CommandPrompt);
      Key := readkeyword;
      GotoXY (1,25);
      ClrEol;
    end;


  Procedure DoCommand;
    var    ch : char;
    begin
      Case Hi(Key) of
        Option : Options;
        New : NewMember;
        Edit,Enter: if Current <> 0 then
                       GetMember (Members[Current])
                     else
                       NewMember;
        Del : begin
                if Current <> 0 then
                  begin
                    TextColor (CommandFG);
                    TextBackGround (CommandBG);
                    GotoXY (1,25); ClrEol;
                    Write ('Delete this record? [y/N]');
                    ch := upcase (readkey);
                    if (ch in ['Y',^M]) then DeleteMember (current);
                    if (Current > NumMembers) then Current := NumMembers;
                    GotoXY (1,25);
                    Clreol;
                  end
                else
                  write (^G);
              end;
        Up  : if (Current <> 0) then
                  if (Current = 1) then
                    Current := NumMembers
                  else
                    Current := Pred (Current)
                else
                  write (^G);
        Down : If (Current <> 0) then
                 if (current = NumMembers) then
                   Current := 1
                 else
                   Inc(Current)
               else
                  write (^G);
        Find : FindMember;
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
  CommandPrompt  := 'Command ('+#24+#25+',E,D,N,O,[ESC=Quit]): ';


  if not MakeWindow (OptionWindow, 5,5, 74,19, True, True, True, Black, LightMagenta,
                     LightRed, ' Option Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;

  if not MakeWindow (FindWindow, 5,5, 74,19, True, True, True, Black, LightGreen,
                     yellow, ' Find Window ') then
    begin
      write (^G, 'Error in MakeWindow');
      halt;
    end;

  ReadFile;
  Current := NumMembers;
  MainDisplay;
  FastWrite ('  ‰AE Chapter Membership Program V'+Version+' - (C) Copyright 1988, Charles E. Kindel  ',
              1,1, Red+16*Green);
  repeat
    if (Current = 0) then
      begin
        Fastwrite (' No Members in file ', 3, 3, LightBlue+16*MainBG);
        ClearMember (mem);
      end
    else
      Mem := Members[Current];
    DisplayMember (Mem);
    GetCommand;
    DoCommand;
  until (Hi(Key) = Esc);
  SaveFile;
  TextColor (lightGreen);
  TextBackGround (Black);
  if Line43 then
    SelectFont8x8 (true);
  ClrScr;
end.

