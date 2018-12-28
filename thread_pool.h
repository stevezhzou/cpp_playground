#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <function>
#include <thread>
#include <future>
#include <utility>
#include <iostream>

#define MAX_THREAD_NUM 256

class ThreadPool {
public:
    ThreadPool(int size = 16) {
        thrNum = size < 1 ? 1: size < MAX_THREAD_NUM ? size: MAX_THREAD_NUM;
        idlNum = thrNum;
        for (int i = 0; i < thrNum; ++i) {
            _threadPool.emplace_back(
                [this]{
                    while (!this->_stoped) {
                        Task task; // std::function<void()>

                        // get task
                        {
                            std::unique_lock l(this->_mtx);
                            this->_cv.wait(l, [this]{return this->_stoped || !this->_taskQueue.empty();});
                            
                            if (this->_stoped) {
                                break;
                            }

                            if (!this->_taskQueue.empty()) {
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }
                        }

                        // run task
                        --idlNum;
                        task();
                        ++idlNum;
                    }
                }
            );
        }
    }

    ~ThreadPool() {
        _stoped = true;
        _cv.notify_all();
        for (std::thread t: _threadPool) {
            // t.detach();
            t.join(); // wait until tasks are done
        }
    }

    template<class F, class ... Args>
    auto schedule(F&& f, Args&& ... args) -> std::future<decltype(f(args...))> {
        using RetType = decltype(f(args...));

        // use packaged_task to get result
        auto task = std::make_shared<std::packaged_task<RetType()> >(std::bind(std::forward<F>(f), std::forward<Args>(args...)));
        std::future<RetType> result = task->get_future();
        {
            std::lock_guard(_mtx);
            _taskQueue.emplace_back([task](){(*task)();}); // void()
        }

        _cv.notify_one();

        return result;
    }

    inline int threadNum() {return thrNum;}
    inline int idleNum() {return idlNum;}

private:
    using Task = std::function<void()>;
    std::queue<Task> _taskQueue;
    std::vector<std::thread> _threadPool;

    std::mutex _mtx;
    std::condition_variable _cv;

    int thrNum; // thread num
    std::atomic<int> idlNum; // idle thread num
    bool _stoped = false;
}; // threadpool

#endif
