copy shopper.exe newshop.exe
makex NEWSHOP.EXE NEWSHOP.EXE
@echo off
echo 
echo copy to a:?
pause
@echo on
copy newshop.exe a:
del newshop.exe
@echo off
echo 
echo At the bbs end:
    echo 
    echo 1) logon as bbs
    echo 2) cd ~/master.bin
    echo 3) doscp -r a:NEWSHOP.EXE .
    echo If this is a new version, increment value in ~/master.bin/WSINFO.
@echo on

