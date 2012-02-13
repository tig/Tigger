program floyd;


uses crt;

const N = 5;

type matrix = array [1..N, 1..N] of integer;
     strng = string[80];

var M, P, PATH : matrix;


procedure writematrix (M : matrix; name : strng);
  var i, j : integer;
      key :char;
  begin
    writeln;
    writeln ('Matrix ', name);
    for i := 1 to N do
      begin
        writeln;
        for j := 1 to N do
          write (M[i,j] :8);
      end;
    writeln;
    key := readkey;
  end;


procedure MinPathFloyd (Var P, PATH, M : matrix);

  var  L, I, J : integer;
       s : strng;

  begin
    for I := 1 to N do
      for J := 1 to N do
        begin
          if (M[I,J] <> 0) then
            P[I,J] := M[I,J]
          else
            P[I,J] := MAXINT div 2;
          PATH [I,J] := 0;
        end;
    for I := 1 to N do
      P[I,I] := 0;
    writematrix (P, 'P');
    writematrix (PATH, 'PATH');

    for L := 1 to N do
      begin
        for I := 1 to N do
          for J := 1 to N do
            if (P[I,L] + P[L,J] < P[I,J]) then
              begin
                P[I,J] := P[I,L] + P[L,J];
                PATH[I,J] := L
              end;
        str (L, s);
        WriteMatrix (P, concat ('P at ', s));
      end;
  end;


begin
  ClrScr;
  M[1,1] := 0;
  M[1,2] := 800;
  M[1,3] := 2985;
  M[1,4] := 310;
  M[1,5] := 200;
  M[2,1] := 800;
  M[2,2] := 0;
  M[2,3] := 410;
  M[2,4] := 612;
  M[2,5] := 0;
  M[3,1] := 2985;
  M[3,2] := 410;
  M[3,3] := 0;
  M[3,4] := 1421;
  M[3,5] := 0;
  M[4,1] := 310;
  M[4,2] := 612;
  M[4,3] := 1421;
  M[4,4] := 0;
  M[4,5] := 400;
  M[5,1] := 200;
  M[5,2] := 0;
  M[5,3] := 0;
  M[5,4] := 400;
  M[5,5] := 0;
  writematrix (M, 'M');
  minpathfloyd (P, PATH, M);
  writematrix (PATH, 'PATH after minpathfloyd');
end.