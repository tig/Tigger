{
Copyright (c) 1987 by TurboPower Software. May be freely used by and
distributed to owners of Turbo Professional 4.0.
}

{$R-,I-,S-,V-}

unit TpDir;
  {-Use a pick window to select a filename}

interface

uses
  Dos,
  TPString,
  TPCrt,
  TPWindow,
  TpPick;

type
  DirColorType = (WindowAttr, FrameAttr, HeaderAttr, SelectAttr,
                  AltNormal, AltHigh);
  DirColorArray = array[DirColorType] of Byte;

const
  {Programs can modify these constants to change TPDIR behavior}
  DirMonocColors : DirColorArray = ($07, $07, $0F, $70, $0F, $78);
  DirColorColors : DirColorArray = ($1B, $17, $30, $7E, $1E, $7B);
  AltPathColor : Boolean = False; {True to use alternate colors for directory entries}
  XLow : Byte = 60;          {Position of pick window}
  YLow : Byte = 2;
  YHigh : Byte = 25;
  {XHigh is determined automatically}
  FileAttr : Byte = Directory; {File selection attribute}

function GetFileName(Mask : string; var FileName : string) : Word;
  {-Given a mask (which may or may not contain wildcards),
    popup a directory window, let user choose, and return pathname.
    Returns zero for success, non-zero for error.
    Error codes:
      0 = Success
      1 = Path not found
      2 = No matching files
      3 = Attempt to use popup in unsupported video mode
      4 = Insufficient memory
    else  Turbo critical error code
  }

  {=========================================================================}

implementation

const
  MaxFiles = 500;            {Absolute maximum number of files found in one directory}
type
  FileString = string[13];   {Has space for \ following subdirectory names}
  FileArray = array[1..MaxFiles] of FileString;
