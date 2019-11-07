{TPHELP.PAS Copyright (C) 1988, by TurboPower Software}

{$R-,S-,I-}

{Activate the following define if you have the TPPICK unit}
{ $DEFINE UsingPickUnit}

unit TpHelp;
  {-General purpose help facility}

interface

uses
  Dos,
  TPDos,
  TPString,
  TPCrt,
  TPWindow
  {$IFDEF UsingPickUnit}
  , TpPick
  {$ENDIF}
  ;

const
  HelpId : array[0..3] of Char = 'TPH0'; {Identifier at start of help file}
  NoHelpAvailable = $FFFFFFFF;    {Flag that no help is available for topic}
  MaxPagesPerSection = 21;        {Maximum number of pages of help per section}
  Attr1Toggle    = ^A;            {Character toggles special attribute 1}
  Attr2Toggle    = ^B;            {Character toggles special attribute 2}
  Attr3Toggle    = ^C;            {Character toggles special attribute 3}
  LineBrkMark    = ^M;            {Denotes end of line of help}
  PageBrkMark    = ^L;            {Denotes end of page of help}
  SectEndMark    = #0;            {Denotes end of help section}
  PickTitle : string[80] = ' Topics ';

type
  HelpAttrType   = (FrAttr, TeAttr, HeAttr, SpAtt1, SpAtt2, SpAtt3);
  HelpAttrArray  = array[HelpAttrType] of Byte;
  HelpAttrState  = array[SpAtt1..SpAtt3] of Boolean;
  HelpHeader =
    record
      ID             : LongInt;   {Marks file as help file}
      MaxSection     : Word;      {Size of largest help section in bytes}
      ItemCnt        : Word;      {Number of help entries in index}
      NameSize       : Byte;      {Size of each entry in pick table, 0 for none}
      Width          : Byte;      {Width of help window, with frame}
    end;
  CharArray      = array[0..64000] of Char; {List of names of help entries}
  HelpIndex      = array[1..16000] of LongInt; {Index of file positions}
  CharArrayPtr   = ^CharArray;
  HelpIndexPtr   = ^HelpIndex;
  HelpHeaderPtr  = ^HelpHeader;

  HelpPtr        = ^HelpDesc;     {The user hook to the help system}
  HelpDesc =                      {Holds parameters of help system}
    record
      RowH           : Byte;      {Upper left corner of help window - Row}
      ColH           : Byte;      {Upper left corner of help window - Col}
      CAttr          : HelpAttrArray; {Attributes used to draw help in color}
      MAttr          : HelpAttrArray; {Attributes used to draw help in monoc}
      Frame          : FrameArray; {Frame characters to use}
      Hdr            : HelpHeader; {Copy of header for fast reference}
      Height         : Byte;      {Height of help window, with frame}
      case InRAM     : Boolean of {True if help file is bound into code}
        True :
          (HdrP           : HelpHeaderPtr; {Points to base of structure in RAM}
           NamP           : CharArrayPtr; {Points to pick name array in RAM}
           IndP           : HelpIndexPtr); {Points to help section index in RAM}
        False :
          (Open           : Boolean; {True when file is open}
           BufP           : CharArrayPtr; {Points to a buffer that will hold largest section}
           Fil            : file); {Untyped file variable for help}
    end;

const
  {Default help colors}
  HelpColorAttr : HelpAttrArray = ($71, $30, $71, $1F, $3E, $31);
  HelpMonocAttr : HelpAttrArray = ($07, $07, $0F, $70, $0F, $01);

  {Context sensitive help}
  CurrentTopic : Word = 0;        {Current help topic}
  HelpIntInstalled : Boolean = False; {True if interrupt handler installed}

function OpenHelpFile(HelpFileName   : string;
                      XLow, YLow, YHigh : Byte;
                      var Help : HelpPtr) : Word;
  {-Find and open help file, returning 0 or error code, and
    an initialized help descriptor if successful}

function OpenHelpMem(HPtr           : Pointer;
                     XLow, YLow, YHigh : Byte;
                     var Help : HelpPtr) : Word;
  {-Initialize help descriptor for a help structure bound into code}

procedure SetHelpPos(Help : HelpPtr; XLow, YLow, YHigh : Byte);
  {-Change the position and height of a help window}

