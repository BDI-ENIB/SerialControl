#include "SerialControl.hpp"

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main() {

	auto modules = SerialControl::listModules(); //init the modules at least once
	for(const auto &elem: modules) {
		std::cout << elem->name << '\n';
		for(int i=0; i<2; i++) {
			//just to get name again, do whatever you want from here on
			std::cout << i << ": " <<  elem->sendCommand("whois;") << '\n';
		}
		std::cout << elem->sendCommand("activate;") << '\n';
		//to add a callback
		elem->watch([](const std::string& str) { std::cout << str << '\n'; });
	}

	//don't forget a loop, or any form of repetition to call update() if you use callbacks
	while(true){
		SerialControl::update();
	}

	return 0;
}

