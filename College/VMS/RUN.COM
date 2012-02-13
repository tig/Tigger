$ ! file to run student programs
$ ! CSc 227, Spring 1987
$ ! program #5
$ on CONTROL_Y then goto PANIC
$ set noon
$ RRUN: SUBROTINE
$ ! directory where bookkeeping data files are kept
$ NAME :== patrick.227.names
$ ! directory where input file(s) are kept (test deck)
$ TEST := patrick.227.testdec.prog5
$ ! directory where student programs & algorithms are
$ SECTION := patrick.227.prog5
$ ! directory where header and message text file 
$ TEXT := patrick.227.runtext.prog5
$ open/read names ['NAME']found.dat
$ open/write aerrors ['NAME']noalgo.dat
$ open/write cerrors ['NAME']comerrors.dat
$ open/write rerrors ['NAME']runerrors.dat
$ copy ['TEXT']start.txt ['NAME']output.dat
$ LOOP:
$ ! process each user
$ read/end=EOF1 names username
$ read/end=EOF2 names person
$ append ['TEXT']header.txt ['NAME']output.dat
$ open/append output ['NAME']output.dat
$ write output "USERNAME:   ", username
$ write output "HUMAN NAME: ", person
$ close output
$ ! append ['TEXT']grader.txt ['NAME']output.dat
$ ! append ['TEXT']ff.txt ['NAME']output.dat
$ on error then goto COMPERR
$ pascal/nooptimize/nostandard/check/list=['SECTION']'username'.lis/object=['SECTION']'username'.obj ['SECTION']'username'.p
$ link/nomap/executable=['SECTION']'username'.exe ['SECTION']'username'.obj
$ !
$ ! test program against our test files
$ !
$ set NOON
$ del ['SECTION']outfile.dat;*
$ del ['SECTION']errfile.dat;*
$ append ['TEXT']one.txt ['NAME']output.dat
$ assign/user ['TEST']inone.dat sys$input:
$ on error then goto RUNERR1
$ assign/user ['SECTION']errfile.dat sys$error:
$ assign/user ['SECTION']outfile.dat sys$output:
$ run ['SECTION']'username'.exe
$ goto CASE2
$ RUNERR1:
$ ! runtime errors exist
$ append ['TEXT']rerrors.txt ['NAME']output.dat
$ append ['SECTION']errfile.dat ['NAME']output.dat
$ write rerrors username
$ write rerrors person
$ CASE2:
$ set NOON
$ append ['SECTION']outfile.dat ['NAME']output.dat
$ append ['TEXT']equal.txt ['NAME']output.dat
$ del ['SECTION']outfile.dat;*
$ del ['SECTION']errfile.dat;*
$ append ['TEXT']two.txt ['NAME']output.dat
$ assign/user ['SECTION']outfile.dat sys$output:
$ assign/user ['TEST']intwo.dat sys$input:
$ on error then goto RUNERR2
$ assign/user ['SECTION']errfile.dat sys$error:
$ run ['SECTION']'username'.exe
$ goto CASE3
$ RUNERR2:
$ append ['TEXT']rerrors.txt ['NAME']output.dat
$ append ['SECTION']errfile.dat ['NAME']output.dat
$ write rerrors username
$ write rerrors person
$ CASE3:
$ set NOON
$ append ['SECTION']outfile.dat ['NAME']output.dat
$ append ['TEXT']equal.txt ['NAME']output.dat
$ del ['SECTION']outfile.dat;*
$ del ['SECTION']errfile.dat;*
$ append ['TEXT']three.txt ['NAME']output.dat
$ assign/user ['SECTION']outfile.dat sys$output:
$ assign/user ['TEST']inthree.dat sys$input:
$ on error then goto RUNERR3
$ assign/user ['SECTION']errfile.dat sys$error:
$ run ['SECTION']'username'.exe
$ goto CASE4
$ RUNERR3:
$ append ['TEXT']rerrors.txt ['NAME']output.dat
$ append ['SECTION']errfile.dat ['NAME']output.dat
$ write rerrors username
$ write rerrors person
$ CASE4:
$ set NOON
$ append ['SECTION']outfile.dat ['NAME']output.dat
$ append ['TEXT']equal.txt ['NAME']output.dat
$ del ['SECTION']outfile.dat;*
$ del ['SECTION']errfile.dat;*
$ append ['TEXT']four.txt ['NAME']output.dat
$ assign/user ['SECTION']outfile.dat sys$output:
$ assign/user ['TEST']infour.dat sys$input:
$ on error then goto RUNERR4
$ assign/user ['SECTION']errfile.dat sys$error:
$ run ['SECTION']'username'.exe
$ goto CASE5
$ RUNERR4:
$ append ['TEXT']rerrors.txt ['NAME']output.dat
$ append ['SECTION']errfile.dat ['NAME']output.dat
$ write rerrors username
$ write rerrors person
$ CASE5:
$ set NOON
$ append ['SECTION']outfile.dat ['NAME']output.dat
$ append ['TEXT']equal.txt ['NAME']output.dat
$ del ['SECTION']outfile.dat;*
$ del ['SECTION']errfile.dat;*
$ append ['TEXT']five.txt ['NAME']output.dat
$ assign/user ['SECTION']outfile.dat sys$output:
$ assign/user ['TEST']infive.dat sys$input:
$ on error then goto RUNERR5
$ assign/user ['SECTION']errfile.dat sys$error:
$ run ['SECTION']'username'.exe
$ goto ALGO
$ RUNERR5:
$ append ['TEXT']rerrors.txt ['NAME']output.dat
$ append ['SECTION']errfile.dat ['NAME']output.dat
$ write rerrors username
$ write rerrors person
$ ! print the student's algorithm
$ ALGO:
$ on error then goto NO_ALGO
$ append ['SECTION']outfile.dat ['NAME']output.dat
$ append ['TEXT']ff.txt ['NAME']output.dat
$ append ['SECTION']'username'.a ['NAME']output.dat
$ append ['TEXT']ff.txt ['NAME']output.dat
$ goto NOERROR
$ NO_ALGO:
$ ! student does not have an algorithm file
$ append ['TEXT']no_algo.txt ['NAME']output.dat
$ write aerrors username
$ write aerrors person
$ NOERROR:
$ ! print the student's program
$ append ['SECTION']'username'.p ['NAME']output.dat
$ goto FF
$ !
$ ! compilation errors exist
$ !
$ COMPERR:
$ append ['TEXT']ff.txt ['NAME']output.dat
$ ! print student's algorithm first
$ on error then goto NO_ALGO2
$ append ['SECTION']'username'.a ['NAME']output.dat
$ append ['TEXT']ff.txt ['NAME']output.dat
$ goto NOERROR2
$ NO_ALGO2:
$ ! student does not have an algorithm file
$ append ['TEXT']no_algo.txt ['NAME']output.dat
$ write aerrors username
$ write aerrors person
$ NOERROR2:
$ ! print student's compilation listing
$ append ['TEXT']cerrors.txt ['NAME']output.dat
$ append ['SECTION']'username'.lis ['NAME']output.dat
$ write cerrors username
$ write cerrors person
$ FF:
$ append ['TEXT']ff.txt ['NAME']output.dat
$ set noon
$ delete/noconfirm ['SECTION']'username'.lis;*
$ delete/noconfirm ['SECTION']'username'.exe;*
$ delete/noconfirm ['SECTION']'username'.obj;*
$ goto LOOP
$ EOF2: 
$ write sys$output "problem with names files....!!!!"
$ EOF1:
$ set noon
$ close names
$ close output
$ close cerrors
$ close rerrors
$ close aerrors
$ delete/noconfirm ['NAME']outfile.dat;*
$ open/append output ['NAME']output.dat
$ write output "The following students did not have algorithm files:

"
$ close output
$ append ['NAME']noalgo.dat ['NAME']output.dat 
$ open/append output ['NAME']output.dat
$ write output "The following students did not have source files:

"
$ close output
$ append ['NAME']not_found.dat ['NAME']output.dat 
$ open/append output ['NAME']output.dat
$ write output "The following student's programs have runtime errors:

"
$ close output
$ append ['NAME']runerrors.dat ['NAME']output.dat 
$ open/append output ['NAME']output.dat
$ write output "The following student's programs have compilation errors:

"
$ close output
$ append ['NAME']comerrors.dat ['NAME']output.dat 
$ EXIT 
$ ENDSUBROTINE
$ call rrun
$ ! to automatically print the output file, uncomment the next line
$ ! print ['NAME']output.dat
$ PANIC:
$ set noon
$ close names
$ close output
$ close cerrors
$ close rerrors
$ close aerrors
$ END:
$ EXIT 1
