//lib header
#include "SerialControl.hpp"

//standard libs
#include <iostream>
#include <fstream>
#include <exception>
//#include <chrono>
//#include <thread>

namespace SerialControl {

//using namespace std::chrono_literals;

#define MAX_INDEX 10
#define TIMEOUT 10

std::vector<std::string> initModules() {
	std::vector<std::string> list;

	//for each USB module connected
	for(int i=0; i<=MAX_INDEX; i++) {
		
		std::string path = "/dev/ttyACM" + std::to_string(i);
		std::fstream file;
		//std::this_thread::sleep_for(0.06s); //necesserry when using open quikly
		try { file.open(path, std::ios_base::in|std::ios_base::out); }
		catch(std::exception& e) { std::cerr << e.what(); }
		if(file.fail()) {
			std::cerr << "unable to open " << path << '\n';
		}
		file << "whois;" << std::endl;
		std::string response = "";
		try {
			getline(file,response,';');
		}
		catch(std::exception& e) {
			std::cerr << e.what();
		}
		file.close();
		list.emplace_back(response);
		
	}
	return list;
}


} //namespace SerialControl

