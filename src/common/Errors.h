#ifndef ERRORS_H
#define ERRORS_H

namespace common
{
static const enum Error {
  ERR_NONE = 0,
  ERR_FAIL = -1,
  ERR_INVALID = -2,
  ERR_MISSING = -3,
  ERR_UNKNOWN = -4
};
}

#endif