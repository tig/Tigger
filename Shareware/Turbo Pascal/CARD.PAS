{----------------------------------------------------------------------}
{ This program generates 90000 hands of 5 cards each.  Cards are dealt }
{ Without replacement.  The total number of each card dealt is output  }
{----------------------------------------------------------------------}

Program Cards;

Const numDeals = 90000;

var i,j, deal: longint;
    card : array [1..5] of longint;
    sum  : array [0..52] of longint;
    used : boolean;
    out_file :text;


begin
  assign (out_file, 'Card.out');
  rewrite (out_file);
  randomize;

  writeln (out_file);
  Writeln (out_file,'Generating ', NumDeals, ' hands of 5 card.');
  writeln (out_file);

  for i := 0 to 52 do sum[i] := 0;

  for deal := 1 to NumDeals do
    begin
      for i := 1 to 5 do card[i] := 0;

      for i := 1 to 5 do
        begin
          repeat
            card[i] := trunc(random*52)+1;
            Used := false;
            for j := 1 to i-1 do
              if card[i] = card[j] then
                used := true;
          until (not used);
          Inc(Sum[Card[i]]);
{          writeln ('Card ', i, ' = ', card[i]);}
       end;

   end;

   writeln (out_file);
   writeln (out_file, 'Out of ', NumDeals, ' Hands:');
   for i := 1 to 52 do
     begin
       writeln (out_file, 'Card ', i:2, ' was dealt ', sum[i]:4, ' times');
       sum[0] :=sum[0] + sum[i];
     end;
   writeln (out_file);
   writeln (out_file, 'Total Number of cards dealt = ', sum[0]);
   writeln (out_file);
   writeln (out_file, 'End of output!');
   close (out_file);
end.


