#ifndef DELETE_COPY_H
#define DELETE_COPY_H

#define DISABLE_COPY(CLASS_NAME)                        \
    CLASS_NAME(const CLASS_NAME &) = delete;            \
    CLASS_NAME(CLASS_NAME &&) = delete;                 \
    CLASS_NAME &operator=(const CLASS_NAME &) = delete; \
    CLASS_NAME &operator=(CLASS_NAME &&) = delete;



class noncopyable
{
    public:
        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
        noncopyable() = default;
        ~noncopyable() = default;
};

#endif
