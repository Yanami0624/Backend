#include "thread_pool.h"

// 测试函数 1：有返回值
int add(int a, int b) {
    this_thread::sleep_for(chrono::seconds(1));
    return a + b;
}

// 测试函数 2：字符串返回值
string hello(string name) {
    return "Hello, " + name;
}

int main() {
    ThreadPool pool(4);  // 4 个线程

    // 提交任务，拿到 future
    auto f1 = pool.push(add, 10, 20);
    auto f2 = pool.push(hello, "ThreadPool");

    // 阻塞等待结果
    cout << "add result: " << f1.get() << endl;
    cout << "hello result: " << f2.get() << endl;

    return 0;
}