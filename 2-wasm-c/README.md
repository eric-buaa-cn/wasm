```bash
# install emscripten using brew
# refer: https://wayou.github.io/2020/05/09/Emscripten-%E5%AE%89%E8%A3%85/
brew install emscripten

# convert c to wasm
emcc math.c -O3 --no-entry -o math.wasm -s EXPORTED_FUNCTIONS="['_add']"
```