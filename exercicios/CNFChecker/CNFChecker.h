#ifndef _PUSHER_RECEIVER_H_
#define _PUSHER_RECEIVER_H_

#include <string>
#include <fstream>
#include <vector>

using namespace std;

class CNFChecker
{
public:
	CNFChecker(std::string _inputPath);
	CNFChecker(std::string _CNFStr, std::string _ValStr);
	~CNFChecker();

	std::string run();

private:
	//"Global variables of the APP"
	std::string inputPath;
	std::string CNFStr;
	std::string ValStr;
	std::vector<string> splitString(std::string str, std::string delimiter);
};

#endif