#ifndef Image_h
#define Image_h

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

class Image
{
public:
    enum RGBA {
        RED,
        GREEN,
        BLUE,
        ALPHA,
    };

    Image(const std::string& filename);
    void read();
    void write();
    void free();

    unsigned char** getPngData() const { return _png; };

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
};

using ImagePtr = std::shared_ptr<Image>;

#endif /* Image_h */
