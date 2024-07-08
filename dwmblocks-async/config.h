#ifndef CONFIG_H
#define CONFIG_H

// String used to delimit block outputs in the status.
#define DELIMITER "  "

// Maximum number of Unicode characters that a block can output.
#define MAX_BLOCK_OUTPUT_LENGTH 45

// Control whether blocks are clickable.
#define CLICKABLE_BLOCKS 1

// Control whether a leading delimiter should be prepended to the status.
#define LEADING_DELIMITER 0

// Control whether a trailing delimiter should be appended to the status.
#define TRAILING_DELIMITER 0

// Define blocks for the status feed as X(icon, cmd, interval, signal).
#define BLOCKS(X)             \
    X("", "sb-forecast", 900, 1)  \
    X("", "sb-disk", 1800, 2)   \
    X("", "sb-memory", 10, 3) \
    X("", "sb-loadavg", 5, 4) \
    X("", "sb-music", 1, 5) \
    X("", "sb-volume", 1, 6)     \
    X("", "sb-date", 1, 7)  \
    X("", "sb-user", 0, 8)
#endif  // CONFIG_H
