{$U+,V-}
Program SetTime;

{*****************************************************************************
*                                                                            *
*                                                                            *
* NAME: SETTIME.PAS                                                          *
*                                                                            *
* TITLE: Set DOS Time and Date from US Naval Obseratory's Master Clock       *
*                                                                            *
* STATUS: Version 1, Release 0                                               *
*                                                                            *
* FUNCTION: Dial the US Naval Observatory's Master Clock and set the DOS     *
*           time and date with the values transmitted by the Master Clock.   *
*                                                                            *
* AUTHOR: Dave King                                                          *
*                                                                            *
* DATE: 8/30/87                                                              *
*                                                                            *
* DESCRIPTION: This program dials the US Naval Observatory's Master          *
*              Clock in Washington, D.C., at (202) 653-0351 using a modem    *
*              that uses either the Attention Command Set or the IBM         *
*              Command Set.  Once connection is made to the Master Clock the *
*              program reads the date and time values transmitted by the     *
*              Master Clock.  These values are converted to formats usable   *
*              by DOS, the date is converted from Julian format to month,    *
*              day and year format, the time is converted from GMT to local  *
*              time.  The DOS time and date are then set using the DOS BIOS  *
*              interrupts.                                                   *
*                                                                            *
*              The program will retry, up to 3 times, to dial and connect    *
*              to the Master Clock if connection is not made initially (i.e. *
*              if the line is busy, etc.)                                    *
*                                                                            *
*              A profile is used to provide various parameters required by   *
*              the program.  These include:                                  *
*                                                                            *
*              - The modem command set to be used, IBM or AT.                *
*                                                                            *
*              - A prefix to be added to the number dialed, allowing         *
*                support for long distance dialing or "dial 9 for outside    *
*                line" situations.                                           *
*                                                                            *
*              - The year.  This value is not supplied by the Master Clock   *
*                and must be supplied by the user.                           *
*                                                                            *
*              - A value that indicates the difference between GMT time and  *
*                local time.  This value is subtracted from the GMT time     *
*                value to set the correct hour.                              *
*                                                                            *
*              - A value that indicates whether or not Daylight Savings Time *
*                is currently in effect or not.                              *
*                                                                            *
*              - The COM port at which the modem is to be addressed.         *
*                                                                            *
*              - A value that determines whether of not the program types    *
*                it's progress messages to the screen.                       *
*                                                                            *
* INPUT:  The following inputs are used by the program:                      *
*                                                                            *
*         - The profile file described above, which must be present in the   *
*           current subdirectory on the current drive, named SETTIME.PRO.    *
*           If this profile is not available then a set of default values    *
*           are used by the program.                                         *
*                                                                            *
*         - The values returned by the modem during the dial/connection      *
*           process.                                                         *
*                                                                            *
*         - The values for the time and date supplied by the Master Clock    *
*           via the modem connection.                                        *
*                                                                            *
* OUTPUT:  The following outputs are produced by the program:                *
*                                                                            *
*          - New values for the DOS time and date are set using the          *
*            standard BIOS interrupts.                                       *
*                                                                            *
*          - Various progress messages are typed on the screen, unless the   *
*            user has turned this function off via the program profile.      *
*                                                                            *
* MODULE TYPE: COM program                                                   *
*                                                                            *
*    PROCESSOR: Turbo Pascal 3.01A                                           *
*                                                                            *
*    MODULE SIZE: 20352 bytes                                                *
*                                                                            *
*    ATTRIBUTES:                                                             *
*                                                                            *
* ENTRY POINTS:                                                              *
*                                                                            *
* EXITS NORMAL:                                                              *
*                                                                            *
* EXITS ERROR:                                                               *
*      Message ID    RC                                                      *
*                                                                            *
* EXTERNAL REFERENCES:                                                       *
*      Macro's:                                                              *
*                                                                            *
*      Includes:                                                             *
*                                                                            *
* NOTES:                                                                     *
*                                                                            *
*****************************************************************************}

  Type
     Str80           = String[80];
     { For TurboStr Functions }
     turbostrtype    = STRING[255];
     turbostrregpack = RECORD ax,bx,cx,dx,bp,si,di,ds,es,flags:INTEGER;END;

  Var
     { Program profile input }
     profile       : Text;
     command_set   : Str80;
     comport       : Str80;
     prefix        : Str80;
     year          : Str80;
     time_zone     : Str80;
     daylight      : Str80;

     keystring     : Str80;
     value         : Str80;

     { Modem control }
     init_string   : Str80;
     dial_string   : Str80;
     hangup_string : Str80;
     reset_string  : Str80;

     { Screen output stuff }
     bar           : String[66];
     fore_color    : Integer;
     back_color    : Integer;
     colon_1       : String[2];
     colon_2       : String[2];
     write_string  : Str80;

     i             : Integer;
     error         : Integer;
     retry_counter : Integer;
     done          : Boolean;

     com           : Integer;
     shutup        : Boolean;

     instring      : Str80;

     yy            : Integer;
     tzone         : Integer;
     tdaylight     : Integer;
     time_diff     : Integer;

  Const
     { Name of profile file }
     filename      : String[14] = 'SETTIME.PRO';

     { Modem stuff }
     attn          : String[3]  = '+++';
     ibm_attn      : Char       = ^N;

     blank         : Str80      = ' ';

