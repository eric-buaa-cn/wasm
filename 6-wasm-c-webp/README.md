> This demo shows how to convert an image to WebP format.

1. Prepare

We migrate WebP C source code to WASM. Firstly, clone WebP project from github.
```shell
git clone https://github.com/webmproject/libwebp
```
2. Compile

Compile the wrapper webp.c and code from WebP project to get webp.js and webp.wasm.
```shell
emcc -O3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -I libwebp webp.c libwebp/src/{dec,dsp,demux,enc,mux,utils}/*.c libwebp/sharpyuv/*.c -o webp.js
```

3. Access

Launch your web browser, access the local web server.
```shell
emrun --browser google webp.html
```

4. Refers
- https://developer.mozilla.org/en-US/docs/WebAssembly/existing_C_to_wasm#get_an_image_from_javascript_into_wasm
- http://talkerscode.com/howto/upload-image-in-html-and-display.php