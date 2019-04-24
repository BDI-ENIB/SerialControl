#ifndef SERIAL_CONTROL_HPP
#define SERIAL_CONTROL_HPP

//----standard libs
#include <string>
#include <vector>
#include <fstream>
#include <tuple>

namespace SerialControl {

//----configuration values (macros for now)

#define MAX_INDEX 10
#define TIMEOUT 10
#define DEBUG 1


//----private variables

namespace {

	typedef struct {
		bool watch = false;
		const int id = 0;
		const std::string name = "no_device";
	} Module;

	std::vector<Module> moduleList;

}


//----functions

/**
 * Update the list of modules connected
 * return value : a list of the modules names (given by the modules)
 */
std::vector<std::string> updateModules();

/**
 * Send a command to the specified module
 */
std::string SendCommand(const std::string& cmd, const std::string& mod); 

/**
 * Set a watch flag on the specified module so that it output will be checked
 * at each update
 */
int watch(const std::string& mod);

/**
 * checked alll watched modules and return a list of all the reponses found
 * with the name of the module that send them
 */
std::vector<std::tuple<std::string,std::string>> update();

} //namespace SerialControl

#endif

