@echo off

mkdir build
cd build
set CMAKE_PREFIX_PATH=C:\Qt\6.2.2\msvc2019_64\lib\cmake\
cmake -G "Visual Studio 17 2022" -A x64 -DPLATFORM=windows -DCMAKE_BUILD_TYPE=Release ..
set /p proceed=Co build app luon hay khong? (y/n): 
if /i "%proceed%"=="y" (
    cmake --build . --target package --config Release -- -nologo -v:m -maxcpucount
    echo Build app da xong
)

echo Nhan phim bat ki de thoat
pause >nul