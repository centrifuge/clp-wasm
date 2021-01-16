
:: git clone https://github.com/emscripten-core/emsdk.git
:: cd emsdk
:: emsdk install latest


call C:\Code\Libs\emsdk\emsdk_env.bat

set EM_CONFIG=%~dp0/.emscripten
cmake -H%~dp0 -B%~dp0build_wasm -DCMAKE_MAKE_PROGRAM=ninja^
  -DCMAKE_MODULE_PATH=%EMSDK%/upstream/emscripten/cmake^
  -DCMAKE_TOOLCHAIN_FILE=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake


