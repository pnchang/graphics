REM Visual Studio 2005
cl /c /Zc:forScope- /D"_WINDOWS" /I"%DXSDK_DIR%Include"  render.cpp 
cl /c /D"_WINDOWS" /I"%DXSDK_DIR%Include"  main.cpp 
link main.obj render.obj /out:main.exe gdi32.lib user32.lib Advapi32.lib winmm.lib d3d9.lib d3dx9.lib  /LIBPATH:"%DXSDK_DIR%Lib\x86"
