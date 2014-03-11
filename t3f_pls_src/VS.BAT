REM Visual Studio 2005
cl /c /D"_WINDOWS" /I"%DXSDK_DIR%Include"  font.cpp 
cl /c /D"_WINDOWS" /I"%DXSDK_DIR%Include"  Particle.cpp 
cl /c /D"_WINDOWS" /I"%DXSDK_DIR%Include"  text.cpp 
cl /c /D"_WINDOWS" /GS- /Zc:forScope- /I"%DXSDK_DIR%Include"  Main.cpp 
link Main.obj font.obj Particle.obj text.obj /out:Main.exe Kernel32.lib User32.lib d3d9.lib d3dx9.lib  /subsystem:WINDOWS /NODEFAULTLIB /LIBPATH:"%DXSDK_DIR%Lib\x86"