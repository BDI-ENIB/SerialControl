#include "SerialControl.hpp"

#include <iostream>
#include <vector>

int main() {

	auto modules = SerialControl::listModules();
	for(const auto &elem: modules) {
		std::cout << elem->name << '\n';
		for(int i=0; i<10; i++) {
			std::cout << elem->sendCommand("whois;") << '\n';
		}
		elem->watch([](const std::string& str) { std::cout << str << '\n'; });
	}

	while(true){
		SerialControl::update();
	}

	return 0;
}

