REM  Open WATCOM v1.9  dos4g/pmodew/nt 
wpp386 -xs -mf -zq -fodemo  demo.cpp 
wlink debug all system nt file { demo.obj } name demow 
