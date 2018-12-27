#include <future>
#include <thread>
#include <iostream>

void test(std::promise<int> p) {
    p.set_value(42);
}

int main() {
    std::promise<int> result_promise;
    std::future<int> result_future = result_promise.get_future();

    std::thread t(test, std::move(result_promise));
    int result = result_future.get(); // block until there's a value

    t.join();

    std::cout << result << std::endl;
}
