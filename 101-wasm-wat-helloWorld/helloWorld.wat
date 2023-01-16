(module
    ;; $js_print has two parameters, offset in memory and string length
    (import "js" "print" (func $js_print (param i32 i32)))
    (import "js" "mem" (memory 1))

    (data (i32.const 0) "hello, world")

    (func $helloWorld
        i32.const 0     ;; offset in memory
        i32.const 12    ;; string length
        call $js_print
    )

    (export "hello" (func $helloWorld))
)