#include "SerialControl.hpp"

#include <iostream>
#include <vector>

int main() {

	auto modules = SerialControl::updateModules();
	for(const auto &elem: modules){
		std::cout << elem;
	}
	return 0;
}
