{--------------------------------------------------------------------}
{ This program generates 50000 throws of a die.  The "average" throw }
{ is computed and output along with the total number of times each   }
{ "side" came up.                                                    }
{--------------------------------------------------------------------}
Program Die_Throw;

Const NumThrows = 50000;

var throw, die:longint;
    sum : array [0..6] of longint;
    out : text;


Begin
  Randomize;
  assign (out, 'die.out');
  rewrite (out);

  for die := 0 to 6 do
    sum[die] :=0;

  writeln (out);
  writeln (out, 'Generating ', NumThrows, ' of a die: ');
  writeln (out);
  For throw := 1 to NumThrows do
    begin
      die := trunc (random*6)+1;
  {    writeln (out, 'Throw ', throw :4,' = ', die:2);}
      sum[0] := sum[0] + die;
      if ((die < 1) or (die >6)) then
        begin
          writeln (out, 'Error...The die is broken!  It rolled a ', die);
          halt
        end
      else
        Inc(Sum [die]);
    end;
  writeln (out);
  Writeln (out,'Average of ', NumThrows,' throws = ', sum[0]/NumThrows:3:2);
  writeln (out);
  for die := 1 to 6 do
    writeln (out, 'Number of ', die,'s thrown: ', sum[die]);
  writeln (out);
  writeln (out, 'End of output!');
  close (out);
end.
