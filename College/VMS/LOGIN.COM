$ !
$ set term/inquire/line_edit/regis
$ @sedt
$ login := set host evax1
$ lock :== @[student.com]newlock.com
$ di  :== direct/size/date=mod/pro
$ dir :== direct/brief
$ dd  :== "$sys$system:sd >>1"
$ du  :== "$sys$system:sd <"
$ do  :== "$sys$system:sd >>"
$ dh  :== "$sys$system:sd ."
$ edt :== edit/edt/command:edtini.edt
$ pur :== purge/log/keep=1
$ tim :== show time
$ SET PROCESS/NAME="Reggit"
$ set prompt="DCL> "
$ SD :== SET DEFAULT
$ DELN :== DELETE/NOCONFIRM/log
$ Del :== @[student.com]deleet.com
$ UNPROT :== SET PROT=(S:RWED,O:RWED,G:RWED,W:RWED)
$ PROT :== SET PROT=(S:RWED,O:RWED,G,W)
$ set prot/default
$ WHO :== SHOW USER
$ users :== @users.com
$ ! 
$ !
$ purge
$ F:== @FOR.COM
$ p:== @PAS.COM
$ b:== @BAS.com
$ bye :== @logout
