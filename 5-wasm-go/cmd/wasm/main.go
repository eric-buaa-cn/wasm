//go:build js && wasm

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"strconv"
	"syscall/js"
)

func formatJSON(str string) (string, error) {
	var prettyJSON bytes.Buffer
	const space4Indent = "    "
	if err := json.Indent(&prettyJSON, []byte(str), "", space4Indent); err != nil {
		return "", err
	}
	return prettyJSON.String(), nil
}

func formatJSONWrapper() js.Func {
	return js.FuncOf(func(this js.Value, args []js.Value) any {
		if len(args) != 1 {
			return map[string]any{
				"error": "Invalid # of arguments passed",
			}
		}

		inputJSON := args[0].String()
		fmt.Printf("formatJSON input: %s\n", inputJSON)
		pretty, err := formatJSON(inputJSON)
		if err != nil {
			fmt.Printf("unable to convert to json %s\n", err)
			return map[string]any{
				"error": err.Error(),
			}
		}
		return map[string]any{
			"value": pretty,
		}
	})
}

func fibonacciSlow(num int) int64 {
	if num == 0 {
		return 0
	}
	if num == 1 {
		return 1
	}

	return fibonacciSlow(num-1) + fibonacciSlow(num-2)
}

func fibonacciWrapper() js.Func {
	return js.FuncOf(func(this js.Value, args []js.Value) any {
		if len(args) != 1 {
			return map[string]any{
				"error": "Invalid # of arguments passed",
			}
		}

		number, _ := strconv.Atoi(args[0].String())
		fmt.Printf("wasmfib input: %v\n", number)
		fib := fibonacciSlow(number)

		return map[string]any{
			"value": fib,
		}
	})
}

func main() {
	fmt.Println("Go Web Assembly v2.0. This line goes to web browser Console.")
	js.Global().Set("formatJSON", formatJSONWrapper())
	js.Global().Set("fibonacciWasm", fibonacciWrapper())
	<-make(chan struct{})
}
