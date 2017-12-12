#include "Image.h"

namespace {
        const std::string FILE_PATH = "Resources/";
}  // namespace

Image::Image(const std::string& filename)
: _fParams()
{
    _fParams.filename = filename;
}

void Image::read()
{
    auto inFileName = FILE_PATH + _fParams.filename;
    auto fp = fopen(inFileName.c_str(), "rb");

    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop infoPtr = png_create_info_struct(pngPtr);
    png_init_io(pngPtr, fp);

    // IHDRチャンク読み込み
    png_read_info(pngPtr, infoPtr);

    // IHDRチャンク情報取得
    png_get_IHDR(pngPtr, infoPtr, &_fParams.width, &_fParams.height,&_fParams.bitDepth, &_fParams.colorType, &_fParams.interlaceType, nullptr, nullptr);
    // 画像データ読み込み
    _png = (png_bytepp)malloc(_fParams.height * sizeof(png_bytep));
    for (int i = 0; i < _fParams.height; i++) {
        _png[i] = (png_bytep)malloc(png_get_rowbytes(pngPtr, infoPtr));
    }
    png_read_image(pngPtr, _png);

    png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp)nullptr);

    fclose(fp);
}

void Image::write()
{
    // フィルタをかけた後のデータを新しいファイルとして書き込み
    std::string outFileName = FILE_PATH + "out_" + _fParams.filename;
    auto fp = fopen(outFileName.c_str(), "wb");

    png_structp pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop infoPtr = png_create_info_struct(pngPtr);
    png_init_io(pngPtr, fp);

    // IHDRチャンク設定
    png_set_IHDR(pngPtr, infoPtr, _fParams.width, _fParams.height, _fParams.bitDepth, _fParams.colorType, _fParams.interlaceType, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // IHDRチャンク書き込み
    png_write_info(pngPtr, infoPtr);
    // 画像データ書き込み
    png_write_image(pngPtr, _png);
    // その他情報書き込み
    png_write_end(pngPtr, infoPtr);

    png_destroy_write_struct(&pngPtr, &infoPtr);

    fclose(fp);
}

void Image::free()
{
    // 読み込んだ画像データは解放
    for (int i = 0; i < _fParams.height; i++) {
        delete _png[i];
        _png[i] = nullptr;
    }
    delete _png;
    _png = nullptr;
}

RGBAArray Image::getImageData()
{
    RGBAArray imageData;

    for (int i = 0; i < _fParams.height; i++) {
        std::vector<Image::RGBA> data;
        for (int j = 0; j < _fParams.width * 4; j += 4) {
            Image::RGBA d;
            d.r = _png[i][j];
            d.g = _png[i][j + 1];
            d.b = _png[i][j + 2];
            d.a = _png[i][j + 3];
            data.push_back(d);
        }
        imageData.push_back(data);
    }

    return imageData;
}

RGBAArray Image::getImageData(const Common::Range width, const Common::Range height)
{
    RGBAArray imageData;

    for (int i = height.begin; i < height.end; i++) {
        std::vector<Image::RGBA> data;
        for (int j = width.begin; j < width.end * 4; j += 4) {
            Image::RGBA d;
            d.r = _png[i][j];
            d.g = _png[i][j + 1];
            d.b = _png[i][j + 2];
            d.a = _png[i][j + 3];
            data.push_back(d);
        }
        imageData.push_back(data);
    }

    return imageData;
}

void Image::setImageData(RGBAArray imageData)
{
    for (int i = 0; i < _fParams.height; i++) {
        auto itr = imageData[i].begin();
        for (int j = 0; j < _fParams.width * 4; j += 4) {
            _png[i][j]     = itr->r;
            _png[i][j + 1] = itr->g;
            _png[i][j + 2] = itr->b;
            _png[i][j + 3] = itr->a;
            itr++;
        }
    }
}
