#ifndef MYSQL_RESULT_SET_H
#define MYSQL_RESULT_SET_H

#include <memory>
#include <vector>
#include "common_mysql.h"
#include "mysql_row.h"

namespace AO
{
    class MySQLResultSet {
    public:
        MySQLResultSet();
        MySQLResultSet(MYSQL_RES* pResult);
        ~MySQLResultSet();

        MySQLRow* Fetch();
        bool Empty();

        void Reset();
        void ResetIter();

    private:
        std::vector<MySQLRow*> m_Rows;
        unsigned long m_CurrentRowIndex;
        unsigned long long m_RowsCount;
        unsigned long m_FieldsCount;
    };
}

#endif // !MYSQL_RESULT_SET_H
