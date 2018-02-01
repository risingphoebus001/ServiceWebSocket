#include "protobuf/addressbook.pb.h"
#include "Aes/Aes.h"
#include <iostream>
#include <uWS/uWS.h>
#include <list>
#include <functional>
#include <stdio.h>
#include "mysql/DBPool.h"
#include "controller/controller.h"
#include <thread>
std::mutex mtx;
std::list<uWS::WebSocket<uWS::SERVER>*>* listws = new std::list<uWS::WebSocket<uWS::SERVER>*>();
int main()
{
	static int ll = 0;
	std::vector<std::thread *> threads(std::thread::hardware_concurrency());
	std::transform(threads.begin(), threads.end(), threads.begin(), [](std::thread *t) {
		return new std::thread([]() {
			uWS::Hub h;
			h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
				auto msg = std::string(message, length);
				if (!msg.compare("1")) {
					if (mtx.try_lock()) {
						//listws->push_back(ws);
						controller::registerUser(std::to_string(ll), 521444);
						ll++;
						mtx.unlock();
					}
				}
				else if(!msg.compare("3")){
					if (listws) {
						std::list<uWS::WebSocket<uWS::SERVER>*>::iterator itr = listws->begin();
						for (;itr != listws->end();itr++) {
							if (*itr != ws) {
								msg = std::to_string(listws->size());
								(*itr)->send(msg.c_str(), msg.length(), opCode);
							}
						}
					}
				}

				/*char* result = new char[length];
				AES::getInstance()->InvCipher((char*)msg.c_str(), result);
				tutorial::Data data;
				data.ParseFromString(result);
				std::cout << "data.messagetype()" + data.messagetype() << std::endl;
				if (data.messagetype() == tutorial::Data::DataType::Data_DataType_login) {
				std::cout << "login" << std::endl;
				for (int i = 0; i < data.persons_size(); i++) {
				tutorial::Data_Person person = data.persons(i);
				std::cout << person.name() << std::endl;
				ws->send(message, length, opCode);
				}
				}
				else if (data.messagetype() == tutorial::Data::DataType::Data_DataType_addPerson) {
				std::cout << "message1" << std::endl;
				for (int i = 0; i < data.persons_size(); i++) {
				tutorial::Data_Person person = data.persons(i);

				std::cout << person.name() << std::endl;
				ws->send(message, length, opCode);
				}

				}
				delete result;*/
			});
			h.onConnection([](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
				static int count = 0;
				count++;
				std::cout << "on commection:" + count << std::endl;
			});
			h.onError([](void *user) {
				std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
				exit(-1);
			});
			h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
				if (mtx.try_lock()) {
					listws->remove(ws);
					mtx.unlock();
				}
				std::cout << "CLIENT CLOSE: " << code << std::endl;
			});
			if (!h.listen(3000, nullptr, uS::ListenOptions::REUSE_PORT)) {
				std::cout << "Failed to listen" << std::endl;
			}
			h.run();
		});
	});
	std::for_each(threads.begin(), threads.end(), [](std::thread *t) {
		t->join();
	});
}
	//初始化连接，创建参数中maxSize一半的连接  
	//DBPool* connpool = DBPool::GetInstance();
	//connpool->
	//Connection *con;
	//Statement *state;
	//ResultSet *result;
	//con = DBPool::GetInstance()->GetConnection();//get a db conn  
	//for (int i = 0; i<1000; i++)
	//{
	//	state = con->createStatement();
	//	state->execute("use mysql");

	//	// 查询  
	//	result = state->executeQuery("select host,user from user");

	//	// 输出查询  
	//	while (result->next())
	//	{
	//		try {
	//			string user = result->getString("user");
	//			string name = result->getString("host");
	//			cout << user << " : " << name << endl;
	//		}
	//		catch (sql::SQLException& e) {
	//			//std::cout << e.what() << std::endl;
	//		}
	//	}

		/*result = state->executeQuery("select cust_id,cust_name from customers");
		while (result->next())
		{
		try{
		string user = result->getString("cust_id");
		string name = result->getString("cust_name");
		cout << user << " : " << name << endl;
		}catch(sql::SQLException& e){
		std::cout << e.what() << std::endl;
		}
		}
		*/
		//std::cout << i << std::endl;

	//}

	/*delete result;
	delete state;
	DBPool::GetInstance()->ReleaseConnection(con);*/

	/*sql::Connection *con;
	sql::PreparedStatement  *prep_stmt;
	prep_stmt = con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)");

	prep_stmt->setInt(1, 1);
	prep_stmt->setString(2, "a");
	prep_stmt->execute();

	prep_stmt->setInt(1, 2);
	prep_stmt->setString(2, "b");
	prep_stmt->execute();

	delete prep_stmt;
	delete con;*/

	//controller::registerUser("wo",521444);
	/*std::hash<std::string> hs;
	size_t n = hs("2344_)d@44");
	std::cout << n << std::endl;*/

	//if (h.listen(3000)) {
	//	//h.connect("ws://localhost:3000", nullptr);
	//	h.run();
	//}
