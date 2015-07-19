REM Visual Studio 2005
cl /c /TP /D"_WINDOWS" /I"."  DEMO.CPP
link DEMO.OBJ  /out:DEMOV.EXE  /SUBSYSTEM:CONSOLE /machine:X86  

