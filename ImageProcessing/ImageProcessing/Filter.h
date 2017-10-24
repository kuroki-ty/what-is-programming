#ifndef Filter_h
#define Filter_h

#include <stdio.h>
#include "Image.h"

class Filter
{
public:
    enum class Type {
        SMOOTHING,
    };

    static void apply(ImagePtr image, Type filtername);

private:
    static void smoothingFilter(ImagePtr image);
};

#endif /* Filter_h */
