package main

import (
        "fmt"

        "busybot/core/server"
        "busybot/core/handler"
        "busybot/core/functions"
)


func main() {
    fmt.Println("[BusyBot] - Initializing configuration...")
    fmt.Println("[BusyBot] - OK!, Initializing MySQL server...")
    fmt.Println("[BusyBot] - OK!, Initializing Slave server...")
    go handle.InitBots(store.Cfg)
    fmt.Println("[BusyBot] - OK!, Initializing HTTP server...")
    go serve.InitCmds()
    serve.InitServer()
}
