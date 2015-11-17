#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <thread>
#include <chrono>
#include <iostream>

namespace timespiece
{
	class timer
	{
	private:
		int duration;
		bool repeat;
		bool async;
		bool invalidated;
		std::function<void(int repeated_count, timespiece::timer *t)> func;
		std::function<void(std::string hash)> completion_handler;
	public:
		int repeated_count;

		timer(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void(std::string hash)> completion_handler);
		~timer();

		std::string hash();
		void resume();
		void invalidate();
	};

	class watchdog
	{
	private:
		std::string last_timer_hash;
		std::map<std::string, std::shared_ptr<timespiece::timer> > timer;
	public:
		watchdog();
		~watchdog();

		std::shared_ptr<timespiece::timer> resume(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void()> completion_handler);
		void invalidate();
		void invalidate(std::string hash);
	};
}
