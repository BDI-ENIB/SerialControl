#ifndef SERIAL_CONTROL_HPP
#define SERIAL_CONTROL_HPP

//----standard libs
#include <string>
#include <vector>
#include <fstream>
#include <tuple>

//----c libs
#include <termios.h>

namespace SerialControl {

//----configuration values (macros for now)

#define MAX_INDEX 10
#define MAX_MESSAGE_SIZE 256 //in number of chars
#define DEBUG 1
#define BAUDRATE 9600


//----private variables

class Module {

	public:

	bool watch;
	const std::string name;

	Module(bool watch, const std::string name, const int fd, const struct termios oldAttr):
		watch{watch}, name{name}, fileDescriptor{fd}, oldAttr{oldAttr} {}

	private:
	const int fileDescriptor;
	const struct termios oldAttr;

};




//----functions

/**
 * Update the list of modules connected
 * return value : a list of the modules names (given by the modules)
 */
std::vector<Module> listModules();

/**
 * Send a command to the specified module
 */
std::string sendCommand(const std::string& cmd, const std::string& mod); 

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

