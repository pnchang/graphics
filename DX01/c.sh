# MINGW DirectX 9.0
# export DXSDK='/C/Program Files (x86)/Microsoft DirectX SDK (June 2010)/'
gcc -fno-exceptions -fpermissive -static -D"MINGW" -I"$DXSDK""Include" -L"$DXSDK""lib/x86" -o mainG.exe main.cpp render.cpp  -lwinmm -ld3d9 -ld3dx9 -lstdc++ -mwindows 