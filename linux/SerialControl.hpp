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

std::vector<std::string> initModules();

} //namespace SerialControl

#endif
