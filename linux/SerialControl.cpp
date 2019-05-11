//----lib header
#include "SerialControl.hpp"

//----standard libs
#include <iostream>
#include <chrono>
#include <thread>

//----c libs
#include <termios.h>
#include <sys/stat.h>	//open parameters
#include <fcntl.h>		//open function
#include <unistd.h>		//write function
#include <string.h> 	//strtok function


//----macros
//
#if DEBUG
#define DEBUG_MSG(str) do { std::cout << "DEBUG:" << str << '\n'; } while(false)
#else
#define DEBUG_MSG(str) do {} while(false)
#endif

#if ERROR
#define ERROR_MSG(str) do { std::cerr << "ERROR:" << str << '\n'; } while(false)
#else
#define ERROR_MSG(str) do {} while(false)
#endif


namespace SerialControl {

using namespace std::chrono_literals;

char paths[][14] = {"/dev/ttyUSB00","/dev/ttyACM00"};


//----functions

std::vector<Module*> listModules(){
	
	moduleList.clear();
	std::vector<Module*> modules;

	//for each paths defined in hpp
	for(auto &elem: paths)	{

		for(int i=0; i<=MAX_INDEX; i++) {

			if(i/10) { 
				elem[11] = char(i/10 + '0');
				elem[12] = char(i%10 + '0');
			} else {
				elem[11] = char(i%10 + '0');
				elem[12] = '\0';
			}

			DEBUG_MSG(elem); 

			//open file in R/W and without linking it to a terminal
			const int fd = open(elem, O_RDWR | O_NOCTTY);
			if(!fd) {
				DEBUG_MSG("Could not open " << elem); 
				continue;
			}

			//store default config to reapply it when communication end
			struct termios oldAttr;
			if(tcgetattr(fd, &oldAttr)) {
				DEBUG_MSG("Could not get config for " << elem); 
				continue;
			}

			//configuration, for more informations see
			//https://www.ibm.com/support/knowledgecenter/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rttcsa.htm#rttcsa
			struct termios newAttr = oldAttr;
			cfsetispeed(&newAttr, BAUDRATE);
			cfsetospeed(&newAttr, BAUDRATE);
			cfmakeraw(&newAttr);
			newAttr.c_cflag |= CS8;		//8 bits chars
			newAttr.c_cc[VMIN]  = 10; 	//number of chars read() wait for (0 means read() don't block)
			newAttr.c_cc[VTIME] = SERIAL_TIMEOUT;	//read timeout time (in 0.1 of secs)

			//apply previously set configuration to file
			if(tcsetattr(fd, TCSANOW, &newAttr)) {
				DEBUG_MSG("Could not apply config for " << elem);
				continue;
			}
			
			//clear the file
			tcflush(fd,TCIOFLUSH);

			//nanos are painfully slow, this help them realize that there is a serial...
			std::this_thread::sleep_for(2s);

			if(write(fd,"whois;",6) != 6) {
				DEBUG_MSG("Could not write whois message for " << elem);
				continue;
			}

			char rawData[MAX_MESSAGE_SIZE];
			if(read(fd,rawData,MAX_MESSAGE_SIZE) < 0) {
				DEBUG_MSG("Could not read message from " << elem);
				continue;
			}
			DEBUG_MSG(rawData);
			
			char* savePtr;
			char* data = strtok_r(rawData,";",&savePtr); //strtok_r is a thread-safe strtok
			if(data == NULL) {
				ERROR_MSG("Could not parse name of " << elem);
				continue;
			}

			DEBUG_MSG("whois : " << data);
			Module module{data,fd,oldAttr};
			moduleList.emplace_back(std::move(module));	//store the module in a list
			modules.emplace_back(&moduleList.back());	//get the module adress (moving adress issues ?)

			newAttr.c_cc[VMIN]  = 0;	//the nano has used serial a first time, it will be faster now 
			if(tcsetattr(fd, TCSANOW, &newAttr)) {
				DEBUG_MSG("Could not apply config for " << elem);
				continue;
			}
			tcflush(fd,TCIOFLUSH);

		}
		
	}

	return modules;
}


std::string 
Module::sendCommand(const std::string& cmd) const{

	//TODO add support for commands longer than MAX_MESSAGE_SIZE
	const ssize_t size = cmd.size();
	const char* inData = cmd.c_str();

	//write to device, try WRITE_TRY_NB before giving up
	int i;
	for(i=0; write(this->fileDescriptor,inData,size) != size && i < WRITE_TRY_NB; i++) {}
	if(i == WRITE_TRY_NB) {
		ERROR_MSG("Could not write message to " << this->name);
		return WRITE_FAIL;
	}

	//read from device, try READ_TRY_NB beofre giving up
	char rawData[MAX_MESSAGE_SIZE];
	ssize_t n = 0;
	for(i=0; i < READ_TRY_NB; i++) {
		n = read(this->fileDescriptor,&rawData,MAX_MESSAGE_SIZE);
		if(n > 0) break;
	}
	if(i == READ_TRY_NB) {
		if(n == 0) return NO_RESPONSE;
		else ERROR_MSG("Could not read message from " << this->name);
		return READ_FAIL;
	}

	char* savePtr;
	char* outData = strtok_r(rawData, ";",&savePtr);
	return std::string{outData};
}


int 
Module::watch(void callback(const std::string& cmd)) {
	this->callback = callback;
	return 0;
}


int update() {

	int nbResp = 0;

	for(const auto &elem: moduleList) {
		if(elem.callback) {
			char rawData[MAX_MESSAGE_SIZE];
			const ssize_t n = read(elem.fileDescriptor,&rawData,MAX_MESSAGE_SIZE);
			if(n>0) {
				char* savePtr;	//used to save strtok_r() contex for futur calls
				char* data = strtok_r(rawData,";",&savePtr); //separate response at the first ";"
				std::string tmpStr{data};
				elem.callback(tmpStr);	//passing response to user's function
				nbResp++;
				data = strtok_r(NULL,";",&savePtr); //separate response another time
				while(data != NULL) {	//NULL signifies that there no more ";"
					std::string tmpStr{data};
					elem.callback(tmpStr);
					nbResp++;
					data = strtok_r(NULL,";",&savePtr);
				}
			}
			else if(n==0) {
				DEBUG_MSG("no message from " << elem.name);
			}
			else ERROR_MSG("Could not read message from " << elem.name);
		}

	}
	return nbResp;
}


} //namespace SerialControl

