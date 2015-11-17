#include <iostream>
#include "timespiece.h"

int main(int argc, char const *argv[])
{
	bool finished = false;
	timespiece::watchdog w;

	std::cout << "start" << std::endl;	

	std::shared_ptr<timespiece::timer> timer = w.resume(1000, true, true, [] (int repeated_count, timespiece::timer *t) {
		std::cout << "dispatch : " << t->repeated_count << std::endl;
	}, [&finished] {
		if (finished == false){
			finished = true;
		}
		std::cout << "complete" << std::endl;
	});
	while (!finished) {
		std::string s;
		std::cin >> s;
		if (!s.empty()) {
			finished = true;
		}
	}

	std::cout << "finish" << std::endl;

	return 0;
}
