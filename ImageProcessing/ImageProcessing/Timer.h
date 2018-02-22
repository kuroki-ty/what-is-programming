#ifndef Timer_h
#define Timer_h

#include <iostream>
#include <chrono>

class Timer {
public:
    void start()
    {
        _start = std::chrono::system_clock::now();
    }

    void showProcessingTime()
    {
        auto end = std::chrono::system_clock::now();
        auto dur = end - _start;
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        printf("処理時間: %lld[ms]\n", msec);
    }

private:
    std::chrono::system_clock::time_point _start;
};

#endif /* Timer_h */
