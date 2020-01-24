@echo off
net stop w3svc
copy debug\*.dll c:\inetpub\scripts
net start w3svc
