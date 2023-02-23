#pragma once

#include "shared_queue.hpp"
#include "thread.hpp" // set_current_thread_to_idle_priority()
#include "numeric.hpp"
#include <atomic>
#include <array>
#include <future>
#include <memory>
#include <sstream>
#include <functional>
#include <iostream>

namespace tuc
{
    enum struct thread_priority
    {
        idle_priority = -1,
        normal_priority = 0
    };

    class thread_pool
    {
    public:
        thread_pool(
            size_t thread_count = std::thread::hardware_concurrency(),
            thread_priority priority = thread_priority::idle_priority
        )
            : priority(priority)
        {
            set_thread_count(thread_count);
        }

        ~thread_pool()
        {
            for (auto& k : killed) {
                *k = true;
            }
            incoming_tasks.halt();
            for (auto& thread : threads) {
                thread.join();
            }
        }

        // note: this will block, if threads to be dropped are still executing
        void set_thread_count(size_t thread_count)
        {
            std::cout << "set_thread_count(" << thread_count << ") starting, current count = " << threads.size() << std::endl;

            for (size_t i = thread_count, end = threads.size(); i < end; ++i) {
                std::cout << "setting the kill flag" << std::endl;
                *killed[i] = true;
            }

            while (threads.size() > thread_count) {
                std::cout << "joining thread" << std::endl;
                threads.back().join();
                std::cout << "popping thread" << std::endl;
                threads.pop_back();
                std::cout << "popping the kill flag" << std::endl;
                killed.pop_back();
                std::cout << "done killing thread" << std::endl;
            }

            while (threads.size() < thread_count) {
                auto const index = threads.size();
                std::cout << "creating kill flag " << index << std::endl;
                killed.push_back(
                    std::make_unique<std::atomic<bool>>(false)
                );
                auto* die = killed[index].get();
                auto const function = [this, die]() {
                    thread_function(die);
                };
                std::cout << "creating thread " << index << std::endl;
                threads.emplace_back(function);
                std::cout << "done creating thread " << index << std::endl;
            }

            std::cout << "set_thread_count(" << thread_count << ") done!" << std::endl;
        }

        template<typename Function, typename... Arguments>
        auto operator()(Function function, Arguments... arguments)
        {
            using packaged_task_type = std::packaged_task<decltype(function(arguments...))()>;
            auto task = std::make_shared<packaged_task_type>(std::bind(function, arguments...));
            auto future = task->get_future();
            incoming_tasks.push_back({ { [task]() { (*task)(); } } });
            return future;
        }

        template<typename Function, typename... Arguments>
        auto operator()(std::launch const& launch_mode, Function function, Arguments... arguments)
        {
            return launch(launch_mode, *this, function, arguments...);
        }

        template<typename Function, typename... Arguments>
        auto static launch(std::launch const& launch_mode, thread_pool& tp, Function function, Arguments... arguments)
        {
            switch (launch_mode) {
            case std::launch::async: return tp(function, arguments...);
            case std::launch::deferred: return std::async(std::launch::deferred, function, arguments...);
            default: throw std::runtime_error("Function run() not implemented for launch mode: " + std::to_string(static_cast<int>(launch_mode)));
            }
        }

        // expected task uniformity:
        // * try negative infinity if you expect tasks to be highly non-uniform (-> small chunk size)
        // * try positive infinity if you expect tasks to be highly uniform     (-> large chunk size)
        // * try in-between values if you expect tasks to be moderately uniform (-> in-between chunk size)
        size_t get_default_desired_chunk_size(size_t task_count, double expected_task_uniformity = 0.0) const
        {
            auto const min_reasonable_chunk_size = 1;
            auto const max_reasonable_chunk_size = tuc::divide_rounding_up(task_count, get_thread_count());
            std::array<double, 2> const extremes = {
                static_cast<double>(min_reasonable_chunk_size),
                static_cast<double>(max_reasonable_chunk_size)
            };
            return tuc::round<size_t>(tuc::power_mean(extremes.begin(), extremes.end(), expected_task_uniformity));
        }

        template<typename Function, typename... Arguments>
        auto launch_in_chunks(Function function, std::vector<Arguments...> const& arguments, size_t desired_chunk_size = 0)
        {
            std::vector<std::future<decltype(function(arguments.front()))>> futures(arguments.size());

            size_t const chunk_size = desired_chunk_size == 0
                ? get_default_desired_chunk_size(arguments.size())
                : desired_chunk_size;

            std::vector<incoming_task> current_chunk;
            current_chunk.reserve(chunk_size);

            auto const dispatch_current_chunk = [&]() {
                incoming_tasks.push_back(std::move(current_chunk));
                current_chunk.clear();
            };

            for (size_t i = 0, end = arguments.size(); i < end; ++i) {
                auto const& argument = arguments[i];
                using packaged_task_type = std::packaged_task<decltype(function(argument))()>;
                auto task = std::make_shared<packaged_task_type>(std::bind(function, argument));
                futures[i] = task->get_future();
                current_chunk.push_back({ { [task]() { (*task)(); } } });
                if (current_chunk.size() >= chunk_size) {
                    dispatch_current_chunk();
                }
            }

            if (!current_chunk.empty()) {
                dispatch_current_chunk();
            }

            return futures;
        }
        
        // A convenience wrapper for the above function
        template<typename Function>
        auto launch_in_chunks(Function function, size_t count, size_t desired_chunk_size = 0)
        {
            std::vector<size_t> arguments(count);
            std::iota(arguments.begin(), arguments.end(), 0);
            return launch_in_chunks(function, arguments, desired_chunk_size);
        }

        size_t get_thread_index(std::thread::id const& thread_id) const
        {
            for (size_t i = 0, end = threads.size(); i < end; ++i ) {
                if (threads[i].get_id() == thread_id) {
                    return i;
                }
            }

            std::ostringstream error;
            error << "Unexpected thread id: " << thread_id;
            throw std::runtime_error(error.str());
        }

        size_t get_this_thread_index() const
        {
            return get_thread_index(std::this_thread::get_id());
        }

        size_t get_thread_count() const
        {
            return threads.size();
        }

    private:
        thread_pool(thread_pool const&) = delete; // not construction-copyable
        thread_pool& operator=(thread_pool const&) = delete; // not copyable

        struct incoming_task
        {
            std::function<void()> task;
        };

        void thread_function(std::atomic<bool>* die)
        {
            if (priority == thread_priority::idle_priority) {
                set_current_thread_to_idle_priority();
            }
            std::chrono::seconds constexpr one_second{ 1 };
            while (!*die) {
                std::vector<incoming_task> tasks;
                if (incoming_tasks.pop_front(tasks, one_second)) {
                    for (auto& task : tasks) {
                        task.task();
                    }
                }
            }
        };

        thread_priority const priority = thread_priority::idle_priority;
        std::vector<std::unique_ptr<std::atomic<bool>>> killed;
        shared_queue<std::vector<incoming_task>> incoming_tasks;
        std::deque<std::thread> threads;
    };

    template<typename Function, typename... Arguments>
    auto launch(std::launch const& launch_mode, thread_pool& tp, Function function, Arguments... arguments)
    {
        return thread_pool::launch(launch_mode, tp, function, arguments...);
    }
}
