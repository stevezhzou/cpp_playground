#include <future>
#include <iostream>

int test() {
    int res = 0;

    // do something
    for (int i = 0; i < 10; ++i) {
        std::cout << i << std::endl;
    }

    res = 1;
    return res;
}

int main(){
    std::future<int> result_future= std::async(test);

    // do something
    for (int i = 100; i < 110; ++i) {
        std::cout << i << std::endl;
    }

    std::cout << "main thread:" << result_future.get() << std::endl;
}
