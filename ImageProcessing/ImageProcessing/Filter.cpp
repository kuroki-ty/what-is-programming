#include "Filter.h"
#include "Image.h"

bool Filter::apply(unsigned char** pngData, Common::Range wRange, Common::Range hRange, Type filtername)
{
    bool ret = false;
    // FilterNameごとにフィルタを適用する
    switch (filtername) {
        case Type::SMOOTHING:
            ret = smoothingFilter(pngData, wRange, hRange);
            break;

        default:
            break;
    }
    return ret;
}

bool Filter::smoothingFilter(unsigned char** pngData, Common::Range wRange, Common::Range hRange)
{
    auto smooth = [wRange, hRange](unsigned char** data, int m, int n, Image::RGBA type) {
        unsigned short int sum = 0x0000;
        for (int i = -1; i <= 1; i++) {
            for (int j = -4; j <= 4; j += 4) {
                int a = m + i;
                int b = n + j;

                // 端処理
                if (a < 0 || a > hRange.end - 1)     a = m;
                if (b < 0 || b > wRange.end * 4  - 1) b = n;

                switch (type) {
                    case Image::RGBA::RED:
                        sum += data[a][b];
                        break;
                    case Image::RGBA::GREEN:
                        sum += data[a][b + 1];
                        break;
                    case Image::RGBA::BLUE:
                        sum += data[a][b + 2];
                        break;
                    case Image::RGBA::ALPHA:
                        sum += data[a][b + 3];
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
    for (int i = hRange.begin; i < hRange.end; i++) {
        for (int j = wRange.begin; j < wRange.end * 4; j += 4) {
            pngData[i][j]     = smooth(pngData, i, j, Image::RGBA::RED);
            pngData[i][j + 1] = smooth(pngData, i, j, Image::RGBA::GREEN);
            pngData[i][j + 2] = smooth(pngData, i, j, Image::RGBA::BLUE);
            pngData[i][j + 3] = smooth(pngData, i, j, Image::RGBA::ALPHA);
        }
    }

    return true;
}
