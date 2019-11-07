PROGRAM HPHardCopy_Test;
{
  This simple routine tests the HP LaserJet HPHardCopy procedure.
  It assumes:
    a) the .BGI drivers are under the subdirectory specified in the
       InitGraph call;
    b) the LaserJet is on LPT1; this is set in the runtime initialization
       of the HPCopy unit;
    c) the default aspect ratio is assumed; it turns out for a VGA and
       output to a LaserJet at 100 dpi, this works fine. Otherwise,
       experiment for yourself until satisfied.

  For more info, etc, see the comments in HPCOPY.PAS;
}

USES Graph,HPCopy;

VAR
  GraphDriver,GraphMode,ErrorCode :INTEGER;

BEGIN
  GraphDriver := Detect; { use autodetect }
  InitGraph(GraphDriver, GraphMode, 'C:\TP');
  ErrorCode := GraphResult;
  IF ErrorCode <> grOK THEN
  BEGIN
    WRITELN('Graphics error: ', GraphErrorMsg(ErrorCode));
    HALT(1);
  END;

  Circle(GetMaxX DIV 2,GetMaxY DIV 2, GetMaxY DIV 3);
  OutTextXY(1,1,' This should look like a circle. Hardcopy happens now to LPT1...');

  { the SetViewPort is necessary to get hard copy of the entire screen;
    if you have changed the ViewPort, that is...Otherwise you get a
    hardcopy of just the current viewport. In this example this call is
    unecessary because the whole screen in the ViewPort by default. }
  SetViewPort(0,0,GetMaxX,GetMaxY,ClipOff);

  HPHardCopy;

  READLN;
  CloseGraph;
END.