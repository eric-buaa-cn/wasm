```shell
> wat2wasm answer.wat -v
```

```text
0000000: 0061 736d                                 ; WASM_BINARY_MAGIC
0000004: 0100 0000                                 ; WASM_BINARY_VERSION
; section "Type" (1)
0000008: 01                                        ; section code
0000009: 00                                        ; section size (guess)
000000a: 01                                        ; num types
; func type 0
000000b: 60                                        ; func
000000c: 00                                        ; num params
000000d: 01                                        ; num results
000000e: 7f                                        ; i32
0000009: 05                                        ; FIXUP section size
; section "Function" (3)
000000f: 03                                        ; section code
0000010: 00                                        ; section size (guess)
0000011: 01                                        ; num functions
0000012: 00                                        ; function 0 signature index
0000010: 02                                        ; FIXUP section size
; section "Export" (7)
0000013: 07                                        ; section code
0000014: 00                                        ; section size (guess)
0000015: 01                                        ; num exports
0000016: 06                                        ; string length
0000017: 616e 7377 6572                           answer  ; export name
000001d: 00                                        ; export kind
000001e: 00                                        ; export func index
0000014: 0a                                        ; FIXUP section size
; section "Code" (10)
000001f: 0a                                        ; section code
0000020: 00                                        ; section size (guess)
0000021: 01                                        ; num functions
; function body 0
0000022: 00                                        ; func body size (guess)
0000023: 00                                        ; local decl count
0000024: 41                                        ; i32.const
0000025: 00                                        ; i32 literal
0000026: 0b                                        ; end
0000022: 04                                        ; FIXUP func body size
0000020: 06                                        ; FIXUP section size
```

```shell
> echo "0061 736d" -n | xxd -r -p
> "\0asm"
```
