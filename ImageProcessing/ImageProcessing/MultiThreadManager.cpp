#include "MultiThreadManager.h"
#include "MultiThreadQueue.h"

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

        if (data.message == WorkData::MESSAGE_FILTERING) {
            FutureData* futureData = static_cast<FutureData*>(p);
            RealDataPtr realData = std::make_shared<RealData>(data.pngData, data.wRange, data.hRange, data.filtername);
            // データが作られたのでFutureDataに値を入れる
            futureData->writeResult(realData);

            /* スレッドを終了する */
            pthread_exit(NULL);
        }
    }

    return nullptr;
}

RealData::RealData(unsigned char** pngData, Common::Range wRange, Common::Range hRange, Filter::Type filtername)
{
    // フィルタリング
    _result = Filter::apply(pngData, wRange, hRange, filtername);
}

FutureData::FutureData()
: _realData(nullptr)
, _ready(false)
, _mtx()
, _notReady()
{
}

bool FutureData::waitResult()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _notReady.wait(lock, [this] { return _ready; });

    return _realData->getResult();
}

void FutureData::writeResult(const RealDataPtr &realData)
{
    std::unique_lock<std::mutex> lock(_mtx);

    if (_ready) {
        return;
    }
    _realData = realData;
    _ready = true;

    _notReady.notify_all();
}

FutureDataPtr Host::request(WorkData workData)
{
    // Futureクラスのインスタンスを作る
    FutureDataPtr futureData = std::make_shared<FutureData>();

    /* スレッドを起動する */
    pthread_t th;
    pthread_create(&th, NULL, worker, futureData.get());
    pthread_detach(th);

    // キューにデータを格納
    queue.enqueue(workData);

    return futureData;
}
