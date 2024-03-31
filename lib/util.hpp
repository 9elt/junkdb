#define FAIL(msg)                                                              \
    {                                                                          \
        fprintf(stderr, msg);                                                  \
        return 1;                                                              \
    }

#define FAIL_L(msg) FAIL(msg "\n")

#define FAIL_F(msg, ...)                                                       \
    {                                                                          \
        fprintf(stderr, msg, __VA_ARGS__);                                     \
        return 1;                                                              \
    }

#define FAIL_LF(msg, ...) FAIL_F(msg "\n", __VA_ARGS__)

#define ERROR(msg)                                                             \
    { fprintf(stderr, msg); }

#define ERROR_L(msg) ERROR(msg "\n")

#define ERROR_F(msg, ...)                                                      \
    { fprintf(stderr, msg, __VA_ARGS__); }

#define ERROR_LF(msg, ...) ERROR_F(msg "\n", __VA_ARGS__)

#define LOG(msg)                                                               \
    { fprintf(stdout, msg); }

#define LOG_L(msg) LOG(msg "\n")

#define LOG_F(msg, ...)                                                        \
    { fprintf(stdout, msg, __VA_ARGS__); }

#define LOG_LF(msg, ...) LOG_F(msg "\n", __VA_ARGS__)

unsigned long hash(char *str);
