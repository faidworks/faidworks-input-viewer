@echo off
setlocal

echo Configuring build...
cmake -B build_win -S . -DBUILD_SHARED_LIBS=OFF -DSFML_STATIC_LIBRARIES=TRUE

echo Building Release...
cmake --build build_win --config Release

echo.
echo Done. Output: build_win\bin\Release\faidworks-input-viewer.exe
echo Remember to include the resources/ folder when distributing.
