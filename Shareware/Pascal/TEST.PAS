program test;

const max = 6;

type mat_type = array [1..Max, 1..Max] of boolean;

var A, B, c : mat_type;
    i, j, k : integer;


 procedure Init (var M : mat_type);
   var i, j : integer;

     begin
       for i := 1 to Max do
         for j := 1 to Max do
            M [i,j] := false;
     end;


procedure Mult (M1, M2 : mat_type;  VAR Result : mat_type);

   var i, j, k : integer;
       temp : mat_type;


   begin
      for i := 1 to Max do
        for j := 1 to Max do
         for k := 1 to Max do
            result [i,j] := (m1 [i,k] and m2[k,j]) or result [i,j];
    end;

  procedure power (M : mat_type; n : integer; Var Result : mat_type);

    var i : integer;

    begin
      Init (result);
      Mult (M, M, result);
      for i := 1 to (n-2) do
        Mult (M, Result, Result);
    end;

   procedure add (A, B : mat_type; VAR Result : mat_type);

      var i, j :integer;

      begin
        for i := 1 to Max do
          for j := 1 to Max do
            Result [i,j] := A[i,j] or B[i,j];
      end;

 procedure write_mat (A : mat_type);
   var i, j : integer;
 begin
   for i := 1 to Max do
    begin
      for j := 1 to Max do
        write (A[i,j] :7);
      writeln;
    end;
    writeln;
  end;


begin
  clrscr;
  for i := 1 to Max do
    for j := 1 to Max do
      begin
        A[i,j] := false;
        B[i,j] := false;
      end;

  {A[1,2] := true;
  A[2,3] := true;
  A[3,4] := true;
  a[4,5] := true;
  a[5,6] := true;
  }
  Add (A, B, B);
  Power (a, 2, c);
  Add (B, C, B);
  Power (a, 3, C);
  Add (B, C, B);
  Power (A, 4, C);
  Add (B, C, B);
  Power (A, 5, C);
  Add (B, C, B);


end.
