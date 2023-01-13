```bash
# install emscripten using brew
# refer: https://wayou.github.io/2020/05/09/Emscripten-%E5%AE%89%E8%A3%85/
brew install emscripten

# convert c to wasm
emcc math.cpp --bind -sWASM=1 -o math.js
```