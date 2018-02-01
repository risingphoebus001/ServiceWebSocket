#include "controller.h"
#include "../mysql/operationSql.h"
#include <functional>
bool controller::registerUser(std::string name, int passwd, std::string nameStr, std::string mail ) {
	Connection *con = DBPool::GetConnection();//get a db conn
	if (con) {
		sql::Statement* stmt = con->createStatement();
		//stmt->execute("USE MyGame");
		sql::PreparedStatement  * prep_stmt;
		prep_stmt = con->prepareStatement("INSERT INTO user(id, name,password,namestr,mail) VALUES (?, ?, ?, ?, ?);");
		std::hash<std::string> hs;
		std::cout << hs(name) << std::endl;
		prep_stmt->setInt(1, hs(name));
		prep_stmt->setString(2, name);
		prep_stmt->setInt(3,passwd);
		prep_stmt->setString(4, nameStr);
		prep_stmt->setString(5, mail);
		return operationSql::insertQuery(con,prep_stmt);
	}
	return false;
}
