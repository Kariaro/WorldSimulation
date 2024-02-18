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

export OPENSSL_ROOT_DIR="D:/CppProjects/dependencies/openssl-3/x64"

if [ "$1" == "--clean" ]; then
	rm -rf build/
fi

function common_cmake()
{
	cmake . -B build/build \
		-G "MinGW Makefiles" \
		-DOPENSSL_ROOT_DIR="${OPENSSL_ROOT_DIR}" \
		"$@"
}

USE_CLANG="false"
if [[ "$USE_CLANG" == "true" ]]; then
	common_cmake \
		-DCMAKE_MAKE_PROGRAM="D:/CppCompilers/MinGW/mingw64/bin/mingw32-make.exe" \
		-DCMAKE_RC_COMPILER="D:/CppCompilers/LLVM/bin/llvm-rc.exe" \
		-DCMAKE_C_COMPILER="D:/CppCompilers/LLVM/bin/clang.exe" \
		-DCMAKE_CXX_COMPILER="D:/CppCompilers/LLVM/bin/clang++.exe"
else
	common_cmake \
		-DCMAKE_MAKE_PROGRAM="D:/CppCompilers/MinGW/mingw64/bin/mingw32-make.exe" \
		-DCMAKE_RC_COMPILER="D:/CppCompilers/MinGW/mingw64/bin/windres.exe" \
		-DCMAKE_C_COMPILER="D:/CppCompilers/MinGW/mingw64/bin/gcc.exe" \
		-DCMAKE_CXX_COMPILER="D:/CppCompilers/MinGW/mingw64/bin/g++.exe"
fi


cmake --build build/build \
	--config RelWithDebInfo \
	--target all \
	-j 18 && \
ctest --test-dir build/build --output-on-failure
