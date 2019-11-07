Program CalcTime;

var  Seconds, Minutes : integer;
     TOT_SEC, TOT_MIN : integer;
     zero :string [1];
     again :boolean;

begin
  again := true;
  while again do
    begin
    TOT_MIN := 0; TOT_SEC := 0;
    Repeat
      Minutes := 0;  Seconds := 0;
      Write ('Enter minutes, seconds:'); readln (Minutes, Seconds);
      TOT_SEC := TOT_SEC + Seconds;
      TOT_MIN := TOT_MIN + Minutes;
    until (Minutes = 0) and (Seconds = 0);
    If TOT_SEC > 59 then
      begin
        Minutes :=  TOT_MIN + (TOT_SEC div 60);
        Seconds :=  (TOT_SEC mod 60);
      end
    else
      begin
        Minutes := TOT_MIN;
        Seconds := TOT_SEC;
      end;
    If Seconds < 10 then Zero := '0'
    else Zero := '';
    writeln;
    Writeln ('The total time is: ', Minutes, ':', Zero, Seconds);
    writeln;
    write ('again?'); read (kbd, zero);
    writeln;
    if (Zero = 'y') or (zero = 'Y') then again := true
    else again := false;
  end;
end.