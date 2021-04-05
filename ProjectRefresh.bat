del .\Makefile
del .\cmake_install.cmake
del .\CMakeFiles\
del CMakeCache.txt
cmake -G "MinGW Makefiles" -S . -B . -DCMAKE_INSTALL_PREFIX=C:\mingw-w64\mingw64\lib\gcc\x86_64-w64-mingw32\8.1.0