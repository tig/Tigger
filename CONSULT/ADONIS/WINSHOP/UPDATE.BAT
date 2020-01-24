@echo off
if not exist newshop.exe goto error
    del shopper.exe
    ren newshop.exe shopper.exe
    goto done
:error
    echo ERROR:  NEWSHOP.EXE missing, download again.
    goto done
:done
