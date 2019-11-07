@echo off
global del *.err *.map *.sym
copy %1t2.zip .
pkunzip -d -o -n -ssoldier t2.zip
