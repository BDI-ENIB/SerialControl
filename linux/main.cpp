#include "SerialControl.hpp"

#include <iostream>
#include <vector>

int main() {

	auto modules = SerialControl::updateModules();
	for(const auto &elem: modules){
		std::cout << elem;
	}
	for(const auto &elem: modules){
		if(elem == "I'm Groot"){
			std::cout << SerialControl::SendCommand("whois;",elem);
		}
	}
	return 0;
}

