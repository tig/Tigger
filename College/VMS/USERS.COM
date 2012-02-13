$ write sys$output ""
$ write sys$output "HJ"
$ Start:
$ write sys$output ""
$ write sys$output "H"
$ who
$ write sys$output "J"
$ goto start
