package config

import (
        "os"
        "fmt"
        "encoding/json"
        "sync"
)

var config *Config
var once sync.Once

type Config struct {
    Database struct {
        Name     string `json:"name"`
        Username string `json:"username"`
        Password string `json:"password"`
        Host     string `json:"host"`
    } `json:"database"`
    Masters struct {
        Host      string   `json:"host"`
        Whitelist []string `json:"whitelist"`
        Bot struct {
            Host      string `json:"host"`
        } `json:"bot"`
    } `json:"masters"`
}

func JsonLoad() (*Config) {
    once.Do(func() {
        file, _ := os.Open("assets/config.json")
        defer file.Close()

        decoder := json.NewDecoder(file)
        config = &Config{}
        err := decoder.Decode(config)
        if err != nil {
            fmt.Println(err)
            os.Exit(0)
        }
        fmt.Println("[Busybot] - JSON configuration was successfully loaded")
    })
    return config
}
