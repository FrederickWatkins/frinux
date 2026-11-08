#ifndef KPRINT_H
#define KPRINT_H

#include <stdarg.h>
#include <stddef.h>

#define kprint(fmt, ...) \
    ({ \
        _kprint_loc; \
        _kprint(fmt, ##__VA_ARGS__); \
    })

#define _kprint_loc \
    ({ \
        _kprint("%s:%s:%d: ", __FILE__, __func__, __LINE__); \
    })

#define kprint_force(fmt, ...) \
    ({ \
        _kprint_loc_force; \
        _kprint_force(fmt, ##__VA_ARGS__); \
    })

#define _kprint_loc_force \
    ({ \
        _kprint_force("%s:%s:%d: ", __FILE__, __func__, __LINE__); \
    })

int _kprint(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

int _kprint_force(const char* fmt, ...) __attribute((format(printf, 1, 2)));

#endif /* KPRINT_H */
