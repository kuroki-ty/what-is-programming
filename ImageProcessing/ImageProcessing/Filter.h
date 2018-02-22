#ifndef Filter_h
#define Filter_h

#include <stdio.h>
#include "Common.h"

class Filter
{
public:
    enum class Type {
        SMOOTHING,
    };

    static bool apply(unsigned char** pngData, Common::Range wRange, Common::Range hRange, Type filtername);

private:
    static bool smoothingFilter(unsigned char** pngData, Common::Range wRange, Common::Range hRange);
};

#endif /* Filter_h */
