#include "Filter.h"

void Filter::apply(ImagePtr image, Type filtername)
{
    // FilterNameごとにフィルタを適用する
    switch (filtername) {
        case Type::SMOOTHING:
            smoothingFilter(image);
            break;

        default:
            break;
    }
}

void Filter::smoothingFilter(ImagePtr image)
{
    auto width = image->getWidth();
    auto height = image->getHeight();
    auto imgData = image->getImageData();

    auto smooth = [width, height](const std::vector<std::vector<Image::RGBA>>& data, int m, int n, Image::RGBAType type) {
        unsigned short int sum = 0x0000;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int a = m + i;
                int b = n + j;

                // 端処理
                if (a < 0 || a > height - 1) a = m;
                if (b < 0 || b > width - 1)  b = n;

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
            imgData[i][j].r = smooth(imgData, i, j, Image::RGBAType::RED);
            imgData[i][j].g = smooth(imgData, i, j, Image::RGBAType::GREEN);
            imgData[i][j].b = smooth(imgData, i, j, Image::RGBAType::BLUE);
            imgData[i][j].a = smooth(imgData, i, j, Image::RGBAType::ALPHA);
        }
    }

    image->setImageData(imgData);
}
