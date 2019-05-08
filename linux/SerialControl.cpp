//----lib header
#include "SerialControl.hpp"

//----standard libs
#include <iostream>

//----c libs
#include <termios.h>
#include <sys/stat.h>	//open parameters
#include <fcntl.h>		//open function
#include <unistd.h>		//write function
#include <string.h> 	//strtok function

namespace SerialControl {

char paths[][14] = {"/dev/ttyUSB00","/dev/ttyACM00"};


//----functions

std::vector<Module*> listModules(){
	
	//TODO find a way to optimise that
	moduleList.clear();
	moduleList.reserve(2*MAX_INDEX);

	std::vector<Module*> modules;

	//for each paths defined in hpp
	for(auto &elem: paths) {

		for(int i=0; i<=MAX_INDEX; i++) {

			//TODO find a way to tidy that and stop warnings
			if(i/10) { 
				elem[11] = char(i/10 + '0');
				elem[12] = char(i%10 + '0');
			} else {
				elem[11] = char(i%10 + '0');
				elem[12] = '\0';
			}

			if(DEBUG) std::cout << elem << '\n';

			//open file in R/W and without linking it to a terminal
			const int fd = open(elem, O_RDWR | O_NOCTTY);
			if(!fd) {
				if(DEBUG) std::cerr << "Could not open " << elem << '\n'; 
				continue;
			}

			//store default config to reapply it when communication end
			struct termios oldAttr;
			if(tcgetattr(fd, &oldAttr)) {
				if(DEBUG) std::cerr << "Could not get config for " << elem << '\n'; 
				continue;
			}

			//configuration, for more informations see
			//https://www.ibm.com/support/knowledgecenter/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rttcsa.htm#rttcsa
			struct termios newAttr = oldAttr;
			cfsetispeed(&newAttr, BAUDRATE);
			cfsetospeed(&newAttr, BAUDRATE);
			newAttr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
			newAttr.c_oflag &= ~OPOST;
			newAttr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
			newAttr.c_cflag &= ~(CSIZE | PARENB | HUPCL);
			newAttr.c_cflag |= CS8;
			newAttr.c_cc[VMIN]  = 50; 	//these two line are related to the kernel timeout
			newAttr.c_cc[VTIME] = 10;

			//apply previously set configuration to file
			if(tcsetattr(fd, TCSANOW, &newAttr)) {
				if(DEBUG) std::cerr << "Could not apply config for " << elem << '\n';
				continue;
			}

			//clear the file
			tcflush(fd,TCIOFLUSH);

			if(write(fd,"whois;",6) != 6) {
				if(DEBUG) std::cerr << "Could not write whois message for " << elem << '\n';
				continue;
			}

			char rawData[MAX_MESSAGE_SIZE];
			if(read(fd,rawData,MAX_MESSAGE_SIZE) < 0) {
				if(DEBUG) std::cerr << "Could not read message from " << elem << '\n';
				continue;
			}
			
			char* data = strtok(rawData,";");

			Module module{data,fd,oldAttr};
			moduleList.emplace_back(std::move(module));	//store the module in a list
			modules.emplace_back(&moduleList.back());	//get the module adress (moving adress issues ?)
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
		if(DEBUG) std::cerr << "Could not write message to " << this->name << '\n';
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
		else if(DEBUG) std::cerr << "Could not read message from " << this->name << '\n';
		return READ_FAIL;
	}

	char* outData = strtok(rawData, ";");
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
				while(*rawData != '\0') {
					char* data = strtok(rawData,";");
					std::string tmpStr{data};
					elem.callback(tmpStr);
					nbResp++;
				}
			}
			else if(n==0) {
				std::string tmpStr{NO_RESPONSE};
				elem.callback(tmpStr);
			}
			else if(DEBUG) std::cerr << "Could not read message from " << elem.name << '\n';
		}

	}
	return nbResp;
}


} //namespace SerialControl

