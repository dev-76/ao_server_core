#include <iostream>

#include "system/conf/config.h"
#include "system/logger/logger.h"
#include "system/database/mysql_interface.h"

bool InitializeDatabases() {
    std::string host, port, user, pass, dbName;
    size_t syncConnections, asyncConnections;

    host = sConf->GetStringOption("Database.World.Host");
    port = sConf->GetStringOption("Database.World.Port");
    user = sConf->GetStringOption("Database.World.User");
    pass = sConf->GetStringOption("Database.World.Password");
    dbName = sConf->GetStringOption("Database.World.DbName");
    syncConnections = sConf->GetUIntOption("Database.World.SyncConnections");
    asyncConnections = sConf->GetUIntOption("Database.World.AsyncConnections");
    AO::WorldDatabase = new AO::Database(host, port, user, pass, dbName, syncConnections, asyncConnections);

    host = sConf->GetStringOption("Database.Characters.Host");
    port = sConf->GetStringOption("Database.Characters.Port");
    user = sConf->GetStringOption("Database.Characters.User");
    pass = sConf->GetStringOption("Database.Characters.Password");
    dbName = sConf->GetStringOption("Database.Characters.DbName");
    syncConnections = sConf->GetUIntOption("Database.Characters.SyncConnections");
    asyncConnections = sConf->GetUIntOption("Database.Characters.AsyncConnections");
    AO::CharactersDatabase = new AO::Database(host, port, user, pass, dbName, syncConnections, asyncConnections);

    host = sConf->GetStringOption("Database.Accounts.Host");
    port = sConf->GetStringOption("Database.Accounts.Port");
    user = sConf->GetStringOption("Database.Accounts.User");
    pass = sConf->GetStringOption("Database.Accounts.Password");
    dbName = sConf->GetStringOption("Database.Accounts.DbName");
    syncConnections = sConf->GetUIntOption("Database.Accounts.SyncConnections");
    asyncConnections = sConf->GetUIntOption("Database.Accounts.AsyncConnections");
    AO::AccountsDatabase = new AO::Database(host, port, user, pass, dbName, syncConnections, asyncConnections);

    if(!AO::WorldDatabase->InitializeConnections() || !AO::CharactersDatabase->InitializeConnections() || !AO::AccountsDatabase->InitializeConnections()) {
        return false;
    }
    return true;
}

void DeinitializeDatabases() {
    if(AO::WorldDatabase) {
        if(AO::WorldDatabase->IsInitialized()) {
            AO::WorldDatabase->DestroyConnections();
        }
        delete AO::WorldDatabase;
    }
    if(AO::CharactersDatabase) {
        if(AO::CharactersDatabase->IsInitialized()) {
            AO::CharactersDatabase->DestroyConnections();
        }
        delete AO::CharactersDatabase;
    }
    if(AO::AccountsDatabase) {
        if(AO::AccountsDatabase->IsInitialized()) {
            AO::AccountsDatabase->DestroyConnections();
        }
        delete AO::AccountsDatabase;
    }
}

int main(int argv, char** args)
{
    std::cout << "Trying to start Allod Online core server ..." << std::endl;
    sConf->SetFilename("server.conf");
    if(!sConf->Read()) {
        std::cout << "Error when parse configuration" << std::endl;
        exit(-1);
    }
    sLog->Open("logfile.log", AO::L_DEBUG);

    if(InitializeDatabases()) {
        AO::DBResSet res = AO::WorldDatabase->Query("SELECT id, name, completed, date_completed FROM daily_quests");
        if(res) {
            AO::DBRow row;
            while(row = res->Fetch()) {
                std::cout << "Daily quest id: " << (*row)[0]->ToStr() << " name: " << (*row)[1]->ToStr() << " completed: " << (*row)[2]->ToStr() << " date_completed: "
                          << (*row)[3]->ToStr() << std::endl;
            }
        }
        DeinitializeDatabases();
    }
    return 0;
}

