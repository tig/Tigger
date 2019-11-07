@echo off
global del *.err *.map *.sym
pkzip -i -p -r -o -ssoldier t2.zip *.*
copy t2.zip %1