{*****************************************************************************
*                                                                            *
*  Control of COM port procedures from Borland's Turbo Pascal Tutor book.    *
*  These procedures are public domain.                                       *
*                                                                            *
*****************************************************************************}
  { Internal use only definitions, these variables should not be used by }
  { user programs.                                                       }

  Type
     __RegisterSet=Record case Integer of
                      1: (AX,BX,CX,DX,BP,DI,SE,DS,ES,Flags: Integer);
                      2: (AL,AH,BL,BH,CL,CH,DL,DH: Byte);
                   end;
     __ParityType=(None,Even,Odd);
     String19 = String[19];

  var
     __Regs: __RegisterSet;
     InError,OutError: Array [1..2] of Byte;

  procedure __Int14(PortNumber,Command,Parameter: Integer);
  { do a BIOS COM driver interrupt }

    begin
      with __Regs do
       begin
        DX:=PortNumber-1;
        AH:=Command;
        AL:=Parameter;
        Flags:=0;
        Intr($14,__Regs);
       end;
    end;


  procedure SetSerial(PortNumber,BaudRate,StopBits,DataBits: Integer;
                      Parity: __ParityType);
  { Set serial parameters on a COM port }

    var
      Parameter: Integer;

    begin
      case BaudRate of
        110: BaudRate:=0;
        150: BaudRate:=1;
        300: BaudRate:=2;
        600: BaudRate:=3;
        1200: BaudRate:=4;
        2400: BaudRate:=5;
        4800: BaudRate:=6;
        else BaudRate:=7; { Default to 9600 baud }
       end;
      if StopBits=2 then StopBits:=1
      else StopBits:=0; { Default to 1 stop bit }
      if DataBits=7 then DataBits:=2
      else DataBits:=3; { Default to 8 data bits }
      Parameter:=(BaudRate Shl 5)+(StopBits Shl 2)+DataBits;
      case Parity of
        Odd: Parameter:=Parameter+8;
        Even: Parameter:=Parameter+24;
        else; { Default to no parity }
       end;
      __Int14(PortNumber,0,Parameter);
    end;


  Function SerialStatus(PortNumber: Integer): Integer;
  { Return the status of a COM port }

  Begin
    __Int14(PortNumber,3,0);
    SerialStatus := __Regs.AX;
  End;


  procedure __OutPort1(C: Byte);
  { Called by Write to Aux or Usr when assigned to COM1 }

    begin
      while (SerialStatus(1) and $30)=0 do ;
      __Int14(1,1,C);
      OutError[1]:=OutError[1] Or (__Regs.AH and $8E);
    end;


  procedure __OutPort2(C: Byte);
  { Called by Write to Aux or Usr when assigned to COM2 }

    begin
      while (SerialStatuS(2) and $30)=0 do ;
      __Int14(2,1,C);
      OutError[2]:=OutError[2] Or (__Regs.AH and $8E);
    end;


  Function __InPort1: Char;
  { Called by Read from Aux or Usr when assigned to COM1 }

    begin
      __Int14(1,2,0);
      __InPort1:=Chr(__Regs.AL);
      InError[1]:=InError[1] Or (__Regs.AH and $8E);
    end;


  Function __InPort2: Char;
  { Called by Read from Aux or Usr when assigned to COM2 }

    begin
      __Int14(2,2,0);
      __InPort2:=Chr(__Regs.AL);
      InError[2]:=InError[2] Or (__Regs.AH and $8E);
    end;


  procedure __AssignPort(PortNumber: Integer; var InPtr,OutPtr: Integer);
  { Assign either Aux or Usr to either COM1 or COM2 }

    begin
      if PortNumber=2 then
       begin
        OutPtr:=Ofs(__OutPort2);
        InPtr:=Ofs(__InPort2);
       end
      else { Default to port 1 }
       begin
        OutPtr:=Ofs(__OutPort1);
        InPtr:=Ofs(__InPort1);
       end;
      InError[PortNumber]:=0;
      OutError[PortNumber]:=0;
    end;

  procedure AssignUsr(PortNumber: Integer);
  { Assign Usr to either COM1 or COM2 }

    begin
      __AssignPort(PortNumber,UsrInPtr,UsrOutPtr);
    end;

  Function Binary(V: Integer): String19;

    var
      I: Integer;
      B: Array [0..3] of String[4];

    begin
      For I:=0 To 15 do
        if (V and (1 Shl (15-I)))<>0 then B[I Div 4][(I Mod 4)+1]:='1'
        else B[I Div 4][(I Mod 4)+1]:='0';
      For I:=0 To 3 do B[I][0]:=Chr(4);
      Binary:=B[0]+' '+B[1]+' '+B[2]+' '+B[3];
    end;

