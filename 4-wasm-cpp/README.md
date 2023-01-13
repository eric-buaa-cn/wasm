```bash
# install emscripten using brew
# refer: https://wayou.github.io/2020/05/09/Emscripten-%E5%AE%89%E8%A3%85/
brew install emscripten

# convert c to wasm
emcc math.cpp --bind -s MODULARIZE -s EXPORT_NAME=MathModule -o math.js
```