#ifndef PORTABLE_H
#define PORTABLE_H

#include <nlohmann/json.hpp>

namespace portable
{
class Portable
{
public:
  Portable(){};
  virtual ~Portable(){};

  // Abstract calls
  virtual common::Error to_json(nlohmann::json &j) const = 0;

protected:
private:
};

} // namespace portable
#endif