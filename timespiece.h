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
		bool watching;
		bool repeat;
		bool async;
		bool fire;
		std::function<void()> func;
		std::function<void()> completion_handler;
	public:
		watchdog(int wait, bool repeat, bool async, std::function<void()> func, std::function<void()> completion_handler) {
			this->wait = wait;
			this->func = func;
			this->repeat = repeat;
			this->async = async;
			this->completion_handler = completion_handler;
			this->watching = false;
		}

		~watchdog() {

		}

		bool watch() {
			if (!this->watching && this->fire) {
				this->completion_handler();
				if (this->repeat) {
					watching = true;
					this->resume();
				}
			}
			
			return this->watching;
		}

		void resume() {
			this->watching = true;
			if (this->async) {
				std::thread([=]() {
                	std::this_thread::sleep_for(std::chrono::milliseconds(wait));
                	func();
                	mutex.lock();
                	this->fire = true;
					this->watching = false;
                	mutex.unlock();
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
		}
	};
}