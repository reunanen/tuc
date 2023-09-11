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

		bool should_act(const std::chrono::steady_clock::time_point& now = std::chrono::steady_clock::now()) const {
			return should_act(get_default_duration(), now);
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

		bool act_if_appropriate(const std::chrono::steady_clock::time_point& now = std::chrono::steady_clock::now()) {
			return act_if_appropriate(get_default_duration(), now);
		}

		void reset() {
			latest_action = std::chrono::steady_clock::time_point();
		}

	private:
		std::chrono::steady_clock::time_point latest_action;

		static std::chrono::milliseconds get_default_duration() {
			 return std::chrono::milliseconds(1000);
		}
	};
}
