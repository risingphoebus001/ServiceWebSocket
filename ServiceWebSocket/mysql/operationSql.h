#pragma once
#include "DBPool.h"
class operationSql
{
private:
	static pthread_mutex_t lock;
public:
	virtual ~operationSql();
	static  bool insertQuery(Connection *con, sql::PreparedStatement * prep_stmt);
private:
	operationSql();
};

