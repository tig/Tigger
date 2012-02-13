$ ! This is a command program to compile and link a fortran file
$ !
$ !
$ on Warning then $ GOTO query    !this goes to SEDT if there is a compile err.
$ on CONTROL_Y then $ goto ex
$ if P1 .eqs. "" then inquire P1 File
$ write SYS$OUTPUT "Compiling "'P1'".FOR"
$ Fortran/List 'P1'
$ Write Sys$Output "Linking "'P1'".OBJ"
$ Link 'P1'
$ Deln 'P1'.OBJ;*
$ Write Sys$Output "Done!"
$ ex:
$ Exit 1
$ !
$ query:
$ Write sys$output " "
$ open console tt:
$ read/prompt="Compilation errors, do you want a listing? " CONSOLE YN
$ !
$ if YN .EQ. "Y" .OR. YN .EQ. "y" then $ short 'P1'.LIS
$ set term/echo
$ close console
$ write sys$output " "
$ Exit 1
