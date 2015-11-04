#pragma once

#include <functional>
#include <thread>
#include <chrono>

namespace timespiece
{
	class watchdog
	{
	private:
		int duration;
		bool repeat;
		bool async;
		bool invalidated;
		std::function<void()> func;
		std::function<void()> completion_handler;
	public:
		watchdog(int duration, bool repeat, bool async, std::function<void()> func, std::function<void()> completion_handler) {
			this->duration = duration;
			this->repeat = repeat;
			this->async = async;
			this->invalidated = false;
			this->func = func;
			this->completion_handler = completion_handler;
		}

		~watchdog() {}

		void resume() {
			if (this->async) {
				std::thread([&]() {
					do {
						std::this_thread::sleep_for(std::chrono::milliseconds(duration));
						if (!this->invalidated) {
							this->func();
						}
					} while (!this->invalidated && this->repeat);
					this->completion_handler();
				}).detach();
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(duration));
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