function ShowHelp(Help : HelpPtr; Item : Word) : Boolean;
  {-Display help screen, returning true if successful}

function ShowHelpByName(Help : HelpPtr; Name : string) : Boolean;
  {-Display help screen for topic with pick name Name}

  {$IFDEF UsingPickUnit}
function PickHelp(Help : HelpPtr; XLow, YLow, YHigh, PickCols : byte) : word;
  {-Display help pick list, returning Item number, or 0 for none}
  {$ENDIF}

procedure SetContextHelp(Help : HelpPtr; Key : Word);
  {-Install a keyboard interrupt handler to pop help when Key is pressed}

procedure RemoveHelp;
  {-Deinstall context sensitive help}

  {=========================================================================}

implementation

type
  PageIndex      = array[1..MaxPagesPerSection] of Word;
  PageAttr       = array[1..MaxPagesPerSection] of HelpAttrState;

  {$F+}
  function HeapFunc(Size : Word)   : Integer;
    {-Return nil pointer if insufficient memory}
  begin
    HeapFunc := 1;
  end;
  {$F-}

  function GetMemCheck(var P; Bytes : Word) : Boolean;
    {-Allocate heap space, returning true if successful}
  var
    SaveHeapError  : Pointer;
    Pt             : Pointer absolute P;
  begin
    {Take over heap error control}
    SaveHeapError := HeapError;
    HeapError := @HeapFunc;
    GetMem(Pt, Bytes);
    GetMemCheck := (Pt <> nil);
    {Restore heap error control}
    HeapError := SaveHeapError;
  end;

  procedure FreeMemCheck(var P; Bytes : Word);
    {-Deallocate heap space}
  var
    Pt             : Pointer absolute P;
  begin
    if Pt <> nil then
      FreeMem(Pt, Bytes);
  end;

  function OpenHelpFile(HelpFileName   : string;
                        XLow, YLow, YHigh : Byte;
                        var Help : HelpPtr) : Word;
    {-Find and open help file, returning 0 or error code, and
      an initialized help descriptor if successful}
  label
    ErrorExit;
  var
    IO             : Word;
    BytesRead      : Word;
    IsOpen         : Boolean;
  begin
    {Initialize the result}
    Help := nil;
    IsOpen := False;

    {Find the help file}
    if not ExistOnPath(HelpFileName, HelpFileName) then begin
      OpenHelpFile := 2;
      goto ErrorExit;
    end;

    {Allocate space for help descriptor}
    if not GetMemCheck(Help, SizeOf(HelpDesc)) then begin
      OpenHelpFile := 203;
      goto ErrorExit;
    end;

    {Initialize the help descriptor}
    with Help^ do begin
      {Most help information is on disk}
      InRAM := False;

      {Open the help file}
      Assign(Fil, HelpFileName);
      Reset(Fil, 1);
      IO := IoResult;
      if IO <> 0 then begin
        OpenHelpFile := IO;
        goto ErrorExit;
      end;
      IsOpen := True;

      {Get header from file}
      BlockRead(Fil, Hdr, SizeOf(HelpHeader), BytesRead);
      IO := IoResult;
      if IO <> 0 then begin
        OpenHelpFile := IO;
        goto ErrorExit;
      end;
      if BytesRead <> SizeOf(HelpHeader) then begin
        OpenHelpFile := 100;
        goto ErrorExit;
      end;

      with Hdr do begin
        {Check file ID}
        if ID <> LongInt(HelpId) then begin
          {"Invalid numeric format" - used as error code for invalid ID}
          OpenHelpFile := 106;
          goto ErrorExit;
        end;
        {Get buffer space for reading help sections}
        if not GetMemCheck(BufP, MaxSection) then begin
          OpenHelpFile := 203;
          goto ErrorExit;
        end;
      end;

      {Initialize remaining fields}
      RowH := YLow;
      ColH := XLow;
      Height := YHigh-YLow+1;
      CAttr := HelpColorAttr;
      MAttr := HelpMonocAttr;
      Frame := FrameChars;
      Open := True;

      {Successful initialization}
      OpenHelpFile := 0;
      Exit;
    end;

