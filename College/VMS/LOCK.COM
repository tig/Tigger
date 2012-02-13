$ ! lock.com
$ !
$ SET NOVER
$ ON CONTROL_Y THEN $ GOTO EX
$ ON WARNING THEN $ GOTO EX
$ SET TERM/NOECHO
$ OPEN CONSOLE TT:
$ READ/END_OF_FILE=WAIT_FOR_PASSWORD-
    /ERROR=WAIT_FOR_PASSWORD-
    /PROMPT="Enter any password (re-enter it to unlock this terminal) => " -
    CONSOLE REAL_PASSWORD
$ !
$ PASSWORD = ""
$ COUNT = 0
$ ON CONTROL_Y THEN $ GOTO WAIT_FOR_PASSWORD
$ ON WARNING THEN $ GOTO WAIT_FOR_PASSWORD
$ @[STUDENT.OPTION]CLEAR.COM
$ !
$ WAIT_FOR_PASSWORD:
$ COUNT = COUNT + 1
$ IF COUNT .GT. 20 THEN $ GOTO EX
$ TYPE SYS$INPUT:







   [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
   []                                                                        []
   []   ˜˜˜˜˜˜    ˜˜˜     ˜˜        ˜˜        ˜˜    ˜˜˜˜˜˜˜˜˜   ˜˜˜˜˜˜˜˜˜    []
   []     ˜˜      ˜˜˜˜    ˜˜        ˜˜        ˜˜   ˜˜           ˜˜           [] 
   []     ˜˜      ˜˜ ˜˜   ˜˜        ˜˜        ˜˜   ˜˜           ˜˜           []
   []     ˜˜      ˜˜  ˜˜  ˜˜        ˜˜        ˜˜    ˜˜˜˜˜˜˜˜    ˜˜˜˜˜˜˜      [] 
   []     ˜˜      ˜˜   ˜˜ ˜˜        ˜˜        ˜˜           ˜˜   ˜˜           []
   []     ˜˜      ˜˜    ˜˜˜˜        ˜˜        ˜˜           ˜˜   ˜˜           []
   []   ˜˜˜˜˜˜    ˜˜     ˜˜˜         ˜˜˜˜˜˜˜˜˜˜    ˜˜˜˜˜˜˜˜˜    ˜˜˜˜˜˜˜˜˜˜   [] 
   []                                                                        []
   [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]

     This terminal is in use and has been locked.  Please don't try to use it.
  







$ !
$ READ/END_OF_FILE=WAIT_FOR_PASSWORD-
    /ERROR=WAIT_FOR_PASSWORD-
    /PROMPT="                                 Password:" CONSOLE PASSWORD
$ IF PASSWORD .NES. REAL_PASSWORD THEN $ GOTO WAIT_FOR_PASSWORD
$ !
$ EX:
$ SET TERM/ECHO
$ CLOSE CONSOLE
$ count = count - 1
$ write Sys$output "There were ", COUNT, " failed attempts to unlock this terminal."
$ EXIT 1
