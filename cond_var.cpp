#include <thread>
#include <mutex>
#include <iostream>

// global shared data
std::mutex m;                   // #include <mutex>
std::condition_variable cv;     // #include <condition_variable>
bool flag = false;

void test(int* value) {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "test func get lock!\n";

    // do some evaluations
    *value = 10;
    flag = true;
    cv.notify_one();

    return;
}

int main() {
    flag = false;

    int* value = new int(1);
    std::thread t(test, value);

    std::unique_lock<std::mutex> lk(m);

    cv.wait(lk, [](){ return flag; });
    std::cout << "main func get lock!\n";
    *value = 100;
    lk.unlock();

    t.join();

    std::cout << "final val: " << *value << std::endl;

    if (nullptr != value)
        delete value;
}
