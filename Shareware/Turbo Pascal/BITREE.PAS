program bitree (input, output);

const space = 4;

type
      nodeptr = ^nodetype;
      nodename = packed array[1..15] of char;
      nodematric = packed array [1..6];
      nodetype = record
                   left:nodeptr;
                   matric: nodematric;
                   name:nodename;
                   test_score:real;
                   right:nodeptr
                 end;

var
   tree, temp,temp1 : nodeptr;
   gradefile :text;
   command :integer;
   rec : nodetype;


procedure init (var tree:nodeptr);
          begin
             tree:=nil
          end;

procedure search (matric : nodematric; root : nodeptr; var foundnode : nodeptr);

  var
   parent : nodeptr;

   begin
     parent:=root;
     if (root=nil) then
        foundnode:=nil
     else
        if matric=root^.matric then
           foundnode:=root
        else
           if matric < root^.matric then
              search (matric, root^.left, foundnode)
           else
              search (matric, root^.right, foundnode)
    end;

procedure add (var tree:nodeptr; rec:nodeptr);

   var
       cur, parent, temp, new_rec : nodeptr;

   begin
     search(rec^.matric, tree, temp);
     if (temp = nil) then
        if ((rec^.test_score>=0) and (rec^.test_score<=100)) then
            begin
              new(new_rec);
              new_rec^.matric:=rec^.matric;
              new_rec^.name:=rec^.name;
              new_rec^.test_score:=rec^.test_score;
              new_rec^.right := nil;
              new_rec^.left := nil;
              cur := tree;
              parent := tree;
              if cur<>nil then
                 begin
                   while cur<>nil do
                         begin
                           if new_rec^.matric > cur^.matric then
                             begin
                              parent:=cur;
                              cur:=cur^.right
                             end
                           else
                            begin
                              parent:=cur;
                              cur:=cur^.left
                            end;
                         end;
                   if new_rec^.matric>parent^.matric then
                      parent^.right:=new_rec
                   else
                      parent^.left:=new_rec;
                 end
              else
                tree := new_rec;
            end
          else
            writeln('error:score is out of bounds')
     else
            writeln('error:duplicate matric')
   end;

procedure  readfile(var tree : nodeptr);

var
    temp : nodeptr;

   begin
     open (gradefile, 'grade.in', old);
     reset (gradefile);
     new (temp);
     writeln ('reading gradefile....');
     while not eof (gradefile) do
           begin
             read (gradefile, temp^.matric);
             read (gradefile, temp^.name);
             readln (gradefile, temp^.test_score);
             add (tree, temp)
           end;
     close (gradefile)
   end;

procedure delete (x :nodematric; var p: nodeptr);

   var
     q:nodeptr;

     procedure del(var r:nodeptr);
        begin
          if r^.right <> nil then del(r^.right)
          else begin
            q^.matric:=r^.matric;
            q^.name:=r^.name;
            q^.test_score:=r^.test_score;
            q:= r;
            r := r^.left;
           end
        end;
    begin
      if p = nil then write ('record not found')
      else
        if x < p^.matric then
           delete (x, p^.left)
        else
          if x> p^.matric then
           delete (x, p^.right)
          else
            begin
              q:=p;
                 if q^.right = nil then
                    p := q^.left
                 else
                   if q^.left = nil then
                      p:= q^.right
                   else
                      del (p^.left);
            end
       end;

procedure view (t:nodeptr);

  begin
    if t <> nil then
       begin
         view (t^.left);
         writeln;
         write (t^.matric, '   ', t^.name, '    ', t^.test_score:0:2);
         view (t^.right);
       end;
  end;


procedure print_tree(t:nodeptr; h:integer);

var
   i : integer;

   begin
     if t<>nil then
       begin
         print_tree(t^.left, h+1);
         for i := 1 to h do write ('  ');
            writeln (t^.matric);
         print_tree (t^.right, h+1);
       end
   end;




BEGIN {MAIN PROGRAM}

     INIT (tree);
     READFILE(tree);
     new (Temp);
     REPEAT
       writeln;
       WRITELN('CHOOSE FROM THE FOLLOWING MENU OF OPTIONS IN ORDER');
       WRITELN('TO MAKE ANY CHANGES TO THE GRADEFILE:');
       WRITELN;
       WRITELN ('1) ADD A RECORD');                 {MAIN MENU BEGINS}
       WRITELN ('2) MODIFY A RECORD(THE SCORE)');
       WRITELN ('3) DELETE A RECORD');
       WRITELN ('4) VIEW A RECORD');
       WRITELN ('5) VIEW ALL RECORDS BY MATRIC');
       WRITELN ('6) PRINT THE TREE');
       WRITELN ('7) END PROGAM');
       WRITE ( '      ENTER YOUR OPTION:');
       READLN (COMMAND);
       writeln;
       CASE COMMAND OF
               1 : BEGIN
                     WRITE('ENTER MATRIC: '); READLN(TEMP^.MATRIc);
                     WRITE('ENTER NAME: '); READLN(TEMP^.NAME);
                     WRITE('ENTER SCORE: '); READLN(TEMP^.test_SCORE);
                     ADD(tree, TEMP);
                   END;
               2 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE MODIFIED:');
                     READLN(TEMP^.MATRIc);
                     SEARCH(TEMP^.MATRIC, TREE, TEMP1);
                     IF (TEMP1 <> NIL) THEN
                         BEGIN
                             WRITE('ENTER NEW SCORE:');
                             READLN (TEMP^.TEST_SCORE);
                             if (TEMP^.TEST_SCORE >= 0) and (TEMP^.TEST_SCORE <= 100) then
                                TEMP1^.TEST_SCORE := TEMP^.TEST_SCORE
                             else
                                writeln ('ERROR: Score is out of bounds.');
                         END
                     ELSE
                         WRITELN ('RECORD NOT FOUND');
                   END;
               3 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE DELETED:');
                     READLN(TEMP^.MATRIC);
                     DELETE(TEMP^.MATRIC, TREE);
                   END;
               4 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE VIEWED:');
                     READLN(TEMP^.MATRIC);
                     SEARCH(TEMP^.MATRIC, TREE, TEMP1);
                     if (temp <> nil) then
                        BEGIN
                          writeln (TEMP1^.MATRIC);
                          writeln (TEMP1^.NAME);
                          writeln (TEMP1^.TEST_SCORE:0:2)
                        END
                     ELSE
                        WRITELN('RECORD NOT FOUND');
                   END;
               5 : begin
                     writeln ('MATRIC     NAME             SCORE');
                     view (tree);
                     writeln;
                   end;
               6 : PRINT_TREE(tree, SPACE);
               7 : writeln ('NOW EXITING PROGRAM');
            end;
     UNTIL (COMMAND = 7);

End.
