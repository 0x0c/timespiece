#include <iostream>
#include "timespiece.h"

int main(int argc, char const *argv[])
{
	timespiece::watchdog t(3 * 1000, true, true, [] {
		std::cout << "dispatch" << std::endl;
	}, [] {
		std::cout << "complete" << std::endl;
	});
	t.resume();
	while (t.watch()) {}

	std::cout << "finish" << std::endl;

	return 0;
}