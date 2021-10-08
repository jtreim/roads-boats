#ifndef PORTABLE_H
#define PORTABLE_H

#include <include/nlohmann_json.hpp>

namespace portable
{
class Portable
{
public:
  virtual ~Portable();

  // Abstract calls
  virtual common::Error to_json(nlohmann::json &j) const = 0;

protected:
private:
};

} // namespace portable
#endif