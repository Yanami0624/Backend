#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <shared_mutex>
#include <memory>
#include <functional>
#include <atomic>
#include <condition_variable>
using namespace std;

class ThreadPool {
    public:
    ThreadPool(int c): capability(c) {
        for(int i = 0; i < capability; ++i) {
            thrds.emplace_back([this]() {
                while(true) {
                    unique_lock<mutex> lock(mtx);
                    cv.wait(lock, [=]() {
                        return !tasks.empty() || stop;
                    });
                    if(stop && tasks.empty()) return;
                    auto task = move(tasks.front());
                    tasks.pop();
                    lock.unlock();
                    task();
                }
            });

        }
    }
    ~ThreadPool() {
        unique_lock<mutex> lock(mtx);
        stop = true;
        cv.notify_all();
        for(auto &t: thrds) {
            t.join();
        }
    }

    template<class F, class ... Args>
    void push(F &&f, Args&&...args) {
        auto task = bind(
            forward<F>(f),
            forward<Args>(args)...
        );
        {
            unique_lock<mutex> lock(mtx);
            tasks.emplace(move(task));
        }
        cv.notify_one();
    }
    
    private:
    int capability;
    mutex mtx;
    vector<thread> thrds;
    queue<function<void()>> tasks;
    atomic<bool> stop;
    condition_variable cv;
    using lkguard = lock_guard<mutex>;
};