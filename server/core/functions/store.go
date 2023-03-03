package store

import (
        "busybot/core/database"
        configuration "busybot/core/config"
        "busybot/core/handler"
)

var Cfg        *configuration.Config = configuration.JsonLoad()
var Db         *database.Database    = database.NewDatabase(Cfg.Database.Host, Cfg.Database.Username, Cfg.Database.Password, Cfg.Database.Name)
var ClientLst  *handle.ClientList    = handle.ClientLists

