//----lib header
#include "SerialControl.hpp"

//----standard libs
#include <iostream>

//----c libs
#include <termios.h>
#include <sys/stat.h>	//open parameters
#include <fcntl.h>		//open function
#include <unistd.h>		//write function

namespace SerialControl {

char paths[][14] = {"/dev/ttyUSB00","/dev/ttyACM00"};


//----functions

std::vector<Module> listModules(){

	std::vector<Module> modules;

	//for each paths defined in hpp
	for(auto &elem: paths) {

		for(int i=0; i<=MAX_INDEX; i++) {

			//TODO find a way to tidy that and stop warnings
			if(i/10) { 
				elem[11] = i/10 + '0';
				elem[12] = i%10 + '0';
			} else {
				elem[11] = i%10 + '0';
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

			char data[MAX_MESSAGE_SIZE];
			if(read(fd,data,MAX_MESSAGE_SIZE) < 0) {
				if(DEBUG) std::cerr << "Could not read message from " << elem << '\n';
				continue;
			}

			Module module{false,data,fd,oldAttr};
			modules.emplace_back(module);
		}
	}

	return modules;
}

/*

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
*/

} //namespace SerialControl

