#ifndef MultiThreadQueue_h
#define MultiThreadQueue_h

#include <iostream>
#include <condition_variable>

namespace {
    const uint32_t N = 256;    // リングバッファ限界値
}  // namespace

template <typename T>
class MultiThreadQueue
{
public:
    void enqueue(const T &data)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        int next = (_tail + 1) % N;

        // リングバッファがいっぱいの時はブロックして待つ
        _notFull.wait(lock, [this, next] { return next != _head; });

        // キューに入れる
        _data[_tail] = data;
        _tail = next;

        _notEmpty.notify_all();   // dequeue OK
    }

    T dequeue()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        T ret = T();

        // リングバッファが空の時はブロックして待つ
        _notEmpty.wait(lock, [this] { return _head != _tail; });

        // キューから取り出す
        ret = _data[_head];
        _head = (_head + 1) % N;

        _notFull.notify_all();    // enqueue OK

        return ret;
    }

private:
    T _data[N];       // バッファ
    int _head = 0;    // 読み出しポインタ
    int _tail = 0;    // 書き出しポインタ

    std::mutex _mutex;
    std::condition_variable _notFull;     // enqueue用状態変数
    std::condition_variable _notEmpty;    // dequeue用状態変数
};


#endif /* MultiThreadQueue_h */
