#include "config.h"

#include "block.h"
#include "util.h"

Block blocks[] = {
    {"sb-forecast",  900,  1 },
    {"sb-disk",      1800, 2 },
    {"sb-memory",    10,   3 },
    {"sb-loadavg",   5,    4 },
    {"sb-music",     1,    5 },
    {"sb-volume",    1,    6 },
    {"sb-date",      1,    7 },
    {"sb-user",      0,    8 },
};

const unsigned short blockCount = LEN(blocks);
