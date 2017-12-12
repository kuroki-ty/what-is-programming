#include "Filter.h"

RGBAArray Filter::apply(RGBAArray imageData, uint32_t width, uint32_t height, Type filtername)
{
    // FilterNameごとにフィルタを適用する
    switch (filtername) {
        case Type::SMOOTHING:
            imageData = smoothingFilter(imageData, width, height);
            break;

        default:
            break;
    }
    return imageData;
}

RGBAArray Filter::smoothingFilter(RGBAArray imageData, uint32_t width, uint32_t height)
{
    auto smooth = [width, height](const RGBAArray& data, int m, int n, Image::RGBAType type) {
        unsigned short int sum = 0x0000;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int a = m + i;
                int b = n + j;

                // 端処理
                if (a < 0 || a > height - 1) a = m;
                if (b < 0 || b > width  - 1) b = n;

                switch (type) {
                    case Image::RGBAType::RED:
                        sum += data[a][b].r;
                        break;
                    case Image::RGBAType::GREEN:
                        sum += data[a][b].g;
                        break;
                    case Image::RGBAType::BLUE:
                        sum += data[a][b].b;
                        break;
                    case Image::RGBAType::ALPHA:
                        sum += data[a][b].a;
                        break;

                    default:
                        break;
                }

            }
        }
        auto ret = static_cast<unsigned char>(sum / 9);
        return ret;
    };

    // 平滑化
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            imageData[i][j].r = smooth(imageData, i, j, Image::RGBAType::RED);
            imageData[i][j].g = smooth(imageData, i, j, Image::RGBAType::GREEN);
            imageData[i][j].b = smooth(imageData, i, j, Image::RGBAType::BLUE);
            imageData[i][j].a = smooth(imageData, i, j, Image::RGBAType::ALPHA);
        }
    }

    return imageData;
}
