{
Copyright (c) 1987 by TurboPower Software. May be freely used by and
distributed to owners of Turbo Professional 4.0.

Modified by Dan T. Davis, January 1988.

See TPDir for an example of using the TPPick unit.
}

{$R-,I-,S-,V-}

unit TpPick;
  {-Manage scrolling pick windows}
  {{}
interface

uses
  TPString,
  TPCrt,
  TPWindow;

const
  PickAttr : Boolean = False; {If True, special color attributes used to pick item}
  PickAttrN : Byte = $00;    {Special Color Attribute; normal color}
  PickAttrH : Byte = $00;    {Special Color Attribute; highlight color}
  HideCursor : Boolean = True; {False to leave hardware cursor on screen}
  PickMinRows : Word = 0;    {We want at least this many rows in PickWindow}
  PickMaxRows : Word = 9999; {We want at most this many rows in PickWindow}
  PickMatrix : Byte = 1;     {Number of Horizontal Fields in window}
  PickStick : Boolean = True; {Get "stuck" at top/bottom on SCROLLING PickWindows?}

type
  CharSet = set of Char;

function PickWindow
  (StringFunc : Pointer;     {Pointer to function to return each item string}
   NumItems : Word;          {Number of items to pick from}
   XLow, YLow : Byte;        {Window coordinates, including frame if any}
   XHigh, YHigh : Byte;      {Window coordinates, including frame if any}
   DrawFrame : Boolean;      {True to draw a frame around window}
   WindowAttr : Byte;        {Video attribute for body of window}
   FrameAttr : Byte;         {Video attribute for frame}
   HeaderAttr : Byte;        {Video attribute for header}
   SelectAttr : Byte;        {Video attribute for selected item}
   Header : string;          {Title for window}
   PickSet : CharSet;        {Selection characters}
   var Choice : Word;        {The item selected, in the range 1..NumItems}
   var PickChar : Char       {Character used to perform selection}
   ) : Boolean;              {True if PickWindow was successful}
  {-Display a window, let user scroll around in it, and return choice.
    Choice returned is in the range 1..NumItems.
    PickChar is an element of PickSet.}

procedure FillPickWindow
  (W : WindowPtr;            {Which window to display pick list}
   StringFunc : Pointer;     {Pointer to function to return each item string}
   Choice : Word;            {Choice,row tell how the items should be drawn}
   Row : Word;               {           in a manner consistent with PickBar}
   NumItems : Word);         {Number of items in PickArray}
  {-Display a window, fill it with choices, and return.
    Choice specifies the initial item highlighted.}

procedure PickBar
  (W : WindowPtr;            {The window to operate in}
   StringFunc : Pointer;     {Pointer to function to return items}
   var Choice : Word;        {The item selected, range 1..numitems}
   var Row : Word;           {The row to draw the bar on}
   NumItems : Word;          {The number of items to pick from}
   SelectAttr : Byte;        {Video attribute for bars}
   PickSet : CharSet;        {Selection Characters}
   var PickChar : Char;      {Character used to perform selection}
   EraseBar : Boolean);      {Should we recolor the bar when finished?}
  {-Choose from a pick list already displayed on the screen}

  {=========================================================================}

  (*}*)
implementation

