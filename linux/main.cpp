#include "SerialControl.hpp"

#include <iostream>
#include <vector>

int main() {

	auto modules = SerialControl::listModules();
	for(const auto &elem: modules) {
		std::cout << elem.name << '\n';
	}
	/*for(const auto &elem: modules) {
		std::cout << elem << ": " << SerialControl::sendCommand("whois;",elem) << '\n';
	}*/
	return 0;
}

