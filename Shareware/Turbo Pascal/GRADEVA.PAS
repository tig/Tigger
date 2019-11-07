program grade (input, output);

{Ron Ref
Assignment #3
600-20-2001
3-14-88}
{THIS PROGRAM REVIEWS A STUDENTS GRADES BY MEANS OF A LINK LIST.  IT WILL ADD,
MODIFY, DELETE AND VIEW A SINGLE RECORD OR THE ENTIRE FILE SORTED BY SCORE OR
NAME.}

{******************************************************************************
                            DECLARATIONS
******************************************************************************}

TYPE
    string_type = PACKED ARRAY [1..15] OF CHAR;  {SPACE FOR STUDENT NAME}
    mat_type = packed array [1..6] of char;      {SPACE FOR STUDENT MATRIX}
     NAME_TYPE = string_type;
     REC_PTR = ^RECORD_TYPE;                      {POINTER FOR EACH LINK}
     RECORD_TYPE = RECORD
                NAME : NAME_TYPE;            {DATA IN EACH RECORD}
                MATRIX : mat_type;
                SCORE : REAL;
                NEXT : REC_PTR
              END;

 VAR
    LIST, S_REC : REC_PTR;               {RECORD POINTERS}
    TEMP : RECORD_TYPE;                  {TEMPORARY RECORD HOLDER}
    A : RECORD_TYPE;
    gradefile :text;                     {INPUT FILE}
    COMMAND : integer;                   {MENU KEY}  

{*******************************************************************************
                            PROCEDURES
*******************************************************************************}


PROCEDURE INIT (VAR LIST:Rec_PTR);    {THIS PROCEDURE INITIALIZES THE}
     BEGIN                            {LINK LISTS AND CREATES A DATA}
       NEW(LIST);                     {HOLDING SPOT AND A  POINTER}
       LIST^.MATRIX := '0';
       LIST^.NAME := 'HeaderNode';
       LIST^.SCORE := 99.99;
       LIST^.NEXT:=LIST
     END;


FUNCTION SEARCH (LIST:REC_PTR;               {SEARCHES FOR THE RECORD TO}
                  VAR S_REC:REC_PTR;         {BE MANIPULATED}
                  MAT:mat_type): BOOLEAN;
     VAR
        CUR,                  {POINTS TO THE CURRENT DATA}
        PREV:REC_PTR;         {POINTS TO THE PREVIOUS DATA}
        FOUND : BOOLEAN;

     BEGIN
       CUR := List;
       FOUND:= FALSE;
       WHILE ((CUR^.NEXT <> LIST) and (FOUND = FALSE)) do
          BEGIN
            PREV := CUR;
            CUR := CUR^.NEXT;
            IF (CUR^.MATRIX = MAT) then
               FOUND := TRUE;
          END;
       S_REC := PREV;
       SEARCH := FOUND;
     END;


PROCEDURE ADD (VAR LIST:REC_PTR; REC: RECORD_TYPE);    {ADDS RECORDS TO THE }
     VAR                                               {LINKED LIST}
        NEW_REC, TEMP: REC_PTR;   {A NEW LINK FOR ADDITIONS AND A TEMPORARY}
     BEGIN                        {HOLDING SPOT}
     IF (SEARCH(LIST, TEMP, REC.MATRIX) = FALSE) THEN
         IF ((REC.SCORE >= 0) AND (REC.SCORE <= 100)) THEN
            BEGIN
              NEW(NEW_REC);
              NEW_REC^.NEXT:=LIST^.NEXT;
              NEW_REC^.MATRIX := REC.MATRIX;
              NEW_REC^.NAME := REC.NAME;
              NEW_REC^.SCORE := REC.SCORE;
              LIST^.NEXT:=NEW_REC
            END
         ELSE
            WRITELN('ERROR:SCORE IS OUT OF BOUNDS')
     ELSE
       WRITELN ('ERROR:DUPLICATE MATRIX')
     END;



PROCEDURE READFILE(VAR LIST: REC_PTR);      {READS THE INPUT FILE}
     VAR
        TEMP : RECORD_TYPE;
        test : mat_type;

     BEGIN
     OPEN (GRADEFILE, 'GRADE.IN', OLD);
     RESET (GRADEFILE);
     WRITELN ('READING GRADEFILE.....');
     WHILE NOT EOF (GRADEFILE) DO
           BEGIN
             read (GRADEFILE,TEMP.MATRIX);
             read (GRADEFILE,TEMP.NAME);
             readln (GRADEFILE,TEMP.SCORE);
             ADD (LIST, TEMP);                 {CALLS THE PROCEDURE ADD}
           END;
     CLOSE (GRADEFILE);
     END;


