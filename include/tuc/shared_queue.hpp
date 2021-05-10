// A generic queue implementation for pushing data from one thread to another
//
// Copyright 2017 Juha Reunanen
//
// Adapted from: https://github.com/reunanen/shared_buffer

#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T> class shared_queue {
public:
    shared_queue() {}

    void push_back(T const& value) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            values.push_back(value);
            ready = true;
        }
        condition_variable.notify_one();
    }

    bool pop_front(T& value) {
        // No waiting.
        std::lock_guard<std::mutex> lock(mutex);
        return pop_front_when_already_locked(value);
    }

    template <class Duration>
    bool pop_front(T& value, Duration const& max_duration) {

        // See if we already have something we can readily pop.
        if (pop_front(value)) {
            return true;
        }

        // We don't have anything right now, so let's just wait.
        std::unique_lock<std::mutex> lock(mutex);
        if (!condition_variable.wait_for(lock, max_duration, [this]{ return this->ready; })) {
            return false;
        }

        // After a successful wait, we own the lock.
        return pop_front_when_already_locked(value);
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(mutex);
		return values.size();
	}

    bool empty() const {
        return size() == 0;
    }

    // Force threads waiting in pop_front() to return.
    void halt() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            ready = true;
        }
        condition_variable.notify_all();
    }

private:
    shared_queue(shared_queue const&) = delete; // not construction-copyable
    shared_queue& operator=(shared_queue const&) = delete; // not copyable

    bool pop_front_when_already_locked(T& value) {
        if (!values.empty()) {
            value = this->values.front();
            this->values.pop_front();
            ready = false;
            return true;
        }
        else {
            return false;
        }
    }

	std::deque<T> values;

	mutable std::mutex mutex;
    std::condition_variable condition_variable;
	bool ready = false;
};
