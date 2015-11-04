#include <iostream>
#include "timespiece.h"

int main(int argc, char const *argv[])
{
	bool finished = false;
	timespiece::watchdog t;

	std::cout << "start" << std::endl;	

	std::shared_ptr<timespiece::timer> timer = t.resume(3 * 1000, false, true, [] {
		std::cout << "dispatch" << std::endl;
	}, [&finished] {
		if (finished == false){
			finished = true;
		}
		std::cout << "complete" << std::endl;
	});
	while (!finished) {}

	std::cout << "finish" << std::endl;

	return 0;
}
