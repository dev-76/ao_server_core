#ifndef MYSQL_INTERFACE_H
#define MYSQL_INTERFACE_H

#include "mysql_database.h"

namespace AO {

    typedef MySQLField DBField;
    typedef std::shared_ptr<MySQLRow> DBRow;
    typedef std::shared_ptr<MySQLResultSet> DBResSet;
    typedef MySQLDatabase Database;

    extern Database* WorldDatabase;
    extern Database* CharactersDatabase;
    extern Database* AccountsDatabase;

}

#endif // !MYSQL_INTERFACE_H
