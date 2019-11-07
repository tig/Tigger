Type   String80 = String [80];

Procedure Center (LineNum : integer; Line : String80);

   Var  Column,
        Len
        : integer;

   begin
     Len := Length (Line) div 2;
     Column :=  40 - Len;
     GotoXY (Column, LineNum);
     Write (Line);
   end;

