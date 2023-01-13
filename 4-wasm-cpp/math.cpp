int add(int a, int b) {
    return a + b;
}

int sub(int a, int b) {
    return a - b;
}

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(math_module) {
    emscripten::function("add", &add);
    emscripten::function("sub", &sub);
}