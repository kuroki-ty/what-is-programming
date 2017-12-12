#ifndef Filter_h
#define Filter_h

#include <stdio.h>
#include "Image.h"
#include "Common.h"

class Filter
{
public:
    enum class Type {
        SMOOTHING,
    };

    static RGBAArray apply(RGBAArray imageData, uint32_t width, uint32_t height, Type filtername);

private:
    static RGBAArray smoothingFilter(RGBAArray imageData, uint32_t width, uint32_t height);
};

#endif /* Filter_h */
