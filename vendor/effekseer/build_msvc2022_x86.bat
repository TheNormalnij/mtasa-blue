mkdir build_msvc2022
cd build_msvc2022
cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_INSTALL_PREFIX=../install_msvc2022 -DUSE_LLGI=OFF -DBUILD_DX12=OFF -DBUILD_DX9=ON .. -DCMAKE_DEBUG_POSTFIX=d
cmake --build . --config Debug --target INSTALL
cmake --build . --config Release --target INSTALL
