#pragma once
#include <mysql_connection.h>    
#include <mysql_driver.h>    
#include <cppconn/exception.h>    
#include <cppconn/driver.h>    
#include <cppconn/connection.h>    
#include <cppconn/resultset.h>    
#include <cppconn/prepared_statement.h>    
#include <cppconn/statement.h>    
#include <pthread.h>    
#include <list> 
using namespace std;
using namespace sql;
class DBPool
{
private:
	const string user{ "root" };
	const string password { "12345678" };
	const string url{ "101.201.239.148" };
	const int maxSize{ 150 };
	int curSize;
private:
	DBPool();
	//init DB pool  
	void InitConnection(int initSize);

	// create a connection  
	Connection* CreateConnection();

	//destory connection  
	void DestoryConnection(Connection *conn);

	//destory db pool  
	void DestoryConnPool();
private:
	

	Driver*             driver;     //sql driver (the sql will free it)  
	list<Connection*>   connList;   //create conn list  

									//thread lock mutex  
	static pthread_mutex_t lock;
	static DBPool* instance_;


	//init pool  
	void initPool();

	// Singleton
	static DBPool*& GetInstance();
public:
	virtual ~DBPool();
	
	//get a conn from pool  
	static Connection* GetConnection();

	//put the conn back to pool  
	static void ReleaseConnection(Connection * &conn);
};

