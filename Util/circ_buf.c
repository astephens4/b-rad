#include "circ_buf.h"

int is_full(const struct circ_buf* cb)
{
    if(cb->begin == (cb->end+1)%cb->size) {
        return 1;
    }
    return 0;
}

int is_empty(const struct circ_buf* cb)
{
    if(cb->begin == cb->end) {
        return 1;
    }
    return 0;
}

int append(struct circ_buf *cb, unsigned char val)
{
    if(is_full(cb) == 0) {
        cb->buf[cb->end] = val;
        cb->end = (cb->end + 1)%cb->size;
        return 1;
    }
    return 0;
}

int peek(const struct circ_buf *cb)
{
    if(is_empty(cb) == 0) {
        return (int)cb->buf[cb->begin];
    }
    return -1;
}

int pop(struct circ_buf *cb)
{
    if(is_empty(cb) == 0) {
        int ret = (int)cb->buf[cb->begin];
        cb->begin = (cb->begin + 1)%cb->size;
        return ret;
    }
    return -1;
}

int length(const struct circ_buf *cb)
{
    int len = cb->end - cb->begin;
    if(len < 0) {
        len += cb->size;
    }
    return len;
}