{******************************************************************************
*                                                                             *
*  That's the end of the public domain code, the rest of this is stuff I      *
*                                                                             *
******************************************************************************}

  { STRIP function from TURBOSTR.LIB by Timothy J. Slegel }
  FUNCTION Strip(str1:turbostrtype;ch:CHAR):turbostrtype;
    BEGIN
      INLINE(
                       {; STRIP function}
                       {;}
       $1E             {              push ds}
      /$16             {              push ss}
      /$16             {              push ss}
      /$1F             {              pop ds}
      /$07             {              pop es}
      /$8D/$BE/>STR1   {              lea di,[bp+>str1]}
      /$8D/$B6/$06/$01 {              lea si,[bp+$0106]}
      /$8A/$86/>CH     {              mov al,[bp+>ch]}
      /$3C/$54         {              cmp al,'T'}
      /$75/$04         {              jne stpfunc1}
      /$B4/$01         {              mov ah,1}
      /$EB/$1A         {              jmp short stpfuncfound}
      /$3C/$74         {stpfunc1:     cmp al,'t'}
      /$75/$04         {              jne stpfunc2}
      /$B4/$01         {              mov ah,1}
      /$EB/$12         {              jmp short stpfuncfound}
      /$3C/$4C         {stpfunc2:     cmp al,'L'}
      /$75/$04         {              jne stpfunc3}
      /$B4/$02         {              mov ah,2}
      /$EB/$0A         {              jmp short stpfuncfound}
      /$3C/$6C         {stpfunc3:     cmp al,'l'}
      /$75/$04         {              jne stpfunc4}
      /$B4/$02         {              mov ah,2}
      /$EB/$02         {              jmp short stpfuncfound}
      /$B4/$03         {stpfunc4:     mov ah,3}
      /$B0/$20         {stpfuncfound: mov al,' '}
      /$8A/$0D         {              mov cl,[di]}
      /$30/$ED         {              xor ch,ch}
      /$80/$F9/$00     {              cmp cl,0}
      /$74/$40         {              je stpallblank}
      /$F6/$C4/$02     {              test ah,2}
      /$74/$28         {              jz stpright}
      /$FC             {              cld}
      /$47             {              inc di}
      /$F3/$AE         {              repe scasb}
      /$74/$35         {              je stpallblank}
      /$41             {              inc cx}
      /$4F             {              dec di}
      /$F6/$C4/$01     {              test ah,1}
      /$74/$12         {              jz stpleft}
      /$01/$CF         {              add di,cx}
      /$4F             {              dec di}
      /$FD             {              std}
      /$F3/$AE         {              repe scasb}
      /$41             {              inc cx}
      /$47             {              inc di}
      /$87/$FE         {              xchg si,di}
      /$88/$0D         {              mov [di],cl}
      /$01/$CF         {              add di,cx}
      /$F2/$A4         {              rep movsb}
      /$EB/$1F         {              jmp short stpdone}
      /$87/$FE         {stpleft:      xchg si,di}
      /$88/$0D         {              mov [di],cl}
      /$47             {              inc di}
      /$F2/$A4         {              rep movsb}
      /$EB/$16         {              jmp short stpdone}
      /$01/$CF         {stpright:     add di,cx}
      /$FD             {              std}
      /$F3/$AE         {              repe scasb}
      /$74/$0C         {              je stpallblank}
      /$47             {              inc di}
      /$87/$FE         {              xchg si,di}
      /$41             {              inc cx}
      /$88/$0D         {              mov [di],cl}
      /$01/$CF         {              add di,cx}
      /$F2/$A4         {              rep movsb}
      /$EB/$03         {              jmp short stpdone}
      /$C6/$04/$00     {stpallblank:  mov byte [si],0}
      /$1F             {stpdone:      pop ds}
      );
    END;

  { TRANSLATE function from TURBOSTR.LIB by Timothy J. Slegel }
  FUNCTION Translate(str1:turbostrtype):turbostrtype;
    BEGIN
      INLINE(
                       {; TRANSLATE function}
       $1E             {            push ds}
      /$16             {            push ss}
      /$16             {            push ss}
      /$07             {            pop es}
      /$1F             {            pop ds}
      /$8D/$B6/>STR1   {            lea si,[bp+>str1]}
      /$8D/$BE/$04/$01 {            lea di,[bp+$0104]}
      /$8A/$0C         {            mov cl,[si]}
      /$88/$0D         {            mov [di],cl}
      /$30/$ED         {            xor ch,ch}
      /$46             {            inc si}
      /$47             {            inc di}
      /$FC             {            cld}
      /$41             {            inc cx}
      /$49             {loop1:      dec cx}
      /$74/$0E         {            jz  leave1}
      /$AC             {            lodsb}
      /$3C/$61         {            cmp al,'a'}
      /$72/$06         {            jb  cont1}
      /$3C/$7A         {            cmp al,'z'}
      /$77/$02         {            ja  cont1}
      /$2C/$20         {            sub al,$20}
      /$AA             {cont1:      stosb}
      /$EB/$EF         {            jmp loop1}
      /$1F             {leave1:     pop ds}
      );
    END;

  { DOS call to set the system date }
  Procedure Set_Date(Var mo,dd,yy :Integer);
    Type regpack = Record
                     AX,BX,CX,DX,BP,SI,DI,DS,ES,FLAGS: Integer;
                   End;
    Var registers: regpack;

    Begin
      registers.AX := $2B00;   { The set date DOS interrupt }
      With registers Do        { Load date into registers   }
        Begin
          CX := yy;
          DX := mo;
          DX := Swap(DX) + dd;
        End;

      Intr($21,registers);     { Call DOS }

    End;

  { DOS call to set the time }
  Procedure Set_Time(Var hh,mm,ss :Integer);
    Type regpack = Record
                     AX,BX,CX,DX,BP,SI,DI,DS,ES,FLAGS: Integer;
                   End;
    Var registers: regpack;

    Begin
      registers.AX := $2D00;   { The set time DOS interrupt }
      With registers Do        { Load time into registers   }
        Begin
          CX := hh;
          CX := Swap(CX) + mm;
          DX := ss;
          DX := Swap(DX) + 0;
        End;

      Intr($21,registers);     { Call DOS }

    End;

  { Procedure to write all lines as 80 character strings, forces screen colors }
  Procedure Write80(Var input_string : Str80);

    Var
      i   : Integer;

    Begin
      For i := (Length(input_string) + 1) To 80 Do
        input_string[i] := ' ';
      input_string[0] := Chr(80);
      Write(input_string);
    End;

  {  This is the major work effort, read in the info and crunch it, then
     set the time and date }
  Procedure Process;
    Var
      pointer             :  Integer;
      temp                :  Str80;
      julien              :  Integer;
      mo,dd,hh,mm,ss      :  Integer;
      tmo,tdd,thh,tmm,tss :  String[2];
      tyy                 :  String[4];
      leap_year           :  Integer;

    Begin
      { Read the time and date from the Master Clock }
      Repeat
        Readln(Usr,instring);
      Until instring[1] = '4';
      Repeat
        Readln(Usr,instring);
      Until instring[1] = '4';

      { Is this a leap year ? }
      If yy mod 4 = 0 Then leap_year := 1
                      Else leap_year := 0;

      { Figure out the month and day (year is in the profile) }
      temp := Copy(instring,7,3);
      Val(temp,julien,error);

      { Crunch the time }
        { The hour }
      temp := Copy(instring,11,2);
      Val(temp,hh,error);
      hh := hh - time_diff;
        { If we are playing around the midnight time frame }
      If hh < 0 Then
        Begin
          hh := hh + 24;
          julien := julien - 1;
          If julien < 1 Then
            If leap_year = 1 Then julien := 364
                             Else julien := 365;
        End;

        { The minutes }
      temp := Copy(instring,13,2);
      Val(temp,mm,error);

        { The seconds }
      temp := Copy(instring,15,2);
      Val(temp,ss,error);

      { Set time }
      Set_Time(hh,mm,ss);

      { Now, figure out the month and day from the julien date }
      If julien > (335 - leap_year) Then
        Begin
          mo := 12;
          dd := julien - 335 - leap_year;
        End;
      If julien <= (334 - leap_year) Then
        Begin
          mo := 11;
          dd := julien - 304 - leap_year;
        End;
      If julien <= (304 - leap_year) Then
        Begin
          mo := 10;
          dd := julien - 273 - leap_year;
        End;
      If julien <= (273 - leap_year) Then
        Begin
          mo := 9;
          dd := julien - 243 - leap_year;
        End;
      If julien <= (243 - leap_year) Then
        Begin
          mo := 8;
          dd := julien - 212 - leap_year;
        End;
      If julien <= (212 - leap_year) Then
        Begin
          mo := 7;
          dd := julien - 181 - leap_year;
        End;
      If julien <= (181 - leap_year) Then
        Begin
          mo := 6;
          dd := julien - 151 - leap_year;
        End;
      If julien <= (151 - leap_year) Then
        Begin
          mo := 5;
          dd := julien - 120 - leap_year;
        End;
      If julien <= (120 - leap_year) Then
        Begin
          mo := 4;
          dd := julien - 90 - leap_year;
        End;
      If julien <= (90 - leap_year) Then
        Begin
          mo := 3;
          dd := julien - 59 - leap_year;
        End;
      If julien <= (59 - leap_year) Then
        Begin
          mo := 2;
          dd := julien - 30 - leap_year;
        End;
      If julien <= 30 Then
        Begin
          mo := 1;
          dd := julien;
        End;

      { Set the date }
      Set_Date(mo,dd,yy);

      { Tell 'em what we did }
      If Not shutup Then
        Begin
          Write80(blank);
          Str(mo,tmo);
          Str(dd,tdd);
          Str(yy,tyy);
          If dd < 10 Then write_string := 'Date: ' + tmo + '/0' + tdd + '/' + tyy
                     Else write_string := 'Date: ' + tmo + '/' + tdd + '/' + tyy;
          Write80(write_string);
          If mm < 10 Then colon_1 := ':0'
                     Else colon_1 := ':';
          If ss < 10 Then colon_2 := ':0'
                     Else colon_2 := ':';
          Str(hh,thh);
          Str(mm,tmm);
          Str(ss,tss);
          write_string := 'Time: ' + thh + colon_1 + tmm + colon_2 + tss;
          Write80(write_string);
        End;

    End;

  { Save screen colors so we can reset them when we are done }
  Procedure SaveColor;

    Type
      regpack = Record ax,bx,cx,dx,bp,si,di,ds,es,flags : Integer;  End;
    Var
      regs    : regpack;

    Begin
      regs.ax := $0F00;
      Intr($10,regs);
      regs.ax := $0800;
      Intr($10,regs);
      fore_color := (Hi(regs.ax)) MOD 16;
      back_color := (HI(regs.ax)) DIV 16;
    End;

  { Restore the original screen colors }
  Procedure RestoreColor;

    Begin
      TextColor(fore_color);
      TextBackground(back_color);
      Write80(blank);
    End;

{******************************************************************************
*                                                                             *
*                                Main Program                                 *
*                                                                             *
******************************************************************************}

  Begin

    SaveColor;

    TextMode(C80);
    TextBackground(Black);
    TextColor(Red);

    { Set the default parameters ... }
    prefix := '';
    year := '1987';
    time_zone := '5';
    daylight := '0';
    comport := '1';

    { Read 'em from the profile ... }
    Assign(profile,filename);
    {$I-}  Reset(profile); {$I+}
    If Not (IOResult = 0)
      Then Begin
        Write80(blank);
        write_string := 'Could not find the profile file, ' + filename;
        Write80(write_string);
        write_string := 'Program defaults will be used';
        Write80(write_string);
      End
      Else Begin
        While (Not Eof(profile)) Do
          Begin
            Readln(profile,instring);
            If instring[1] <> '*' Then
              Begin
                i := 0;
                Repeat
                  i := i + 1;
                  keystring[i] := instring[i];
                Until instring[i] = '=';
                keystring[0] := Chr((i - 1));
                keystring := Translate(Strip(keystring,'B'));
                value := Translate(Copy(instring,i + 1,Length(instring) - i));
                If keystring = 'COMMAND SET'
                  Then command_set := Strip(value,'B')
                  Else If keystring = 'PREFIX'
                         Then prefix := Strip(value,'B')
                         Else If keystring = 'YEAR'
                                Then year := Strip(value,'B')
                                Else If keystring = 'TIME ZONE'
                                       Then time_zone := Strip(value,'B')
                                       Else If keystring = 'DAYLIGHT'
                                              Then daylight := Strip(value,'B')
                                              Else { --+
                                                       |
                   +-----------------------------------+
                   | }
                   If keystring = 'COM'
                     Then comport := Strip(value,'B')
                     Else If keystring = 'SHUTUP'
                            Then Begin
                              If Strip(value,'B') = '1' Then shutup := True
                                                        Else shutup := False;
                            End
                            Else Begin
                              Write80(blank);
                              write_string := 'Invalid string, "' + instring + '" found in profile, "' + filename + '".';
                              Write80(write_string);
                            End;
              End;
          End;

          { Check for errors in profile values }
          If ((command_set <> 'AT') And (command_set <> 'IBM')) Then
            Begin
              Write80(blank);
              write_string := 'Invalid value for COMMAND SET entered in profile, "' + command_set + '".';
              Write80(write_string);
              write_string := 'AT command set will be used.';
              Write80(write_string);
              command_set := 'AT';
            End;
          Val(year,yy,error);
          If error <> 0 Then
            Begin
              Write80(blank);
              write_string := 'Invalid value for YEAR entered in profile, "' + year + '".';
              Write80(write_string);
              write_string := '"1987" will be used.';
              Write80(write_string);
              yy := 1987;
            End;
          Val(time_zone,tzone,error);
          If error <> 0 Then
            Begin
              Write80(blank);
              write_string := 'Invalid value for TIME ZONE entered in profile, "' + time_zone + '".';
              Write80(write_string);
              write_string := '"5" (Eastern) will be used.';
              Write80(write_string);
              tzone := 5;
            End;
          Val(daylight,tdaylight,error);
          If ((error <> 0) Or ((tdaylight <> 0) And (tdaylight <> 1))) Then
            Begin
              Write80(blank);
              write_string := 'Invalid value for DAYLIGHT entered in profile, "' + daylight + '".';
              Write80(write_string);
              write_string := '"0" (No Daylight Savings Time) will be used.';
              Write80(write_string);
              tdaylight := 0;
            End;
          Val(comport,com,error);
          If ((error <> 0) Or ((com <> 1) And (com <> 2))) Then
            Begin
              Write80(blank);
              write_string := 'Invalid value for COM entered in profile, "' + comport + '".';
              Write80(write_string);
              write_string := '"1" (COM1:) will be used.';
              Write80(write_string);
              com := 1;
            End;

      End;

    time_diff := tzone - tdaylight;

    { Show the program title }
    TextColor(Green);
    If Not shutup Then
      Begin
        For i := 1 to 66 Do bar[i] := Chr(205);
        bar[0] := Chr(66);
        Write80(blank);
        write_string := '     ' + Chr(201) + bar + Chr(187);
        Write80(write_string);
        write_string := '     ' + Chr(186) + '                                                                  ' + Chr(186);
        Write80(write_string);
        write_string := '     ' + Chr(186) + ' Set DOS Date and Time from the US Naval Observatory Master Clock ' + Chr(186);
        Write80(write_string);
        write_string := '     ' + Chr(186) + '                                                                  ' + Chr(186);
        Write80(write_string);
        write_string := '     ' + Chr(186) + '                                                                  ' + Chr(186);
        Write80(write_string);
        write_string := '     ' + Chr(200) + bar + Chr(188);
        Write80(write_string);
      End;

    { Set command set controls }
    If command_set = 'IBM'
      Then Begin
        init_string := 'DI T';
        hangup_string := 'HA';
        reset_string := 'IN';
      End
      Else Begin
        If shutup
          Then init_string := 'AT E0 Q0 V0 X0 S12=20 M0 DT'
          Else init_string := 'AT E0 Q0 V0 X0 S12=20 M1 DT';
        hangup_string := 'AT H';
        reset_string := 'AT Z';
      End;

    dial_string := init_string + prefix + '6530351';

    { Set up the COM1 port }
    AssignUsr(com);
    SetSerial(com,1200,1,8,__ParityType(None));
    If command_set = 'IBM'
      Then Begin
         Writeln(Usr,'START');
         Delay(2000);
         Write(Usr,ibm_attn);
         Delay(1000);
         Writeln(Usr,'EC 0');
         Delay(1000);
         Write(Usr,ibm_attn);
         Delay(1000);
         Writeln(Usr,'SO 1');
         Delay(1000);
         Write(Usr,ibm_attn);
         Delay(1000);
         If shutup Then Writeln(Usr,'OP 1=0,2=1,5=2')
                   Else Writeln(Usr,'OP 1=2,2=1,5=2');
         Delay(1000);
      End
      Else Begin
        Delay(1000);
        Write(Usr,attn);
        Delay(2100);
        Writeln(Usr,reset_string);
        Delay(750);
      End;

    { Dial the number }
    done := False;
    retry_counter := 0;

    If Not shutup Then
      Begin
        Write80(blank);
        write_string := 'Dialing the Master Clock ...';
        Write80(write_string);
      End;
    Repeat
      If command_set = 'IBM' Then Write(Usr,ibm_attn);
      Writeln(Usr,dial_string);
      If Not shutup Then
        Begin
          Write80(blank);
          write_string := 'Waiting for connection to it ...';
          Write80(write_string);
        End;

      { First return is the end of the number - junk}
{      Readln(Usr,instring);
 }     delay (1000);
      { The second return is the result code for the call }
      If command_set = 'IBM' Then
        Repeat
          Readln(Usr,instring);
          instring := Copy(instring,1,2);
        Until ((instring <> '35') And (Strip(instring,'B') <> ''))
        Else Begin
          Readln(Usr,instring);
          instring := Strip(Copy(instring,Length(instring) - 1,2),'B');
          If Ord(instring[1]) = 0 Then instring := Copy(instring,2,1);
        End;


      { Check for connection, if connected then process, else retry }
      If ((instring = '1') Or (instring = '12') Or (instring = '31'))
        Then Begin
          If Not shutup Then
            Begin
              Write80(blank);
              write_string := 'Connected to the Clock ...';
              Write80(write_string);
            End;
          Process;
          If Not shutup Then
            Begin
              Write80(blank);
              write_string := 'Time and Date set by the Master Clock.';
              Write80(write_string);
            End;
          done := True;
        End
        Else Begin    { Otherwise retry }
          retry_counter := retry_counter + 1;
          If command_set = 'IBM'
            Then Begin
              Write(Usr,ibm_attn);
              Delay(500);
              Writeln(Usr,hangup_string);
            End
            Else Begin
              Delay(1000);
              Write(Usr,attn);
              Delay(2100);
              Writeln(Usr,reset_string);
            End;
          Delay(750);
        End;
      If (Not done And Not shutup) Then
        Begin
          Write80(blank);
          If retry_counter < 3
            Then write_string := 'Unable to connect to the Naval Observatory Master Clock, retrying ...'
            Else write_string := 'Unable to connect to the Naval Observatory Master Clock, Time and Date not set.';
          Write80(write_string);
        End;
      writeln(instring);
    Until (retry_counter = 3) Or done;

    If command_set = 'IBM'
      Then Begin
        Write(Usr,ibm_attn);
        Delay(500);
      End
      Else Begin
        Delay(1000);
        Write(Usr,attn);
        Delay(2100);
      End;
    Writeln(Usr,hangup_string);
    Delay(1000);
    If command_set = 'IBM'
      Then Begin
        Write(Usr,ibm_attn);
        Delay(1000);
      End;
    Writeln(Usr,reset_string);
    Delay(1000);

    Write80(blank);
    RestoreColor;

  End.
