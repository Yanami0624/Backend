#pragma once

#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <memory>
#include <future>

using namespace std;

class ThreadPool {
private:
    using func = function<void()>;
    vector<thread> workers;
    condition_variable cv;
    atomic_bool running{true};
    queue<func> tasks;
    mutex tasks_lock;

public:
    ThreadPool(int nthrds) {
        for(int i = 0; i < nthrds; ++i) {
            thread t([this]() {
            while(true) {
                func f;
                {
                    unique_lock<mutex> lock(tasks_lock);
                    cv.wait(lock, [&](){return !tasks.empty() || !running;});
                    if(!running) return;

                    f = move(tasks.front());
                    tasks.pop();
                }
                f();
                }
            });
            workers.emplace_back(move(t));
        }
    }

    ~ThreadPool() {
        running.store(false);
        cv.notify_all();
        for(auto &w: workers)
        w.join();
    }

    template<class F, class... Arg>
    auto push(F&& f, Arg&&... arg) -> future<decltype(f(arg...))> {
        static int id = 0;
        ++id;
        using return_type = decltype(f(arg...));
        
        auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Arg>(arg)...)
        );
        
        future<return_type> res = task->get_future();
        
        {
            unique_lock<mutex> lock(tasks_lock);
            tasks.emplace([task]() {
                (*task)();
            });
        }
        
        cv.notify_one();
        return res;
    }
};