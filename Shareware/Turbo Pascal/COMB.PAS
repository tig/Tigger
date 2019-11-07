program Problem_3;

uses crt;

const
     ArraySize = 100;

type
     stacktype = array [1..ArraySize] of integer;

var
     NC, RC, n,m : integer;
     Key : char;


procedure Push (    ITEM  : integer;
                var STACK : stacktype;
                var TOP   : integer;
                var FULL  : boolean);

   begin
     if TOP = ArraySize then
       FULL := true
     else
       begin
         FULL := FALSE;
         TOP := TOP + 1;
         STACK [TOP] := ITEM;
       end;
   end;

procedure Pop (var ITEM  : integer;
               var STACK : stacktype;
               var TOP   : integer;
               var EMPTY : boolean);

    begin
      if TOP = 0 then
        EMPTY := TRUE
      else
        begin
          EMPTY := FALSE;
          ITEM  := STACK [TOP];
          TOP   := TOP - 1;
        end;
    end;




function R_comb (n, m : integer) : integer;
   begin
     if (n=1) or (m=0) or (m=n) then
       R_comb := 1
     else
       R_comb := R_comb (n -1, m) + R_comb (n-1,m-1)
   end;



function comb (n, m : integer) : integer;
   { computes the binomial coefficient for 0 <= m <= n and n >= 1 }

   label
     10,
     11,
     12,
     13;

   var
     temp, L, C, NTOP, MTOP, LTOP, CTOP : integer;
     NSTACK, MSTACK, LSTACK, CSTACK : stacktype;
     EMPTY, FULL : boolean;


   begin
     C := 0;
     L := 0;
     temp := 0;
     NTOP := 0;
     LTOP := 0;
     MTOP := 0;
     CTOP := 0;

     13 : {entry}
       if (N = 1) or (M = 0) or (M = N) then
         begin
           {now simulate a RETURN}
           writeln ('return;');
           Pop (N, NSTACK, NTOP, EMPTY);
           Pop (M, MSTACK, MTOP, EMPTY);
           Pop (L, LSTACK, LTOP, EMPTY);
           Push (C+1, CSTACK, CTOP, FULL);
           if EMPTY then
             GOTO 10 {RETURN to main calling program}
           else      {BRANCH to appropriate return label}
             case L of
               11 : goto 11;
               12 : goto 12;
               else writeln ('fuck');
             end; {case}
          end; {if}

     { Simulate comb (n-1, m) }
     { Push parameters onto stack }

       Writeln ('comb (n-1, m)');
       Push (N, NSTACK, NTOP, FULL);
       Push (M, MSTACK, MTOP, FULL);
       Pop (C, CSTACK, CTOP, EMPTY);

       { Arrange paramter passing }

       N := N - 1;

       { Push return label onto stack }

       Push (11, LSTACK, LTOP, FULL);

       { now call comb (n-1, m) (call replaced by goto) }

       goto 13;

     11 :
     { simulate comb (n-1, m-1) }
       writeln ('comb (n-1,m-1)');
       Push (N, NSTACK, NTOP, FULL);
       Push (M, MSTACK, MTOP, FULL);
       Pop (C, CSTACK, CTOP, EMPTY);

     { simulate parameter passing for comb (n-1,m-1) }

       N := N - 1;
       M := M - 1;

     { Push return label onto stack }

       Push (12, LSTACK, LTOP, FULL);

     { Call comb (n-1, m-1) }

       goto 13;

     { Now we must simulate a return that would have occurred at the }
     { end of the orignial routine.                                  }

     12 :
       writeln ('return.');
       Pop (N, NSTACK, NTOP, EMPTY);
       Pop (M, MSTACK, MTOP, EMPTY);
       Pop (L, LSTACK, LTOP, EMPTY);
       Push (C+1, CSTACK, CTOP,FULL);
       if EMPTY then
         goto 10
       else
         begin
           case L of
             11 : goto 11;
             12 : goto 12;
             else writeln ('fuck2');
           end; {case}
         end;

      10 : { return to caller }
         writeln ('return!');
         Pop (C, CSTACK, CTOP, EMPTY);
         comb := C;
    end; { routine}


begin {main}
  repeat
    writeln;
    write ('enter n: '); readln (n);
    write ('enter m: '); readln (m);
    RC := R_Comb (n,m);
    NC := Comb (n,m);
    writeln;
    writeln ('Recusive answer = ', RC);
    writeln ('Non Recursive answer = ', NC);
    writeln;
    write ('more?');
    Key := ReadKey;
  until (Key in ['n','N']);
end.