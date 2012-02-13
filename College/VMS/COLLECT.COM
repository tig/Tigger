$ ! this program collects programs and algorithms
$ set noon
$ COLLECT: SUBROUTINE
$ ! directory where bookkeeping files are kept
$ NAMES :== patrick.227.names
$ ! directory where student files are to be copied into
$ SPROGS :== patrick.227.prog6
$ open/read names ['NAMES']matrics.dat
$ open/write not_found ['NAMES']not_found.dat
$ open/write found ['NAMES']found.dat
$ LOOP:
$ on error then goto ERR1
$ read/end=EOFl names username
$ read names person
$ copy/log ['username']'prog'.p ['SPROGS']'username'.p
$ write found username 
$ write found person
$ on error then goto LOOP
$ copy/log ['username']'prog'.a ['SPROGS']'username'.a
$ goto LOOP
$ ERR1:
$ write not_found username
$ write not_found person
$ goto LOOP
$ EOFl:
$ close names
$ close not_found
$ close found
$ EXIT 
$ ENDSUBROTINE
$ prog = "pr6"
$ call collect
$ EXIT 1
