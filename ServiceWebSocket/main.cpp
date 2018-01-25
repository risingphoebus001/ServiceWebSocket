#include "protobuf/addressbook.pb.h"
#include "Aes/Aes.h"
#include <iostream>
#include <uWS/uWS.h>

int main()
{
	uWS::Hub h;

	h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		auto msg = std::string(message, length);
		char result[1024];
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
	});
	h.onConnection([](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
		std::cout << "on commection" << std::endl;
	});
	h.onError([](void *user) {
		std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
		exit(-1);
	});
	h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
		std::cout << "CLIENT CLOSE: " << code << std::endl;
	});

	if (h.listen(3000)) {
		h.connect("ws://localhost:3000", nullptr);
		h.run();
	}
}