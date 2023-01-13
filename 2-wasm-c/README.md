```bash
# install emscripten using brew
# refer: https://wayou.github.io/2020/05/09/Emscripten-%E5%AE%89%E8%A3%85/
brew install emscripten

# convert c to wasm, refer here for the underscore prefix in each exported functions: 
# https://kapadia.github.io/emscripten/2013/09/13/emscripten-pointers-and-pointers.html
emcc math.c -O3 --no-entry -o math.wasm -s EXPORTED_FUNCTIONS="['_add','_sub']"
```