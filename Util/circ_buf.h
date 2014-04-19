/* 
 * File:   circ_buf.h
 * Author: alex
 *
 * Created on March 29, 2014, 1:03 PM
 */

#ifndef CIRC_BUF_H
#define	CIRC_BUF_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUF_LEN 128

struct circ_buf {
    int begin;
    int end;
    int size;
    unsigned char buf[BUF_LEN+1];
};

#define INIT_CIRCBUF {0, 0, BUF_LEN+1}

/**
 * Check the given buffer to see if it is full
 * @param cb the circular buffer
 * @return 1 if full, 0 if not full
 */
int is_full(const struct circ_buf* cb);

/**
 * Check to see if the given buffer is empty
 * @param cb the circular buffer
 * @return 1 if empty, 0 if not empty
 */
int is_empty(const struct circ_buf* cb);

/**
 * Place an item at the end of the buffer
 * @param cb The buffer to append to
 * @param val The value to place at the end of the buffer
 * @return -1 if the operation was not siccessful (the buffer was full)
 */
int append(struct circ_buf *cb, unsigned char val);

/**
 * Use this to append values in ISRs
 * @param cb circ_buf struct
 * @param val unsigned char value to append
 * @param status pointer to int to place a status value in
 */
#define append_fast(cb, val, status) \
if((cb).begin == ((cb).end + 1)%(cb).size) { \
    *(status) = -1; \
} \
else { \
    (cb).buf[cb.end] = (val); \
    (cb).end = ((cb).end + 1)%(cb).size; \
    *(status) = 0; \
}

/**
 * Look at the item at the beginning of the buffer without removing it
 * @param cb The buffer to look at
 * @return -1 if the buffer is empty, otherwise the first item (uchar cast to int)
 */
int peek(const struct circ_buf *cb);

/**
 * Get the first item from the buffer and remove it
 * @param cb The buffer to use
 * @return -1 if the buffer is empty, otherwise the first item (uchar cast to int)
 */
int pop(struct circ_buf *cb);

/**
 * Get the number of elements in the buffer
 * @param cb the buffer to use
 * @return The length of the buffer
 */
int length(const struct circ_buf *cb);

#ifdef	__cplusplus
}
#endif

#endif	/* CIRC_BUF_H */

