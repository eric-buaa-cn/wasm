```shell
emcc main.cpp glad.cpp -std=c++11 -s WASM=1 -s USE_GLFW=3 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s MODULARIZE -s EXPORT_ES6 -s EXPORT_NAME=createModule -O3 -o index.js -I .