#include <iostream>
#include <vector>
#include <numeric>
#include <pthread.h>
#include <condition_variable>
#include <unistd.h>
#include "MultiThreadQueue.h"

#include <errno.h>

void randomSleep()
{
    int time = rand() % 5;
    sleep(time);
}

/**
 * 実際に計算してデータ(RealData)を作成するクラス
 * 計算に時間がかかる。
 */
class RealData
{
public:
    RealData(std::vector<int>& values)
    {
        printf("    [BEGIN] Making Real Data.\n");

        // 計算に時間がかかる想定
        randomSleep();
        _result = std::accumulate(values.begin(), values.end(), 0);

        printf("    [END] Making Real Data. %d\n", _result);
    }

    int getResult() { return _result; }

private:
    int _result = 0;
};

using RealDataPtr   = std::shared_ptr<RealData>;

/**
 * RealDataの引換券となるクラス
 */
class FutureData
{
public:
    /** 結果が来るまで待って結果を返す。既に結果が書き込まれている場合はすぐに戻る */
    int waitResult()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        _notReady.wait(lock, [this] { return _ready; });

        return _realData->getResult();
    }

    /** 結果が入っているかを返す */
    bool tryResult()
    {
        return _ready;
    }

    /** 結果を書き込む。待っているスレッドをすべて起こす */
    void writeResult(const RealDataPtr& realData)
    {
        if (_ready) {
            return;
        }
        _realData = realData;
        _ready = true;

        _notReady.notify_all();
    }

private:
    RealDataPtr _realData;
    bool _ready = false;

    std::mutex _mtx;
    std::condition_variable _notReady;
};

using FutureDataPtr = std::shared_ptr<FutureData>;

struct WorkData
{
    enum {
        MESSAGE_TERMINATE,
        MESSAGE_CALCSUM,
    } message;
    std::vector<int> vec;
};

MultiThreadQueue<WorkData> queue;

void* worker(void* p)
{
    /* 渡されたWorkDataに基づいて処理する */
    for (;;) {
        WorkData data = queue.dequeue();

        if (data.message == WorkData::MESSAGE_TERMINATE) {
            /* スレッドを終了する */
            pthread_exit(NULL);    // スレッドを終了
            break;
        }

        if (data.message == WorkData::MESSAGE_CALCSUM) {
            FutureData* futureData = static_cast<FutureData*>(p);
            RealDataPtr realData = std::make_shared<RealData>(data.vec);
            // データが作られたのでFutureDataに値を入れる
            futureData->writeResult(realData);

            /* スレッドを終了する */
            pthread_exit(NULL);
        }
    }

    return nullptr;
}

class Host
{
public:
    FutureDataPtr request(WorkData workData)
    {
        // Futureクラスのインスタンスを作る
        FutureDataPtr futureData = std::make_shared<FutureData>();

        /* スレッドを起動する */
        pthread_t th;
        pthread_create(&th, NULL, worker, futureData.get());
        pthread_detach(th);

        // キューにデータを格納
        queue.enqueue(workData);

        printf("  work data request.\n");

        return futureData;
    }
};

int main()
{
    // Hostクラス生成
    auto host = std::make_shared<Host>();

    std::vector<WorkData> workDataList = {
        { WorkData::MESSAGE_CALCSUM, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } },           // 55
        { WorkData::MESSAGE_CALCSUM, { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 } },       // 100
        { WorkData::MESSAGE_CALCSUM, { 1, -1, 3, -3, 5, -5, 7, -7, 9, -9 } },       // 0
        { WorkData::MESSAGE_CALCSUM, { 1, 4, 9, 16, 25, 36, 49, 64, 81, 100 } },    // 385
    };
    std::vector<FutureDataPtr> futureDataList;

    printf("Work data requrst start.\n");
    for (auto workData : workDataList) {
        auto data = host->request(workData);
        futureDataList.push_back(data);
    }
    printf("Work data requrst done.\n");

    for (auto futureData : futureDataList) {
        printf("Real Data is creating. ready: %s\n", futureData->tryResult() ? "true" : "false");
        printf("created. data: %d\n", futureData->waitResult());
    }

    return 0;
}
