

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif


#ifdef DEBUG
#define printt(fmt, ...) \
    do { \
        struct timeb tp; \
        ftime(&tp); \
        printf("%ld.%d: " fmt, tp.time, tp.millitm, ##__VA_ARGS__); \
    } while (0)
#else
#define printt(...) while(0){}



/* prints error if return value is equal to errrtn */
#define ec_et(var, errrtn) \
    {\
        if((intptr_t)(var) == (intptr_t)(errrtn)) { \
            char error[50];\
            sprintf(error, "\033[31mError at %s, %s, %d\033[0m", __func__, __FILE__, __LINE__);\
            perror(error);\
        }\
    }
/* prints error if return value is different from rtn */
#define ec_df(var, rtn) \
    {\
        if((intptr_t)(var) != (intptr_t)(rtn)) { \
            char error[50];\
            sprintf(error, "Error at %s, %s, %d", __func__, __FILE__, __LINE__);\
            perror(error);\
        }\
    }
/* prints error if return value is less than value */
#define ec_lt(var, rtn) \
    {\
        if((intptr_t)(var) < (intptr_t)(rtn)) { \
            char error[50];\
            sprintf(error, "Error at %s, %s, %d", __func__, __FILE__, __LINE__);\
            perror(error);\
        }\
    }

#define ec_et_null(x) ec_et(x, NULL) // prints error if rval equal to NULL
#define ec_et_m1(x) ec_et(x, -1)     // prints error if rval equal to -1
#define ec_df_1(x) ec_df(x, 1)       // prints error if rval different from 1
#define ec_df_0(x) ec_df(x, 0)       // prints error if rval different from 0
#define ec_lt_0(x) ec_lt(x, 0)       // prints error if rval is negative