var
  XSize : Word;              {Active width of pick window (no frame)}
  YSize : Word;              {Active height of pick window}
  PickFunc : Pointer;        {Pointer to function that returns each string}

  Items : Word;              {Total Items being considered}
  ItemWidth : Byte;          {Maximum width for an item}
  ItemOffSet : Word;         {Offset Between Item Numbers in different Columns}
  ItemWrap : Boolean;        {Wrap when hitting PickWindow periphery}

  procedure Lower(var Source : Word; MaxVal : Word);
  begin
    if Source > MaxVal then
      Source := MaxVal;
  end;

  function InitPickVars
    (W : WindowPtr;
     NumItems : Word;
     StringFunc : Pointer) : Boolean;
    {-Initialize variables we'll use for display}
  begin
    InitPickVars := True;

    {Make sure the window is on screen; if it is, ASSUME that}
    {the user has made it the top window with SETTOPWINDOW or SELECTWINDOW}
    {Otherwise, assume the user wants us to turn it on}
    if DisplayWindow(W) then {we had to turn it on} ;

    if W <> nil then
      with WindowP(W)^ do begin
        XSize := Succ(XH-XL);
        YSize := Succ(YH-YL);
    end else ;
    { you should have already set XSize and YSize }

    Items := NumItems;
    ItemWidth := XSize div PickMatrix;

    Lower(YSize, Items);
    Lower(YSize, PickMaxRows);

    ItemOffSet := (Items+Pred(PickMatrix)) div PickMatrix;
    if ItemOffSet < PickMinRows then
      ItemOffSet := PickMinRows;
    Lower(ItemOffSet, Items);

    Lower(YSize, ItemOffSet);

    ItemWrap := (YSize = ItemOffSet) or not PickStick;

    PickFunc := StringFunc;

    {Validate item information}
    if (YSize = 0) or (PickFunc = nil) then
      InitPickVars := False;

    {Were we able to show the window?}
    if (W <> nil) and not WindowP(W)^.Active then
      InitPickVars := False;
  end;

  procedure ReCalc(var Row : Word; var Choice : Word; var Top : Word);
  var
    I : Integer;
  begin
    {make sure that we are asking for a valid Choice/Row combination}
    I := Succ(Pred(Choice) mod ItemOffSet);
    Lower(Row, I);
    Lower(Row, YSize);
    I := YSize-(ItemOffSet-I);
    if Row < I then
      Row := I;
    if Row < 1 then
      Row := 1;
    Top := Succ((Choice-Row) mod ItemOffSet);
  end;

  {{}
  function GetString(Item : Word) : string;
    {-Return the name of each item}
    inline($FF/$1E/>PickFunc); {CALL DWORD PTR [>PickFunc]}
  (*}*)

  procedure DrawItem(ItemNum : Word; Row, Col, Attr : Byte; HiLi : Boolean);
    {-Draw the specified item}
  var
    S : string;
  begin
    if ItemNum <= Items then
      S := GetString(ItemNum)
    else
      S := '';
    if Length(S) <= ItemWidth then
      S := Pad(S, ItemWidth)
    else
      S[0] := Chr(ItemWidth);
    if PickAttr then begin
      if HiLi then
        FastWriteWindow(S, Row, Col, PickAttrH)
      else
        FastWriteWindow(S, Row, Col, PickAttrN);
      PickAttr := False;
    end else
      FastWriteWindow(S, Row, Col, Attr);
  end;

  procedure DrawPage(Top : Word; Attr : Byte);
    {-Draw a full page of items, with Choice shown on Row}
  var
    I, J, BeforeTop, AtCol : Word;
  begin
    for I := 0 to Pred(PickMatrix) do begin
      AtCol := Succ(I*ItemWidth);
      BeforeTop := Pred(Top+I*ItemOffSet);
      for J := 1 to YSize do
        DrawItem(J+BeforeTop, J, AtCol, Attr, False);
    end;
  end;

  procedure PickBar
    (W : WindowPtr;          {The window to operate in}
     StringFunc : Pointer;   {Pointer to function to return items}
     var Choice : Word;      {The item selected, range 1..numitems}
     var Row : Word;         {The row to draw the bar on}
     NumItems : Word;        {The number of items to pick from}
     SelectAttr : Byte;      {Video attribute for bars}
     PickSet : CharSet;      {Selection Characters}
     var PickChar : Char;    {Character used to perform selection}
     EraseBar : Boolean);    {Should we recolor the bar when finished?}
  var
    SaveBreak : Boolean;
    XY, CursorScanLines : Word;
    Done : Boolean;
    KW : Word;
    Top, PrevTop, PrevChoice, PrevRow : Word;
    Column : Word;
    AtLoc : Word;
    MoveMax : Word;

  begin
    if not InitPickVars(W, NumItems, StringFunc) then
      Exit;

    { Initialize PrevTop to make sure we draw page initially }
    PrevTop := 0;

    with WindowP(W)^ do begin

      GetCursorState(XY, CursorScanLines);
      if HideCursor then
        HiddenCursor
      else
        NormalCursor;
      SaveBreak := CheckBreak;
      CheckBreak := False;

      {Loop getting characters}
      Done := False;
      repeat

        {Check to see if we need to redraw the page or erase the bar}
        ReCalc(Row, Choice, Top);
        if PrevTop <> Top then
          DrawPage(Top, WAttr)
        else if (PrevChoice <> Choice) or (PrevRow <> Row) then
          DrawItem(PrevChoice, PrevRow, Column, WAttr, False);

        PrevTop := Top;
        PrevChoice := Choice;
        PrevRow := Row;
        Column := Succ((Pred(Choice) div ItemOffSet)*ItemWidth);

        {Highlight the selected entry}
        DrawItem(Choice, Row, Column, SelectAttr, True);

        GoToXY(Column, Row);

        {Find our relative Location in the PickList}
        AtLoc := Succ(Pred(Choice) mod ItemOffSet);

        {Get a command}
        KW := ReadKeyWord;

        {See if a pick character first}
        PickChar := Char(lo(KW));
        if PickChar = #0 then
          PickChar := Char(hi(KW) or $80);
        if PickChar in PickSet then
          Done := True;

        if not Done then
          case KW of
            $4700 :          {Home}
              Choice := 1;

            $4800 :          {Up arrow}
              if AtLoc <> 1 then begin
                {Move to previous item}
                Dec(Choice);
                {Move selection bar}
                Dec(Row);
              end else if ItemWrap then begin
                {Wrap to previous column, if any}
                if Choice > 1 then
                  Dec(Choice)
                else
                  Choice := Items;
                Row := YSize;
              end;

            $4900 :          {PgUp}
              begin
                MoveMax := Pred(AtLoc);
                Lower(MoveMax, YSize);
                if MoveMax > 0 then
                  Dec(Choice, MoveMax)
                else if ItemWrap then begin
                  {Wrap to previous column, if any}
                  if Choice > 1 then
                    Dec(Choice)
                  else
                    Choice := Items;
                  Row := YSize;
                end;
              end;

            $4B00 :          {Left Arrow}
              begin
                if Choice > ItemOffSet then
                  Dec(Choice, ItemOffSet)
                else if Choice > 1 then begin
                  Choice := Pred(Choice)+Pred(PickMatrix)*ItemOffSet;
                  Dec(Row);
                end else if ItemWrap then
                  Choice := PickMatrix*ItemOffSet;
                if Choice > Items then
                  repeat Dec(Choice, ItemOffSet) until Choice <= Items;
              end;

            $4D00 :          {Right Arrow}
              if Choice <= Items-ItemOffSet then
                Inc(Choice, ItemOffSet)
              else if AtLoc <> ItemOffSet then begin
                Choice := Succ(AtLoc);
                Inc(Row);
              end else if ItemWrap then
                Choice := 1;

            $4F00 :          {End}
              begin
                Choice := Items;
                Row := 1;
              end;

            $5000 :          {Down arrow}
              if (AtLoc <> ItemOffSet) and (Choice < Items) then begin
                {Move to next item}
                Inc(Choice);
                {Move selection bar}
                Inc(Row);
              end else if ItemWrap then begin
                {Wrap to next column, if any}
                if Choice < Items then
                  Inc(Choice)
                else
                  Choice := 1;
                Row := 1;
              end else if (Choice = Items) then
                Dec(Row);

            $5100 :          {PgDn}
              begin
                MoveMax := ItemOffSet-AtLoc;
                Lower(MoveMax, YSize);
                if (MoveMax > 0) and (Choice < Items) then begin
                  Inc(Choice, MoveMax);
                  if Choice > Items then begin
                    Choice := Items;
                    Row := 1;
                  end;
                end else if ItemWrap then begin
                  {Wrap to next column, if any}
                  if Choice < Items then
                    Inc(Choice)
                  else
                    Choice := 1;
                  Row := 1;
                end;
              end;
          end;
      until Done;
      if EraseBar then
        DrawItem(Choice, Row, Column, WAttr, False);
      CheckBreak := SaveBreak;
      RestoreCursorState(XY, CursorScanLines);
    end;
  end;

  function PickWindow
    (StringFunc : Pointer;   {Pointer to function to return each item string}
     NumItems : Word;        {Number of items in PickArray}
     XLow, YLow : Byte;      {Window coordinates, including frame if any}
     XHigh, YHigh : Byte;    {Window coordinates, including frame if any}
     DrawFrame : Boolean;    {True to draw a frame around window}
     WindowAttr : Byte;      {Video attribute for body of window}
     FrameAttr : Byte;       {Video attribute for frame}
     HeaderAttr : Byte;      {Video attribute for header}
     SelectAttr : Byte;      {Video attribute for selected item}
     Header : string;        {Title for window}
     PickSet : CharSet;      {Selection characters}
     var Choice : Word;      {The item selected, in the range 1..NumItems}
     var PickChar : Char     {Character used to perform selection}
     ) : Boolean;            {True if PickWindow was successful}
  var
    Correction : Integer;
    Row : Word;
    W : WindowPtr;

  begin

    {Assume failure}
    PickWindow := False;

    {Get a Value for YHigh}
    if DrawFrame then
      Correction := -1
    else
      Correction := +1;
    XSize := XHigh-XLow+Correction;
    YSize := YHigh-YLow+Correction;
    if not InitPickVars(nil, NumItems, StringFunc) then
      Exit;
    if YSize >= PickMinRows then
      YHigh := YLow+YSize-Correction
    else
      YHigh := YLow+PickMinRows-Correction;

    {Initialize the window}
    if not MakeWindow(W, XLow, YLow, XHigh, YHigh,
                      DrawFrame, True, False,
                      WindowAttr, FrameAttr, HeaderAttr,
                      Header) then Exit;

    if not InitPickVars(W, NumItems, StringFunc) then
      Exit;

    {Initial item is the one we say if legal}
    if (Choice < 1) or (Choice > NumItems) then
      Choice := 1;
    Row := Choice;
    PickBar(W, PickFunc, Choice, Row, Items,
            SelectAttr, PickSet, PickChar, False);

    {Restore the screen and deallocate the window}
    W := EraseTopWindow;
    DisposeWindow(W);

    {If we get to here, all was well}
    PickWindow := True;
  end;

  procedure FillPickWindow
    (W : WindowPtr;          {Which window to display pick list}
     StringFunc : Pointer;   {Pointer to function to return each item string}
     Choice : Word;          {Choice,row tell how the items should be drawn}
     Row : Word;             {           in a manner consistent with PickBar}
     NumItems : Word);       {Number of items in PickArray}
  var
    Top : Word;
  begin
    if not InitPickVars(W, NumItems, StringFunc) then
      Exit;

    ReCalc(Row, Choice, Top);
    DrawPage(Top, WindowP(W)^.WAttr);
  end;

end.

