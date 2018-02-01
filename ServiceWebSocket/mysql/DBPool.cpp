#include <iostream>  
#include <stdexcept>    
#include <exception>    
#include <stdio.h>    
#include "DBPool.h"  

using namespace std;
using namespace sql;

pthread_mutex_t DBPool::lock = PTHREAD_MUTEX_INITIALIZER;
DBPool* DBPool::instance_ = DBPool::GetInstance();
//Singleton: get the single object
DBPool::DBPool() {
		
}
DBPool*& DBPool::GetInstance()
{
	if (!instance_) {
		instance_ = new DBPool();
		instance_->initPool();
	}
	return instance_;
}

void DBPool::initPool()
{
	this->curSize = 0;
	try {
		this->driver = static_cast<Driver*>(sql::mysql::get_driver_instance());
	}
	catch (sql::SQLException& e)
	{
		perror("Get sql driver failed");
	}
	catch (std::runtime_error& e)
	{
		perror("Run error");
	}
	this->InitConnection(maxSize);
}

//init conn pool  
void DBPool::InitConnection(int initSize)
{
	Connection* conn;
	pthread_mutex_lock(&lock);
	for (int i = 0;i <initSize; i++)
	{
		conn = this->CreateConnection();

		if (conn)
		{
			connList.push_back(conn);
			++(this->curSize);
		}
		else
		{
			perror("create conn error");
		}
	}
	pthread_mutex_unlock(&lock);
}

Connection* DBPool::CreateConnection()
{
	Connection* conn;
	try {
		conn = driver->connect(this->url, this->user, this->password);  //create a conn   
		conn->setSchema("MyGame");
		return conn;
	}
	catch (sql::SQLException& e)
	{
		perror("link error");
		return NULL;
	}
	catch (std::runtime_error& e)
	{
		perror("run error");
		return NULL;
	}
}

Connection*  DBPool::GetConnection()
{
	Connection* conn = nullptr;

	if (DBPool::GetInstance()->connList.size()>0)//the pool have a conn   
	{
		pthread_mutex_lock(&lock);
		conn = DBPool::GetInstance()->connList.front();
		DBPool::GetInstance()->connList.pop_front();//move the first conn  
		pthread_mutex_unlock(&lock);
		if (conn->isClosed())//if the conn is closed, delete it and recreate it  
		{
			delete conn;
			conn = nullptr;
			conn = DBPool::GetInstance()->CreateConnection();
		}

		if (conn == NULL)
		{
			--DBPool::GetInstance()->curSize;
		}
		return conn;
	}
	else
	{
		//if (DBPool::GetInstance()->curSize< DBPool::GetInstance()->maxSize)//the pool no conn  
		//{
		//	conn = DBPool::GetInstance()->CreateConnection();
		//	if (conn)
		//	{
		//		++DBPool::GetInstance()->curSize;
		//		return conn;
		//	}
		//	else
		//	{
		//		return nullptr;
		//	}
		//}
		//else //the conn count > maxSize  
		//{
			return nullptr;
	//	}
	}
}

//put conn back to pool  
void DBPool::ReleaseConnection(Connection * &conn)
{
	if (conn)
	{
		pthread_mutex_lock(&lock);
		DBPool::GetInstance()->connList.push_back(conn);
		pthread_mutex_unlock(&lock);
	}
}

void DBPool::DestoryConnPool()
{
	list<Connection*>::iterator iter;
	pthread_mutex_lock(&lock);
	for (iter = connList.begin(); iter != connList.end(); ++iter)
	{
		this->DestoryConnection(*iter);
	}
	curSize = 0;
	connList.clear();
	pthread_mutex_unlock(&lock);
}


void DBPool::DestoryConnection(Connection* conn)
{
	if (conn)
	{
		try {
			conn->close();
		}
		catch (sql::SQLException&e)
		{
			perror(e.what());
		}
		catch (std::exception& e)
		{
			perror(e.what());
		}
		delete conn;
	}
}

DBPool::~DBPool()
{
	this->DestoryConnPool();
}