#!/bin/bash
current_dir=$(dirname $(readlink -f $0))
cd "$current_dir/.."

echo "WorkingDir: '$(pwd)'"
echo ""

# PATH="D:/CppCompilers/LLVM/bin;${PATH}"
#export PATH="D:/CppCompilers/MinGW/mingw64/bin;${PATH}"
export PATH="D:/msys64/mingw64/bin;${PATH}"
export PATH="D:/CppCompilers/LLVM/bin;${PATH}"
export LD_LIBRARY_PATH="D:/CppCompilers/MinGW/mingw64/bin:${LD_LIBRARY_PATH}"

if [ "$1" == "--clean" ]; then
	rm -rf build/build
fi

USE_CLANG="false"
if [[ "$USE_CLANG" == "true" ]]; then
	cmake . -B build/build \
		-G "MinGW Makefiles" \
		-DCMAKE_MAKE_PROGRAM="D:/CppCompilers/MinGW/mingw64/bin/mingw32-make.exe" \
		-DCMAKE_RC_COMPILER="D:/CppCompilers/LLVM/bin/llvm-rc.exe" \
		-DCMAKE_C_COMPILER="D:/CppCompilers/LLVM/bin/clang.exe" \
		-DCMAKE_CXX_COMPILER="D:/CppCompilers/LLVM/bin/clang++.exe"
else
	cmake . -B build/build \
		-G "MinGW Makefiles" \
		-DCMAKE_MAKE_PROGRAM="D:/CppCompilers/MinGW/mingw64/bin/mingw32-make.exe" \
		-DCMAKE_C_COMPILER="D:/CppCompilers/MinGW/mingw64/bin/gcc.exe" \
		-DCMAKE_CXX_COMPILER="D:/CppCompilers/MinGW/mingw64/bin/g++.exe"
fi


#	  --config RelWithDebInfo
cmake --build build/build \
      --config Release \
	  --target all \
	  -j 18
