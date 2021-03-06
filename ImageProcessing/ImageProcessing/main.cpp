#include <iostream>
#include <vector>

#include "Image.h"
#include "Filter.h"
#include "MultiThreadManager.h"
#include "Common.h"
#include "Timer.h"

int main()
{
    // 時間計測開始
    Timer timer;
    timer.start();

    // データ読み込み
    auto image = std::make_shared<Image>("1920x1080.png");
    image->read();

    timer.showProcessingTime();

    // Hostクラス生成
    auto host = std::make_shared<Host>();

    // workerスレッドに渡すworkDataを作成
    constexpr uint32_t D_NUM = 4;
    const uint32_t D_HEIGHT = image->getHeight() / D_NUM;
    Common::Range wRange = {0, image->getWidth()};
    Common::Range hRange = {0, D_HEIGHT};
    std::vector<WorkData> workDataList;
    for (int i = 0; i < D_NUM; i++) {
        workDataList.push_back({
            WorkData::MESSAGE_FILTERING,
            image->getPngData(),
            wRange,
            hRange,
            Filter::Type::SMOOTHING
        });

        hRange.begin = hRange.end;
        hRange.end   = hRange.begin + D_HEIGHT;
    }

    // workerスレッド起動
    std::vector<FutureDataPtr> futureDataList;
    for (auto workData : workDataList) {
        auto data = host->request(workData);
        futureDataList.push_back(data);
    }

    // futureDataからrealDataを取得する
    for (auto futureData : futureDataList) {
        futureData->waitResult();
    }

    // データ出力
    image->write();

    // 時間計測終了
    timer.showProcessingTime();

    image->free();

    return 0;
}
