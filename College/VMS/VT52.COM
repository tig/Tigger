$ escape[0,32]=%X1B
$ write sys$output escape,"PrTM0",escape, "\"
$ set term/vt52
