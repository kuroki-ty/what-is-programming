#include <iostream>
#include <vector>
#include <numeric>
#include <pthread.h>
#include <semaphore.h>

#include <errno.h>

namespace {
    const uint32_t N = 256;    // リングバッファ限界値
}  // namespace

template <typename T>
class MultiThreadQueue
{
public:
    void enqueue(const T &data)
    {
        int next = (_tail + 1) % N;
        if (next != _head) {    // リングバッファがいっぱいではない
            _data[_tail] = data;
            _tail = next;
        }

        sem_post(_sem);    // キューに入れたのでセマフォ加算
        perror("sem_post error  ");
    }

    T dequeue()
    {
        sem_wait(_sem);    // キューに積まれるまでブロック
        perror("sem_wait error  ");

        T ret = T();
        if (_head != _tail) {    // リングバッファが空ではない
            ret = _data[_head];
            _head = (_head + 1) % N;
        }

        return ret;
    }

    void setSem(sem_t* sem){ _sem = sem; }
    sem_t* getSem() const { return _sem; }

private:
    T _data[N];       // バッファ
    int _head = 0;    // 読み出しポインタ
    int _tail = 0;    // 書き出しポインタ
    sem_t* _sem;      // セマフォ
};

struct WorkData
{
    enum {
        MESSAGE_TERMINATE,
        MESSAGE_CALCSUM,
    } message;
    std::vector<int> vec;
};

MultiThreadQueue<WorkData> queue;

void* worker(void *)
{
    /* 渡されたWorkDataに基づいて処理する */
    for (;;) {
        WorkData data = queue.dequeue();

        if (data.message == WorkData::MESSAGE_TERMINATE) {
            /* スレッドを終了する */
            sem_destroy(queue.getSem());          // セマフォを解放
            perror("sem_destroy error  ");
            pthread_exit(NULL);                   // スレッドを終了
            break;
        }

        if (data.message == WorkData::MESSAGE_CALCSUM) {
            int sum = std::accumulate(data.vec.begin(), data.vec.end(), 0);

            printf("sum of vector is %d\n", sum);
        }
    }

    return nullptr;
}

int main()
{
    /* セマフォの初期化 */
    sem_t* sem = (sem_t*)malloc(sizeof(sem_t) * 10);
    sem_init(sem, 0, 0);
    queue.setSem(sem);
    perror("sem_init error  ");

    /* スレッドを起動する */
    pthread_t th1;
    pthread_create(&th1, NULL, worker, NULL);

    WorkData work1 = { WorkData::MESSAGE_CALCSUM, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } };        // 55
    queue.enqueue(work1);
    WorkData work2 = { WorkData::MESSAGE_CALCSUM, { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 } };    // 100
    queue.enqueue(work2);
    WorkData work3 = { WorkData::MESSAGE_CALCSUM, { 1, -1, 3, -3, 5, -5, 7, -7, 9, -9 } };    // 0
    queue.enqueue(work3);
    WorkData work4 = { WorkData::MESSAGE_CALCSUM, { 1, 4, 9, 16, 25, 36, 49, 64, 81, 100 } }; // 385
    queue.enqueue(work4);
    WorkData work5 = { WorkData::MESSAGE_TERMINATE, { } };
    queue.enqueue(work5);

    /* スレッドの終了を待つ */
    pthread_join(th1, NULL);

    free(queue.getSem());

    return 0;
}