PROCEDURE DELETE (VAR LIST:REC_PTR; MATRIX:mat_type);   {DELETES A RECORD FROM}
     VAR                                                {LINKED LIST}
         TEMP, S_REC : REC_PTR;            {POINTS  TO DATA IN DELETED RECORD}
     BEGIN
       IF (SEARCH (LIST, S_REC, MATRIX) = TRUE) THEN
          BEGIN
            writeln ('Deleting ', S_REC^.NEXT^.NAME);
            TEMP := S_REC^.NEXT^.NEXT;
            DISPOSE(S_REC^.NEXT);
            S_REC^.NEXT:= TEMP;
          END
       ELSE
         WRITELN ('CANNOT DELETE....NOT FOUND');
     END;


Procedure View (list : rec_ptr);            {ALLOWS YOU TO VIEW A RECORD}
     var temp : rec_ptr;

     begin
       writeln;
       writeln ('Matrix   Name            Score');
       temp := list^.next;
       while (temp <> list) do
         begin
           writeln (temp^.matrix,'  ',temp^.name:15,'  ', temp^.score:0:2);
           temp := temp^.next;
         end;
     end;

BEGIN {MAIN PROGRAM}

     INIT (LIST);
     READFILE(LIST);
     view (list);
     REPEAT
       writeln;
       writeln;
       writeln;
       WRITELN('CHOOSE FROM THE FOLLOWING MENU OF OPTIONS IN ORDER');
       WRITELN('TO MAKE ANY CHANGES TO THE GRADEFILE:');
       WRITELN;
       WRITELN ('1) ADD A RECORD');                 {MAIN MENU BEGINS}
       WRITELN ('2) MODIFY A RECORD(THE SCORE)');
       WRITELN ('3) DELETE A RECORD');
       WRITELN ('4) VIEW A RECORD');
       WRITELN ('5) VIEW ALL RECORDS BY SCORE');
       WRITELN ('6) VIEW ALL RECORDS BY NAME');
       WRITELN ('7) END PROGAM');
       WRITE ( '      ENTER YOUR OPTION:');
       READLN (COMMAND);
       CASE COMMAND OF
               1 : BEGIN
                     WRITE('ENTER MATRIX: '); READLN(TEMP.MATRIX);
                     WRITE('ENTER NAME: '); READLN(TEMP.NAME);
                     WRITE('ENTER SCORE: '); READLN(TEMP.SCORE);
                     ADD(LIST, TEMP);
                   END;
               2 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE MODIFIED:');
                     READLN(TEMP.MATRIX);
                     IF (SEARCH(LIST, S_REC, TEMP.MATRIX)) THEN
                         BEGIN
                             WRITE('ENTER NEW SCORE:');
                             READLN (TEMP.SCORE);
                             if (TEMP.SCORE >= 0) and (TEMP.SCORE <= 100) then
                                S_REC^.NEXT^.SCORE := TEMP.SCORE
                             else
                                writeln ('ERROR: Score is out of bounds.');
                         END
                     ELSE
                         WRITELN ('RECORD NOT FOUND');
                   END;
               3 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE DELETED:');
                     READLN(TEMP.MATRIX);
                     DELETE(LIST, TEMP.MATRIX);
                   END;
               4 : BEGIN
                     WRITE('ENTER THE MATRIC # OF THE RECORD TO BE VIEWED:');
                     READLN(TEMP.MATRIX);
                     IF (SEARCH(LIST, S_REC, TEMP.MATRIX) = TRUE)THEN
                        BEGIN
                          WRITELN(S_REC^.NEXT^.NAME);
                          WRITELN(S_REC^.NEXT^.MATRIX);
                          WRITELN(S_REC^.NEXT^.SCORE:0:2);
                        END
                     ELSE
                        WRITELN('RECORD NOT FOUND');
                   END;
               5 : view (list);
               7 : writeln ('NOW EXITING PROGRAM');
            end;
     UNTIL (COMMAND = 7);

End.
