#ifndef SERIAL_CONTROL_HPP
#define SERIAL_CONTROL_HPP

//standard libs
#include <string>
#include <vector>

namespace SerialControl {

/**
 * Send a command to the specified 
 */
int SendCommand(std::string cmd); 

/**
 * Update the list of modules connected
 * return value : a list of the modules names (given by the modules)
 */
std::vector<std::string> updateModules();

} //namespace SerialControl

#endif
