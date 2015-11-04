#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <thread>
#include <chrono>

namespace timespiece
{
	class timer
	{
	private:
		int duration;
		bool repeat;
		bool async;
		bool invalidated;
		std::function<void()> func;
		std::function<void()> completion_handler;
	public:
		timer(int duration, bool repeat, bool async, std::function<void()> func, std::function<void()> completion_handler) {
			this->duration = duration;
			this->repeat = repeat;
			this->async = async;
			this->invalidated = true;
			this->func = func;
			this->completion_handler = completion_handler;
		};
		~timer() {};
		
		void resume() {
			if (this->invalidated) {
				this->invalidated = false;
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
		}

		void invalidate() {
			this->repeat = false;
			this->invalidated = true;
		}
	};
	class watchdog
	{
	private:
		std::vector<std::shared_ptr<timespiece::timer> > timer;
	public:
		watchdog() {}
		~watchdog() {}

		void resume(int duration, bool repeat, bool async, std::function<void()> func, std::function<void()> completion_handler) {
			std::shared_ptr<timespiece::timer> t = std::make_shared<timespiece::timer>(timespiece::timer(duration, repeat, async, func, completion_handler));
			t->resume();
			this->timer.push_back(t);
		}

		void invalidate() {
			this->invalidate(0);
		}

		void invalidate(int index) {
			if (!this->timer.empty()) {
				std::shared_ptr<timespiece::timer> t = this->timer.at(index);
				t->invalidate();
				this->timer.erase(this->timer.begin() + index);	
			}
		}
	};
}
