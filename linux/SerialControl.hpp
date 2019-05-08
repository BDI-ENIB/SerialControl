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
#define WRITE_TRY_NB 3 	//number of time SerialControl will retry in case of fail while writing
#define READ_TRY_NB 3 //same but in for reading
#define DEBUG 1
#define BAUDRATE 9600
#define WRITE_FAIL "sc_wf"
#define READ_FAIL "sc_rf"
#define NO_RESPONSE "sc_nr"

//----private variables

class Module {

	public:

	bool isWatched;
	const std::string name;

	Module(bool isWatched, const std::string name, const int fd, const struct termios oldAttr):
		isWatched{isWatched}, name{name}, fileDescriptor{fd}, oldAttr{oldAttr} {}

	std::string sendCommand(const std::string& cmd) const;
	int watch(void callback(const std::string& cmd)) const; 

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
 * checked alll watched modules and return a list of all the reponses found
 * with the name of the module that send them
 */
std::vector<std::tuple<std::string,std::string>> update();

} //namespace SerialControl

#endif

