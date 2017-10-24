#include <iostream>
#include <chrono>

#include "Image.h"
#include "Filter.h"

int main()
{
    // 時間計測開始
    auto start = std::chrono::system_clock::now();

    // データ読み込み
    auto image = std::make_shared<Image>("1920x1080.png");
    image->read();

    // データにフィルタをかける
    Filter::apply(image, Filter::Type::SMOOTHING);

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