var
  F : ^FileArray;            {Pointer to file array}
  MaxNumFiles : Word;        {Maximum number of files we have memory space for}
  NumFiles : Word;           {Actual number of files found}
  Frec : SearchRec;          {Used in directory operations}

  function StUpcase(S : string) : string;
    {-Uppercase a string}
  var
    I : Integer;
  begin
    for I := 1 to Length(S) do
      S[I] := Upcase(S[I]);
    StUpcase := S;
  end;

  function HasWildCards(Mask : string) : Boolean;
    {-Return true if Mask has DOS wildcards}
  begin
    HasWildCards := (pos('*', Mask) <> 0) or (pos('?', Mask) <> 0);
  end;

  function EndsPathDelim(Mask : string) : Boolean;
    {-Return true if Mask ends in a DOS path delimiter}
  begin
    case Mask[Length(Mask)] of
      #0, ':', '\' : EndsPathDelim := True;
    else
      EndsPathDelim := False;
    end;
  end;

  function AddFilePath(Mask : string; Fname : FileString) : string;
    {-Concatenate a filemask and filename}
  var
    Mlen : Byte absolute Mask;
    Flen : Byte absolute Fname;
  begin
    if EndsPathDelim(Mask) then begin
      if ((Fname = '..\') and (Mlen > 2) and
          (Mask[Mlen] = '\') and (Mask[Mlen-1] <> '.')) then begin
        {Remove last subdirectory}
        repeat
          Dec(Mlen);
        until EndsPathDelim(Mask);
        AddFilePath := Mask;
      end else
        AddFilePath := Mask+Fname
    end else
      AddFilePath := Mask+'\'+Fname;
  end;

  function AddWildCard(Mask : string) : string;
    {-Add a default wild card to Mask if it needs it}
  begin
    if HasWildCards(Mask) then
      AddWildCard := Mask
    else
      AddWildCard := AddFilePath(Mask, '*.*');
  end;

  function FindFiles(SearchMask : string; FileAttr : Byte) : Word;
    {-Add any matched files to File arrays}
  begin
    FindFirst(SearchMask, FileAttr, Frec);
    while (DosError = 0) and (NumFiles < MaxNumFiles) do begin
      with Frec do
        if (Attr and Directory) = (FileAttr and Directory) then
          {Matches directory type}
          if Name <> '.' then begin
            Inc(NumFiles);
            if Attr and Directory <> 0 then
              F^[NumFiles] := Name+'\'
            else
              F^[NumFiles] := Name;
          end;
      FindNext(Frec);
    end;
    case DosError of
      3, 18 : FindFiles := 0;
    else
      FindFiles := DosError;
    end;
  end;

  procedure SwapItem(I, J : Word);
    {-Swap two sort items}
  var
    TmpF : FileString;
  begin
    TmpF := F^[J];
    F^[J] := F^[I];
    F^[I] := TmpF;
  end;

  procedure ShellSort(NumFiles : Word);
    {-Shellsort the directory entries}
  var
    Offset, I, J, K : Word;
    InOrder : Boolean;
  begin
    Offset := NumFiles;
    while Offset > 1 do begin
      Offset := Offset shr 1;
      K := NumFiles-Offset;
      repeat
        InOrder := True;
        for J := 1 to K do begin
          I := J+Offset;
          if F^[I] < F^[J] then begin
            SwapItem(I, J);
            InOrder := False;
          end;
        end;
      until InOrder;
    end;
  end;

  {$F+}
  function SendFileName(Item : Word) : string;
    {-Pass each file name to the pick unit}
  var
    S : string;
  begin
    S := ' '+F^[Item];
    if AltPathColor then
      PickAttr := (S[Length(S)] = '\');
    SendFileName := S;
  end;
  {$F-}

  function GetFileName(Mask : string; var FileName : string) : Word;
    {-Get a filename from a user mask}
  label
    ExitPoint;
  var
    PickChar : Char;
    Done : Boolean;
    XHigh : Byte;
    Choice : Word;
    Status : Word;
    Memory : LongInt;
    VA : DirColorArray;
    SaveN, SaveH : Byte;
    SearchMask : string;
    PathName : string;
    WildCard : FileString;
  begin

    {Assume success}
    GetFileName := 0;
    FileName := '';

    {Get the default searchmask}
    Mask := StUpcase(Mask);
    SearchMask := AddWildCard(Mask);

    {See if mask specifies a subdirectory}
    if (Length(Mask) <> 0) and not HasWildCards(Mask) then begin
      FindFirst(SearchMask, FileAttr, Frec);
      case DosError of
        0 : ;                {Files found, it is a subdirectory}
        3 :                  {Path not found, invalid subdirectory}
          begin
            {See if Mask itself is a valid path}
            FindFirst(Mask, FileAttr, Frec);
            case DosError of
              3 : GetFileName := 1; {Path not found}
            else
              FileName := Mask; {New or existing file}
            end;
            Exit;
          end;
        18 :                 {No more files, not a subdirectory}
          begin
            case Mask[Length(Mask)] of
              ':', '\' : GetFileName := 2; {No matching files}
            else
              FileName := Mask; {New or existing file}
            end;
            Exit;
          end;
      else
        GetFileName := DosError; {DOS critical error}
        Exit;
      end;
    end;

    {Initialize display colors}
    case LastMode and $FF of
      0, 2, 7 : VA := DirMonocColors;
      1, 3 : VA := DirColorColors;
    else
      {Unsupported video mode}
      GetFileName := 3;
      Exit;
    end;

    {Get space for file array - reserve 2000 bytes for popup window}
    Memory := MaxAvail-2000;
    if Memory > MaxFiles*SizeOf(FileString) then
      {Room for MaxFiles}
      MaxNumFiles := MaxFiles
    else begin
      {Limited space available}
      MaxNumFiles := Memory div SizeOf(FileString);
      if (Memory < 0) or (MaxNumFiles < 2) then begin
        GetFileName := 4;    {Insufficient memory}
        Exit;
      end;
    end;
    GetMem(F, MaxNumFiles*SizeOf(FileString));

    Done := False;
    repeat

      {Separate wildcard from pathname}
      WildCard := JustFilename(SearchMask);
      PathName := Copy(SearchMask, 1, Length(SearchMask)-Length(WildCard));

      {Build the file array}
      NumFiles := 0;
      {Find non-subdirectories}
      Status := FindFiles(SearchMask, FileAttr and not Directory);
      {Find subdirectories}
      if Status = 0 then
        if (FileAttr and Directory) <> 0 then
          Status := FindFiles(AddWildCard(PathName), FileAttr);
      if Status <> 0 then begin
        GetFileName := Status;
        goto ExitPoint;
      end;

      if NumFiles = 0 then begin
        {No files found}
        Done := True;
        GetFileName := 2;    {No matching files}

      end else begin
        {Sort the directory}
        ShellSort(NumFiles);

        {Choose the window width}
        if PickMatrix*SizeOf(FileString) >= Length(SearchMask)+3 then
          XHigh := XLow+PickMatrix*SizeOf(FileString)+2
        else
          XHigh := XLow+Length(SearchMask)+5;

        if XHigh > CurrentWidth then
          XHigh := CurrentWidth;

        {Pick from the directory}
        if AltPathColor then begin
          SaveN := PickAttrN;
          SaveH := PickAttrH;
          PickAttrN := VA[AltNormal];
          PickAttrH := VA[AltHigh];
        end;
        Choice := 1;
        if PickWindow(@SendFileName, NumFiles, XLow, YLow, XHigh, YHigh, True,
                      VA[WindowAttr], VA[FrameAttr], VA[HeaderAttr], VA[SelectAttr],
                      ' '+SearchMask+' ', [#13, #27], Choice, PickChar) then
          begin
            case PickChar of
              #27 :          {User pressed Escape - return empty file name}
                Done := True;
              #13 :          {User pressed Enter}
                if F^[Choice][Length(F^[Choice])] = '\' then begin
                  {Selected a subdirectory}
                  Mask := AddFilePath(PathName, F^[Choice]);
                  SearchMask := AddFilePath(Mask, WildCard);
                end else begin
                  {Not a directory}
                  FileName := AddFilePath(PathName, F^[Choice]);
                  Done := True;
                end;
            end;
        end else begin
          {Error occurred in PickWindow - most likely insufficient memory}
          GetFileName := 4;
          Done := True;
        end;
        if AltPathColor then begin
          PickAttrN := SaveN;
          PickAttrH := SaveH;
        end;
      end;
    until Done;

ExitPoint:
    {Free the memory space used for file array}
    FreeMem(F, MaxNumFiles*SizeOf(FileString));
  end;

end.
