package serve

import (
        "os"
        "fmt"
        "net/http"

        "busybot/core/functions"
)

func InitServer() {
    err := http.ListenAndServeTLS(store.Cfg.Masters.Host, "assets/tls/cert.pem", "assets/tls/key.pem", nil)
    fmt.Println("[Busybot] - Started HTTPS connection successfully on '"+store.Cfg.Masters.Host+"'.")
    if err != nil {
	fmt.Println(err)
        os.Exit(0)
    }
}

func isTor(ip string) bool {
    torNodes := store.Cfg.Masters.Whitelist

    for _, node := range torNodes {
        if node == ip {
            return true
        }
    }

    return false
}
