procedure Mult (M1, M2 : mat_type;  VAR Result : mat_type);

   var i, j, k : integer;
       temp : mat_type;


   begin
      for i := 1 to Max do
        for j := 1 to Max do
         for k := 1 to Max do
            result [i,j] := (m1 [i,k] and m2[k,j]) or result [i,j];
    end;

