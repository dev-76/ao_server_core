#ifndef COMMON_MYSQL_H
#define COMMON_MYSQL_H

#ifdef __WIN32
    #ifdef _DEBUG
        #pragma comment(lib, "mysqlclient_d.lib")
    #else
        #pragma comment(lib, "mysqlclient.lib")
    #endif
#endif

#ifdef __linux__
    #include <my_global.h>
    #include <mysql.h>
#endif

#include <my_global.h>
#include <mysql.h>

#endif // COMMON_MYSQL_H
