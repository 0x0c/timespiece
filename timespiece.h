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
		
		timer(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void(std::string hash)> completion_handler) {
			this->duration = duration;
			this->repeated_count = 0;
			this->repeat = repeat;
			this->async = async;
			this->invalidated = true;
			this->func = func;
			this->completion_handler = completion_handler;
		};
		~timer() {};
		
		std::string hash() {
			const void * address = static_cast<const void*>(this);
			std::stringstream ss;
			ss << address;  
			std::string hash = ss.str(); 
			return hash;
		}

		void resume() {
			if (this->invalidated) {
				this->invalidated = false;
				if (this->async) {
					std::thread([&]() {
						do {
							std::this_thread::sleep_for(std::chrono::milliseconds(duration));
							if (!this->invalidated) {
								this->func(this->repeated_count, this);
							}
							this->repeated_count++;
						} while (!this->invalidated && this->repeat);
						this->completion_handler(this->hash());
					}).detach();
				}
				else {
					do {
						std::this_thread::sleep_for(std::chrono::milliseconds(duration));
						if (!this->invalidated) {
							this->func(this->repeated_count, this);
						}
						this->repeated_count++;
					} while (!this->invalidated && this->repeat);
					this->completion_handler(this->hash());
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
		std::string last_timer_hash;
		std::map<std::string, std::shared_ptr<timespiece::timer> > timer;
	public:
		watchdog() {}
		~watchdog() {}

		std::shared_ptr<timespiece::timer> resume(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void()> completion_handler) {
			std::shared_ptr<timespiece::timer> t = std::make_shared<timespiece::timer>(timespiece::timer(duration, repeat, async, func, [&, completion_handler] (std::string hash) {
				completion_handler();
				this->timer.erase(hash);
			}));
			this->last_timer_hash = t->hash();
			this->timer[t->hash()] = t;
			t->resume();
			
			return t;
		}

		void invalidate() {
			this->invalidate(this->last_timer_hash);
		}

		void invalidate(std::string hash) {
			if (this->timer.find(hash) != this->timer.end()) {
				std::shared_ptr<timespiece::timer> t = this->timer[hash];
				t->invalidate();
			}
		}
	};
}