ErrorExit:
    if IsOpen then begin
      Close(Help^.Fil);
      IsOpen := False;
      IO := IoResult;
    end;
    FreeMemCheck(Help, SizeOf(HelpDesc))
  end;

  function OpenHelpMem(HPtr           : Pointer;
                       XLow, YLow, YHigh : Byte;
                       var Help : HelpPtr) : Word;
    {-Initialize help descriptor for a help structure bound into code}
  label
    ErrorExit;
  begin
    {Initialize the result in case of failure}
    Help := nil;

    {Allocate space for help descriptor}
    if not GetMemCheck(Help, SizeOf(HelpDesc)) then begin
      OpenHelpMem := 203;
      goto ErrorExit;
    end;

    {Initialize the help descriptor}
    with Help^ do begin
      {Help information is in RAM}
      InRAM := True;

      {Check out header}
      HdrP := HPtr;
      Hdr := HdrP^;
      with Hdr do begin
        if ID <> LongInt(HelpId) then begin
          {"Invalid numeric format" - used as error code for invalid ID}
          OpenHelpMem := 106;
          goto ErrorExit;
        end;
        NamP := HPtr;
        Inc(LongInt(NamP), SizeOf(HelpHeader));
        IndP := Pointer(NamP);
        Inc(LongInt(IndP), ItemCnt*NameSize);
      end;

      {Initialize remaining fields}
      RowH := YLow;
      ColH := XLow;
      Height := YHigh-YLow+1;
      CAttr := HelpColorAttr;
      MAttr := HelpMonocAttr;
      Frame := FrameChars;

      {Successful initialization}
      OpenHelpMem := 0;
      Exit;
    end;

