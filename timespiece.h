#pragma once

#include <functional>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

namespace timespiece
{
	class watchdog
	{
	private:
		std::mutex mutex;
		int wait;
		bool repeat;
		bool async;
		bool invalidated;
		std::function<void()> func;
		std::function<void()> completion_handler;
	public:
		watchdog(int wait, bool repeat, bool async, std::function<void()> func, std::function<void()> completion_handler) {
			this->wait = wait;
			this->func = func;
			this->repeat = repeat;
			this->async = async;
			this->completion_handler = completion_handler;
			this->invalidated = false;
			this->fired = false;
		}

		~watchdog() {}

		void resume() {
			if (this->async) {
				std::thread([=]() {
					do {
						std::this_thread::sleep_for(std::chrono::milliseconds(wait));
						mutex.lock();
						func();
						mutex.unlock();
					} while (!this->invalidated && this->repeat);
					this->completion_handler();
				}).detach();
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(wait));
				this->func();
				this->completion_handler();
			}
		}

		void invalidate() {
			this->repeat = false;
			this->invalidated = true;
		}
	};
}
