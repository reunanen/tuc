#pragma once

#include <chrono>

namespace tuc
{ 
	class throttle {
	public:
		template <typename Duration>
		bool should_act(Duration duration, const std::chrono::steady_clock::time_point& now = std::chrono::steady_clock::now()) const {
			return now - latest_action >= duration;
		}

		void record_action(const std::chrono::steady_clock::time_point& now = std::chrono::steady_clock::now()) {
			latest_action = now;
		}

		template <typename Duration>
		bool act_if_appropriate(Duration duration, const std::chrono::steady_clock::time_point& now = std::chrono::steady_clock::now()) {
			if (should_act(duration, now)) {
				record_action(now);
				return true;
			}
			else {
				return false;
			}
		}

	private:
		std::chrono::steady_clock::time_point latest_action;
	};
}
