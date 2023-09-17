#include "config.h"

#include "block.h"
#include "util.h"

Block blocks[] = {
    {"sb-disk",    1800, 1 },
    {"sb-memory",  10,   2 },
    {"sb-loadavg", 5,    3 },
    {"sb-record",  0,    4 },
    {"sb-volume",  1,    5 },
    {"sb-date",    1,    6 },
};

const unsigned short blockCount = LEN(blocks);
