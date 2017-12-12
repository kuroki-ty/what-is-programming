#ifndef MultiThreadManager_h
#define MultiThreadManager_h

#include <stdio.h>
#include <vector>
#include <numeric>
#include <pthread.h>
#include <condition_variable>
#include "Image.h"
#include "Filter.h"
#include "Common.h"

/**
 * 実際に計算してデータ(RealData)を作成するクラス
 * 計算に時間がかかる。
 */
class RealData
{
public:
    RealData(RGBAArray imageData, uint32_t width, uint32_t height, Filter::Type filtername);

    const RGBAArray& getResult() { return _result; }

private:
    RGBAArray _result;
};

using RealDataPtr = std::shared_ptr<RealData>;

/**
 * RealDataの引換券となるクラス
 */
class FutureData
{
public:
    FutureData();

    /** 結果が来るまで待って結果を返す。既に結果が書き込まれている場合はすぐに戻る */
    const RGBAArray& waitResult();

    /** 結果が入っているかを返す */
    bool tryResult() { return _ready; }

    /** 結果を書き込む。待っているスレッドをすべて起こす */
    void writeResult(const RealDataPtr& realData);

private:
    RealDataPtr _realData;
    bool _ready;

    std::mutex _mtx;
    std::condition_variable _notReady;
};

using FutureDataPtr = std::shared_ptr<FutureData>;

struct WorkData
{
    enum {
        MESSAGE_TERMINATE,
        MESSAGE_FILTERING,
    } message;
    RGBAArray imageData;
    uint32_t width;
    uint32_t height;
    Filter::Type filtername;
};

class Host
{
public:
    FutureDataPtr request(WorkData workData);
};

#endif /* MultiThreadManager_h */
