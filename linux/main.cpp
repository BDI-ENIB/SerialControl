#include "SerialControl.hpp"

#include <iostream>
#include <vector>

int main() {

	auto output = SerialControl::initModules();
	for(const auto &elem: output){
		std::cout << elem;
	}
	return 0;
}
