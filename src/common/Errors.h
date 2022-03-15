#ifndef ERRORS_H
#define ERRORS_H

namespace common
{
enum Error
{
  ERR_NONE = 0,
  ERR_FAIL = -1,
  ERR_INVALID = -2,
  ERR_NOT_FOUND = -3,
  ERR_UNKNOWN = -4
};
}

#endif