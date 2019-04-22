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

//----configuration values (macros for now)

#define MAX_INDEX 10
#define TIMEOUT 10


//----private variables

namespace {

	typedef struct {
		const int id = 0;
		std::fstream* file = nullptr;
		const std::string name = "no_device";
	} module;

	std::vector<module> moduleList;

}


//----functions

std::vector<std::string> updateModules(){

	std::vector<std::string> moduleNames;
	//for each USB module connected
	for(int i=0; i<=MAX_INDEX; i++) {
		
		std::string path = "/dev/ttyACM" + std::to_string(i);
		std::fstream file;

		//std::this_thread::sleep_for(0.06s); //necessary when using open() at high rates

		file.open(path, std::ios_base::in|std::ios_base::out);
		if(file.fail()) {std::cerr << "unable to open " << path << '\n';}
		file << "whois;" << std::endl;
		std::string response = "";

		getline(file,response,'\n');
		if(!file.fail()) {
			module mod = {i, nullptr, response};
			moduleList.emplace_back(mod);
			moduleNames.emplace_back(response);
		}

		file.close();
	
	}
	return moduleNames;

}


} //namespace SerialControl