ErrorExit:
    FreeMemCheck(Help, SizeOf(HelpDesc))
  end;

  procedure SetHelpPos(Help : HelpPtr; XLow, YLow, YHigh : Byte);
    {-Change the position of a help window}
  begin
    with Help^ do
      if Hdr.ID = LongInt(HelpId) then begin
        RowH := YLow;
        ColH := XLow;
        Height := YHigh-YLow+1;
      end;
  end;

  function GetNameString(Help : HelpPtr; Item : Word) : string;
    {-Return name string for help item, if any}
  var
    N              : ^string;
    S              : string;
    C              : CharArrayPtr;
  begin
    GetNameString := '';
    with Help^, Hdr do
      if NameSize <> 0 then
        if InRAM then begin
          N := Pointer(NamP);
          Inc(LongInt(N), NameSize*(Item-1));
          GetNameString := N^;
        end else if Open then begin
          Seek(Fil, LongInt(SizeOf(HelpHeader))+NameSize*(Item-1));
          if IoResult <> 0 then
            Exit;
          BlockRead(Fil, S, NameSize);
          if (IoResult <> 0) then
            Exit;
          GetNameString := S;
        end;
  end;

  function PaginateHelp(var C : CharArray; TextHgt : Word;
                        var P : PageIndex; var PA : PageAttr) : Word;
    {-Paginate help text for a single section}
  var
    Cpos           : Word;
    Pofs           : Word;
    Pcnt           : Word;
    Phgt           : Word;
    Done           : Boolean;
    HA, LA         : HelpAttrState;

    procedure NewPage;
      {-Store information about previous page}
    begin
      if Pcnt+1 >= MaxPagesPerSection then
        Done := True
      else begin
        Inc(Pcnt);                {Increment page count}
        P[Pcnt] := Pofs;          {Character offset at start of page}
        PA[Pcnt] := LA;           {Attrubute at start of page}
        Pofs := Cpos+1;           {Start of next page}
        P[Pcnt+1] := Pofs;        {Sentinel to end last page}
        Phgt := 0;                {New page has no lines}
        LA := HA;                 {Attributes at start of new page}
      end;
    end;

  begin
    Pcnt := 0;
    Cpos := 0;
    Pofs := 0;
    Phgt := 0;
    FillChar(HA, SizeOf(HelpAttrState), False);
    LA := HA;
    Done := False;
    repeat
      case C[Cpos] of
        Attr1Toggle :
          HA[SpAtt1] := not HA[SpAtt1];
        Attr2Toggle :
          HA[SpAtt2] := not HA[SpAtt2];
        Attr3Toggle :
          HA[SpAtt3] := not HA[SpAtt3];
        LineBrkMark :
          begin
            Inc(Phgt);
            if Phgt >= TextHgt then
              NewPage;
          end;
        PageBrkMark :
          if Cpos = Pofs then
            Inc(Pofs)
          else
            NewPage;
        SectEndMark :
          begin
            if Cpos <> Pofs then
              NewPage;
            Done := True;
          end;
      end;
      Inc(Cpos);
    until Done;
    PaginateHelp := Pcnt;
  end;

  procedure ShowPrompt(Pnum, Pcnt : Word; Row, ColMin, ColMax, Attr : Byte);
    {-Show information about help}
  const
    MoreMsg : string[13] = '  for more,';
    ExitMsg : string[13] = ' Esc to exit ';
  var
    Cpos           : Byte;
  begin
    Cpos := ColMax+1-Length(ExitMsg);
    if Cpos < ColMin then
      {No room for any messages}
      Exit;
    FastWrite(ExitMsg, Row, Cpos, Attr);
    if Pcnt = 1 then
      {No need for More message}
      Exit;
    Dec(Cpos, Length(MoreMsg));
    if Cpos < ColMin then
      {No room for More message}
      Exit;
    if Pnum = 1 then
      MoreMsg[2] := ' '
    else
      MoreMsg[2] := ^X;
    if Pnum = Pcnt then
      MoreMsg[3] := ' '
    else
      MoreMsg[3] := ^Y;
    FastWrite(MoreMsg, Row, Cpos, Attr);
  end;

  function GetAttr(var A          : HelpAttrArray;
                   var AtSt : HelpAttrState) : Byte;
    {-Return attribute for current attribute state}
  begin
    if AtSt[SpAtt1] then
      GetAttr := A[SpAtt1]
    else if AtSt[SpAtt2] then
      GetAttr := A[SpAtt2]
    else if AtSt[SpAtt3] then
      GetAttr := A[SpAtt3]
    else
      GetAttr := A[TeAttr];
  end;

  function ToggleAttr(var A          : HelpAttrArray;
                      var AtSt       : HelpAttrState;
                      SpAtt : HelpAttrType) : Byte;
    {-Toggle attribute state and return new video attribute}
  begin
    AtSt[SpAtt] := not AtSt[SpAtt];
    ToggleAttr := GetAttr(A, AtSt);
  end;

  procedure DrawPage(var C : CharArray; Pstart, Pend : Word; ColMax : Byte;
                     var A : HelpAttrArray; AtSt : HelpAttrState);
    {-Draw one page of help}
  const
    ColSt          = 2;
  var
    Attr           : Byte;
    Pdone          : Boolean;
    Cpos           : Word;
    Row            : Byte;
    Col            : Byte;
    Ch             : Char;
  begin
    Row := 1;
    Col := ColSt;
    Attr := GetAttr(A, AtSt);
    ClrScr;
    Cpos := Pstart;
    Pdone := False;
    repeat
      Ch := C[Cpos];
      case Ch of
        LineBrkMark :
          begin
            Inc(Row);
            Col := ColSt;
          end;
        Attr1Toggle :
          Attr := ToggleAttr(A, AtSt, SpAtt1);
        Attr2Toggle :
          Attr := ToggleAttr(A, AtSt, SpAtt2);
        Attr3Toggle :
          Attr := ToggleAttr(A, AtSt, SpAtt3);
        PageBrkMark, SectEndMark :
          Pdone := True;
      else
        if Col <= ColMax then
          FastWriteWindow(Ch, Row, Col, Attr);
        Inc(Col);
      end;
      Inc(Cpos);
      if Cpos >= Pend then
        Pdone := True;
    until Pdone;
  end;

  function ShowHelp(Help : HelpPtr; Item : Word) : Boolean;
    {-Display help screen, returning true if successful}
  var
    Done           : Boolean;
    Ch             : Char;
    Pnum           : Word;
    Lnum           : Word;
    Pcnt           : Word;
    BytesRead      : Word;
    Fpos           : LongInt;
    W              : WindowPtr;
    A              : HelpAttrArray;
    C              : CharArrayPtr;
    P              : PageIndex;
    PA             : PageAttr;
    HeaderStr      : string[80];
  begin
    ShowHelp := False;
    if Item = 0 then
      exit;

    with Help^, Hdr do begin

      {Get help text into memory and initialize pointer to it}
      if InRAM then begin
        {Already in memory, just compute the pointer}
        C := Pointer(HdrP);
        Inc(LongInt(C), IndP^[Item]);
      end else if Open then begin
        {On disk, first read the index}
        Seek(Fil, SizeOf(HelpHeader)+LongInt(NameSize)*ItemCnt+SizeOf(LongInt)*(Item-1));
        if IoResult <> 0 then
          Exit;
        BlockRead(Fil, Fpos, SizeOf(LongInt), BytesRead);
        if (IoResult <> 0) or (BytesRead <> SizeOf(LongInt)) then
          Exit;
        {Check for available help}
        if Fpos = NoHelpAvailable then
          Exit;
        {Now read the help section}
        Seek(Fil, Fpos);
        if IoResult <> 0 then
          Exit;
        BlockRead(Fil, BufP^, MaxSection, BytesRead);
        if (IoResult <> 0) or (BytesRead = 0) then
          Exit;
        C := BufP;
      end else
        {Help file not open}
        Exit;

      {Scan help text to find page boundaries}
      Pcnt := PaginateHelp(C^, Height-2, P, PA);

      if Pcnt = 0 then
        {No help for this topic}
        Exit;

      {Set colors and frame}
      case LastMode and $FF of
        0, 2, 7 : A := MAttr;
        1, 3 : A := CAttr;
      else
        Exit;
      end;
      FrameChars := Frame;

      {Display window}
      HeaderStr := GetNameString(Help, Item);
      if Length(HeaderStr) > 0 then
        HeaderStr := ' '+HeaderStr+' ';
      if not MakeWindow(W, ColH, RowH, ColH+Width-1, RowH+Height-1,
                        True, True, False, A[TeAttr], A[FrAttr], A[HeAttr],
                        HeaderStr) then
        Exit;
      if not DisplayWindow(W) then
        Exit;
      HiddenCursor;

      {Allow user to browse help}
      Done := False;
      Pnum := 1;
      Lnum := 0;
      repeat
        if Pnum <> Lnum then begin
          DrawPage(C^, P[Pnum], P[Pnum+1], Width-3, A, PA[Pnum]);
          ShowPrompt(Pnum, Pcnt, RowH+Height-1, ColH+1, ColH+Width-2, A[HeAttr]);
          Lnum := Pnum;
        end;
        case ReadKeyWord of
          $011B :                 {Escape}
            Done := True;
          $4700 :                 {Home}
            Pnum := 1;
          $4800, $4900 :          {Up arrow, PgUp}
            if Pnum > 1 then
              Dec(Pnum);
          $4F00 :                 {End}
            Pnum := Pcnt;
          $5000, $5100 :          {Down arrow, PgDn}
            if Pnum < Pcnt then
              Inc(Pnum);
        end;
      until Done;

      {Restore the screen}
      DisposeWindow(EraseTopWindow);
      ShowHelp := True;
    end;
  end;

  function GetNameBuffer(Help           : HelpPtr;
                         var P          : CharArrayPtr;
                         var SizeAlloc : Word) : Boolean;
    {-Return pointer to loaded array of pick names}
  var
    BytesRead      : Word;
  begin
    GetNameBuffer := False;
    SizeAlloc := 0;
    with Help^, Hdr do begin
      if InRAM then begin
        {Already in memory, just compute the pointer}
        P := Pointer(HdrP);
        Inc(LongInt(P), SizeOf(HelpHeader));
      end else if Open then begin
        {On disk, first allocate space}
        if not GetMemCheck(P, ItemCnt*NameSize) then
          Exit;
        SizeAlloc := ItemCnt*NameSize;
        {Read names into buffer}
        Seek(Fil, SizeOf(HelpHeader));
        if IoResult <> 0 then
          Exit;
        BlockRead(Fil, P^, SizeAlloc, BytesRead);
        if (IoResult <> 0) or (BytesRead <> SizeAlloc) then
          Exit;
      end else
        {Help file not open}
        Exit;
    end;
    GetNameBuffer := True;
  end;

  function ShowHelpByName(Help : HelpPtr; Name : string) : Boolean;
    {-Display help screen for topic with pick name Name}
  var
    P              : CharArrayPtr;
    NP             : ^string;
    SizeAlloc      : Word;
    I              : Word;
  begin
    ShowHelpByName := False;
    if GetNameBuffer(Help, P, SizeAlloc) then
      with Help^, Hdr do begin
        {Match the name}
        Name := StUpcase(Name);
        NP := Pointer(P);
        I := 1;
        while I <= ItemCnt do begin
          if StUpcase(NP^) = Name then begin
            {Show the help, getting status from that routine}
            ShowHelpByName := ShowHelp(Help, I);
            {Force exit}
            I := ItemCnt;
          end;
          Inc(I);
          Inc(LongInt(NP), NameSize);
        end;
      end;
    if SizeAlloc <> 0 then
      FreeMem(P, SizeAlloc);
  end;

  {$IFDEF UsingPickUnit}
  var
    PBuff : CharArrayPtr;  {Pointer to buffer of pick names}
    NSize : byte;          {Size of array element in pick buffer}

  {$F+}
  function SendHelpName(Item : Word) : string;
    {-Pass each help item to the pick unit}
  var
    NP : ^string;
  begin
    NP := pointer(PBuff);
    inc(longint(NP),NSize*(Item-1));
    SendHelpName := ' '+NP^;
  end;
  {$F-}

  function PickHelp(Help : HelpPtr; XLow, YLow, YHigh, PickCols : byte) : word;
    {-Display help pick list, returning Item number, or 0 for none}
  var
    SizeAlloc : Word;
    Choice : Word;
    PickChar : Char;
    XHigh : byte;
    A : HelpAttrArray;
  begin
    PickHelp := 0;
    if GetNameBuffer(Help, PBuff, SizeAlloc) then
      with Help^, Hdr do begin
        {Set up global with NameSize}
        NSize := NameSize;

        {Choose the window width}
        XHigh := XLow+PickCols*(NSize+1)+1;
        if XHigh > CurrentWidth then
          XHigh := CurrentWidth;

        {Set colors and frame}
        case LastMode and $FF of
          0, 2, 7 : A := MAttr;
          1, 3 : A := CAttr;
        else
          Exit;
        end;
        FrameChars := Frame;
        TpPick.PickMatrix := PickCols;

        {Pick from list}
        if PickWindow(@SendHelpName, ItemCnt, XLow, YLow, XHigh, YHigh, True,
                      A[TeAttr], A[FrAttr], A[HeAttr], A[SpAtt1], PickTitle,
                      [#13, #27], Choice, PickChar) then
          if PickChar = #13 then
            PickHelp := Choice;
      end;
    if SizeAlloc <> 0 then
      FreeMem(PBuff, SizeAlloc);
  end;
  {$ENDIF}

const
  StackSize      = 2000;          {Size of stack for int 16 handler}
var
  SaveExit       : Pointer;       {Exit chain}
  SaveInt16      : Pointer;       {Previous int 16}
  HelpSystem     : HelpPtr;       {Help system for context sensitive help}
  HelpActive     : Boolean;       {True if context sensitive help popped up}
  HelpKey        : Word;          {Scan word for help popup}
  StackSpace     : Pointer;       {Pointer to alternate stack}

  {$L TPHELP.OBJ}
  procedure Int16Handler;
    {-Handle int 16 for popup context sensitive help}
    external;

  procedure RemoveHelp;
    {-Deinstall context sensitive help}
  begin
    if HelpIntInstalled then begin
      ExitProc := SaveExit;
      FreeMem(StackSpace, StackSize);
      SetIntVec($16, SaveInt16);
      HelpIntInstalled := False;
    end;
  end;

  procedure SetContextHelp(Help : HelpPtr; Key : Word);
    {-Install a keyboard interrupt handler to pop help when Key is pressed}
  begin
    if not HelpIntInstalled then
      if Help^.InRAM then
        if MaxAvail >= StackSize then begin
          GetMem(StackSpace, StackSize);
          GetIntVec($16, SaveInt16);
          SetIntVec($16, @Int16Handler);
          SaveExit := ExitProc;
          ExitProc := @RemoveHelp;
          HelpIntInstalled := True;
        end;
    HelpActive := False;
    HelpKey := Key;
    HelpSystem := Help;
  end;

end.
