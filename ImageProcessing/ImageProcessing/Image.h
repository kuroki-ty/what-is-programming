#ifndef Image_h
#define Image_h

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <png.h>
#include <unordered_map>
#include "Common.h"

class Image
{
public:
    enum RGBAType {
        RED,
        GREEN,
        BLUE,
        ALPHA,
    };

    struct RGBA {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    Image(const std::string& filename);
    void read();
    void write();
    void free();

    std::vector<std::vector<RGBA>> getImageData();
    std::vector<std::vector<RGBA>> getImageData(const Common::Range wRange, const Common::Range hRange);
    void setImageData(const std::vector<std::vector<RGBA>>& imageData);
    void setImageData(const std::vector<std::vector<RGBA>>& imageData, const Common::Range hRange);
    void toPngData();
    unsigned int getWidth() const { return _fParams.width; }
    unsigned int getHeight() const { return _fParams.height; }

private:
    struct FileParams {
        std::string filename;
        unsigned int width;
        unsigned int height;
        int bitDepth;
        int colorType;
        int interlaceType;
    };

    FileParams _fParams;
    unsigned char** _png;

    std::unordered_map<uint32_t, std::vector<std::vector<Image::RGBA>>> _imageList;
};

using ImagePtr = std::shared_ptr<Image>;
using RGBAArray = std::vector<std::vector<Image::RGBA>>;

#endif /* Image_h */
