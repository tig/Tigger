$ ! This is a command program to compile and link a BASIC file
$ !
$ !
$ On Warning then Exit          ! this exits if there is a compile err.
$ if P1 .eqs. "" then inquire P1 File
$ write SYS$OUTPUT "Compiling "'P1'".BAS"
$ BASIC/List 'P1'
$ Write Sys$Output "Linking "'P1'".OBJ"
$ Link 'P1'
$ Write Sys$Output "Done!"
$ Exit 1
