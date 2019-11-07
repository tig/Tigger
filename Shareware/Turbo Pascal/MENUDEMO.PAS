Program Demo;

uses Tpcrt, tpmenu;

var ch :char;
    main : menu;
    key : menukey;


  procedure InitMenu (var M :menu);
  const
       Fcolor  = $1F;
       HColor  = $1A;
       BColor  = $1E;
       SColor  = $5E;
       HiColor = $1B;
       HelpColor = $0C;

       Color1  : MenuColorArray = (Fcolor, HColor, BCOlor, SColor,HiColor,HelpCOlor);
       Frame1  : FrameArray = LotusFrame;
       Frame2  : FrameArray = #218#192#191#217#196#179;


     begin
       M := NewMenu ([],nil);
       SubMenu(10,5,25, Vertical, Frame2, Color1, 'Options Menu');
         MenuItem ('First       ', 1,1,1,'Help for First');
         MenuItem ('Alpha', 2,1,2,'Help For Alpha');
         MenuItem ('Beta', 3,1,3,'Help For Beta');
         MenuItem ('Second',4,1,4,'Help for Second');
         MenuItem ('Gamma', 5,1,5,'help For Gamma');
         MenuItem ('Delta',6,1,6,'Help For Delta. Setect this to quit.');
         PopSubLevel;
       ResetMenu(M);
     end;

begin
  TextColor (White);
  TextBackground (black);
  ClrScr;
  InitMenu (Main);
  ClrScr;
  writeln (1);
  writeln (2);
  repeat
    Key := menuChoice (Main,Ch);
    GotoXY (1,24);
    Write ('Selection=', Key, ' with keystroke code of ', Ord(ch));
    EraseMenu (Main, False);
  Until (Ch = ^M) and (key = 6) or (ch = #27);
end.