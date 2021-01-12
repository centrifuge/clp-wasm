
# Build LP solver with emscripten

docker build . -t tinlake-wasm-builder

docker run -v $PWD:/app tinlake-wasm-builder