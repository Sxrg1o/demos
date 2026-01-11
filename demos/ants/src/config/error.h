#ifndef ERROR_H
#define ERROR_H

typedef enum {
  RC_SUCCESS = 0,
  RC_UNEXPECTED_ERROR,
  RC_INDEX_ERROR,
  RC_NULL_VALUE_ERROR,
} ReturnCode;

#endif // ERROR_H