#include "protobuf/addressbook.pb.h"
#include "Aes/Aes.h"
#include <iostream>
#include <uWS/uWS.h>
#include <list>
#include <stdio.h>
std::mutex mtx;
std::list<uWS::WebSocket<uWS::SERVER>*>* list = new std::list<uWS::WebSocket<uWS::SERVER>*>;


int main()
{
	uWS::Hub h;
	std::list<uWS::WebSocket<uWS::SERVER>*>* list = new std::list<uWS::WebSocket<uWS::SERVER>*>;
	h.onMessage([&list](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		
		auto msg = std::string(message, length);
		if (!msg.compare("1")) {
			if (mtx.try_lock()){
				list->push_back(ws);
				mtx.unlock();
			}
		}
		else {
			if (list) {
				std::list<uWS::WebSocket<uWS::SERVER>*>::iterator itr = list->begin();
				for (;itr != list->end();itr++) {
					if (*itr != ws) {
						msg = std::to_string(list->size());
						(*itr)->send(msg.c_str(), sizeof(msg) / sizeof(decltype(msg)), opCode);
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
		std::cout << "on commection:"+count << std::endl;
	});
	h.onError([](void *user) {
		std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
		exit(-1);
	});
	h.onDisconnection([&list](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
		if (mtx.try_lock()) {
			list->remove(ws);
			mtx.unlock();
		}
		std::cout << "CLIENT CLOSE: " << code << std::endl;
	});

	if (h.listen(3000)) {
		//h.connect("ws://localhost:3000", nullptr);
		h.run();
	}
}