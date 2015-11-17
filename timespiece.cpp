#include "timespiece.h"

static std::mutex mutex;
static std::condition_variable cond;

namespace timespiece
{
	timer::timer(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void(std::string hash)> completion_handler) {
		this->duration = duration;
		this->repeated_count = 0;
		this->repeat = repeat;
		this->async = async;
		this->invalidated = true;
		this->func = func;
		this->completion_handler = completion_handler;
	};
	timer::~timer() {};

	std::string timer::hash() {
		const void * address = static_cast<const void*>(this);
		std::stringstream ss;
		ss << address;
		std::string hash = ss.str();
		return hash;
	}

	void timer::resume() {
		if (this->invalidated) {
			this->invalidated = false;
			if (this->async) {
				std::thread([&]() {
					do {
						std::this_thread::sleep_for(std::chrono::milliseconds(duration));
						std::unique_lock<std::mutex> lock(mutex);
						this->repeated_count++;
						if (!this->invalidated) {
							this->func(this->repeated_count, this);
						}
						lock.unlock();
						cond.notify_one();
					} while (!this->invalidated && this->repeat);
					this->completion_handler(this->hash());
				}).detach();
			}
			else {
				do {
					std::this_thread::sleep_for(std::chrono::milliseconds(duration));
					this->repeated_count++;
					if (!this->invalidated) {
						this->func(this->repeated_count, this);
					}
				} while (!this->invalidated && this->repeat);
				this->completion_handler(this->hash());
			}
		}
	}

	void timer::invalidate() {
		std::unique_lock<std::mutex> lock(mutex);
		this->repeat = false;
		this->invalidated = true;
		lock.unlock();
		cond.notify_one();
	}

	watchdog::watchdog() {}
	watchdog::~watchdog() {}

	std::shared_ptr<timespiece::timer> watchdog::resume(int duration, bool repeat, bool async, std::function<void(int repeated_count, timespiece::timer *t)> func, std::function<void()> completion_handler) {
		std::shared_ptr<timespiece::timer> t = std::make_shared<timespiece::timer>(timespiece::timer(duration, repeat, async, func, [&, completion_handler] (std::string hash) {
			completion_handler();
			this->timer.erase(hash);
		}));
		this->last_timer_hash = t->hash();
		this->timer[t->hash()] = t;
		t->resume();

		return t;
	}

	void watchdog::invalidate() {
		this->invalidate(this->last_timer_hash);
	}

	void watchdog::invalidate(std::string hash) {
		if (this->timer.find(hash) != this->timer.end()) {
			std::shared_ptr<timespiece::timer> t = this->timer[hash];
			t->invalidate();
		}
	}
}