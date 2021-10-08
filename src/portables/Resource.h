#ifndef RESOURCE_H
#define RESOURCE_H

#include <include/nlohmann_json.hpp>

#include <common/Errors.h>
#include <portables/Portable.h>

namespace portable
{
class Resource : public Portable
{
public:
  enum Type
  {
    trunks = 0,
    boards,
    paper,
    goose,
    clay,
    stone,
    fuel,
    iron,
    gold,
    coins,
    stock,
    bomb,
    manager,
    pearl,
    marble,
    wood_carving,
    ring,
    sketch,
    pottery,
    statue
  };

  Resource(const Resource &other);
  Resource(const Type &r);
  ~Resource();

  inline Type get_type() const { return m_p_type; };

  inline common::Error get_name(std::string &name) const;
  common::Error to_json(nlohmann::json &j) const;

  Resource operator=(const Resource &other);
  Resource operator=(const Type &other);
  bool operator==(Resource const &other) const;
  bool operator==(Type const &t) const;

  // helpers
  friend std::ostream &operator<<(std::ostream &os, const Resource &resource);

protected:
private:
  Type m_p_type;
};
} // namespace portable
#endif // end RESOURCE_H