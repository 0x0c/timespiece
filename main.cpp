#include <iostream>
#include "timespiece.h"

int main(int argc, char const *argv[])
{
	bool finished = false;
	timespiece::watchdog t(3 * 1000, false, true, [] {
		std::cout << "dispatch" << std::endl;
	}, [&finished] {
		finished = true;
		std::cout << "complete" << std::endl;
	});
	t.resume();
	while (!finished) {}

	std::cout << "finish" << std::endl;

	return 0;
}