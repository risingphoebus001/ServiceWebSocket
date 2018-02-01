#include "operationSql.h"


pthread_mutex_t operationSql::lock = PTHREAD_MUTEX_INITIALIZER;
operationSql::operationSql()
{
}


operationSql::~operationSql()
{
}
bool  operationSql::insertQuery(Connection *con, sql::PreparedStatement * prep_stmt) {
		bool isOk = prep_stmt->executeUpdate();
		delete prep_stmt;
		DBPool::ReleaseConnection(con);
		if (!isOk) {
			return false;
		}
		return true;
}