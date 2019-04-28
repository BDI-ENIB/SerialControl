//----lib header
#include "SerialControl.hpp"

//----standard libs
#include <iostream>
#include <exception>
#include <thread>
#include <chrono>
#include <string.h>


namespace SerialControl {

//----configuration values (macros for now)


using namespace std::chrono;

//----internal functions

namespace {

	bool isEmpty(std::fstream& file) {
    	return file.peek() == std::fstream::traits_type::eof();
	}
}


//----functions

std::vector<std::string> updateModules(){

	std::vector<std::string> moduleNames;

	//for each path to check
	for(const auto& basePath: paths) {

		//for each USB module connected
		for(int i=0; i<=MAX_INDEX; i++) {
			
			//setup
			std::string path = basePath + std::to_string(i);
			std::fstream file;
			Module mod;

			//open connection
			file.open(path, std::ios_base::in|std::ios_base::out);
			if(file.fail()) {
				if(DEBUG) std::cout << "debug:unable to open " << path  << '\n';
				continue;
			}
			mod.path = path;

			//send name request
			file.write("whois;",6);

			//receive name request response
			std::string response;
			char str[2] = "";
			
			while(file.read(str,1)) {
				std::cout << str << '\n';
				if(!strcmp(str,";")) { break; }
				response.push_back(*str);
			}

			if(!file.fail()) {
				mod.name = response;
				moduleList.emplace_back(mod);
				moduleNames.emplace_back(response);
			}
			
			//because magic
			std::this_thread::sleep_for(5s);

			//end connection
			file.close();
		}
	}
	return moduleNames;

}


std::string sendCommand(const std::string& cmd, const std::string& mod) {
	for(auto &elem: moduleList) {
		if(elem.name == mod) {

			//setup
			std::fstream file;

			//open connection
			file.open(elem.path, std::ios_base::in|std::ios_base::out);
			if(file.fail()) {
				if(ERROR) std::cerr << "unable to open communication with " << elem.name << '\n';
				continue;
			}

			//send command
			file << cmd << std::endl;

			//receive response
			std::string response;
			file >> response;
			
			//because magic
			std::this_thread::sleep_for(2s);

			//file.close is called implicitly

			return response;
		}
	}
	return "no_response";
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
			file.open(elem.path, std::ios_base::in);
			if(file.fail()) {
				if(DEBUG) std::cerr << "unable to open communication with " << elem.name << '\n';
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

