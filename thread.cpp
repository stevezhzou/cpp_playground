#include <thread>
#include <functional>
#include <string>
#include <iostream>

void work(std::string str, int& x) {
    ++x;
    std::cout << "work: " << str << x << std::endl;
}

int main() {
    std::string s = "Test";
    int x = 100;
    std::thread t(work, s, std::ref(x));

    // do something
    for (int i = 0; i < 10; ++i)
        std::cout << "main thread: " << i << std::endl;

    t.join();
}
