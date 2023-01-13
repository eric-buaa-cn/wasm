This demo shows how to do wasm using golang. It follows the docs at `https://golangbot.com/webassembly-using-go/` but
with little change by merging result and error into an object. I don't like to directly manipulate html doc in golang
source code.

1. Prepare

In webassembly directory, cross compile golang source code into wasm binary code.
```shell
GOOS=js GOARCH=wasm go build -o assets/json.wasm cmd/wasm/main.go
```
2. Run

In webassembly directory, run the web server.
```shell
go run cmd/server/main.go
```
3. Access

Open your web browser, access the local web server.
```shell
http://localhost:9090
```