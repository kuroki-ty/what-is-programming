#include <iostream>
#include <vector>
#include <chrono>

#include "Image.h"
#include "Filter.h"
#include "MultiThreadManager.h"
#include "Common.h"

int main()
{
    // 時間計測開始
    auto start = std::chrono::system_clock::now();

    // データ読み込み
    auto image = std::make_shared<Image>("1920x1080.png");
    image->read();

    // Hostクラス生成
    auto host = std::make_shared<Host>();

    // workerスレッドに渡すworkDataを作成
    constexpr uint32_t D_NUM = 108;
    const uint32_t D_HEIGHT = image->getHeight() / D_NUM;
    Common::Range wRange = {0, image->getWidth()};
    Common::Range hRange = {0, D_HEIGHT};
    std::vector<WorkData> workDataList;
    for (int i = 0; i < D_NUM; i++) {
        workDataList.push_back({
            WorkData::MESSAGE_FILTERING,
            image->getImageData(wRange, hRange),
            wRange.end - wRange.begin,
            hRange.end - hRange.begin,
            Filter::Type::SMOOTHING
        });

        hRange.begin = hRange.end;
        hRange.end   = D_HEIGHT * (i + 2);
    }

    // workerスレッド起動
    std::vector<FutureDataPtr> futureDataList;
    for (auto workData : workDataList) {
        auto data = host->request(workData);
        futureDataList.push_back(data);
    }

    // futureDataからrealDataを取得する
    RGBAArray data;
    for (auto futureData : futureDataList) {
        auto d = futureData->waitResult();
        data.insert(data.end(), d.begin(), d.end());
    }
    image->setImageData(data);

    // データ出力
    image->write();

    // 時間計測終了
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    printf("処理時間: %lld[ms]\n", msec);

    image->free();

    return 0;
}
