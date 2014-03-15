REM Visual Studio 2010
cl /nologo /W3 /Od /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Gm- /MT /GS- /c /I"%DXSDK_DIR%Include" elevated.cpp
link elevated.obj winmm.lib Kernel32.lib User32.lib d3d9.lib d3dx9.lib /out:elevated.exe /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib  /ENTRY:"WinMainCRTStartup"  /NOENTRY /NODEFAULTLIB /LIBPATH:"%DXSDK_DIR%Lib\x86"
