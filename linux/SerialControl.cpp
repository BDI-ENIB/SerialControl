//----lib header
#include "SerialControl.hpp"

//----standard libs
#include <iostream>
#include <exception>

namespace SerialControl {

//----configuration values (macros for now)

#define MAX_INDEX 10
#define TIMEOUT 10


//----internal functions

namespace {

	bool isEmpty(std::fstream& file) {
    	return file.peek() == std::fstream::traits_type::eof();
	}
}


//----functions

std::vector<std::string> updateModules(){

	std::vector<std::string> moduleNames;
	//for each USB module connected
	for(int i=0; i<=MAX_INDEX; i++) {
		
		std::string path = "/dev/ttyACM" + std::to_string(i);
		std::fstream file;

		file.open(path, std::ios_base::in|std::ios_base::out);
		if(file.fail()) { if(DEBUG) std::cerr << "unable to open " << path << '\n';}
		file << "whois;" << std::endl;
		std::string response = "";
		
		getline(file,response,'\n');
		if(!file.fail()) {
			std::cout << response;
			Module mod = {false, i, response};
			moduleList.emplace_back(mod);
			moduleNames.emplace_back(response);
		}

		file.close();
	
	}
	return moduleNames;

}


std::string SendCommand(const std::string& cmd, const std::string& mod) {
	for(auto &elem: moduleList) {
		if(elem.name == mod) {
			std::fstream file;
			file.open("/dev/ttyACM" + std::to_string(elem.id), std::ios_base::in|std::ios_base::out);
			if(file.fail()) {
				std::cerr << "unable to open communication with " << mod << '\n';
				return "no response";
			}
			file << cmd;
			std::string response;
			getline(file,response,'\n');
			file.close();
			return response;
		}
	}
	return "no response";
}

int watch(const std::string& mod) {
	for(auto &elem: moduleList) {
		if(elem.name == mod) {
			elem.watch = true;
			return 0;
		}
	}
	return 1;
}

std::vector<std::tuple<std::string,std::string>> update() {
	std::vector<std::tuple<std::string,std::string>> output;
	for(auto &elem: moduleList) {
		if(elem.watch) {
			std::fstream file;
			file.open("/dev/ttyACM" + std::to_string(elem.id), std::ios_base::in);
			if(file.fail()) {
				std::cerr << "unable to open communication with " << elem.name << '\n';
				continue;
			}
			if(isEmpty(file)){
				file.close();
				continue;
			}
			std::string response;
			file >> response;
			output.emplace_back(std::make_tuple(elem.name,response));
		}
	}
	return output;
}


} //namespace SerialControl

