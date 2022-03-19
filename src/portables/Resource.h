#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

#include <nlohmann/json.hpp>

namespace portable
{
enum Resource
{
  invalid = -1,
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

static const std::string RESOURCE_NAMES[] = {
    "trunks", "boards",       "paper", "goose",  "clay",    "stone",   "fuel",
    "iron",   "gold",         "coins", "stock",  "bomb",    "manager", "pearl",
    "marble", "wood_carving", "ring",  "sketch", "pottery", "statue"};
NLOHMANN_JSON_SERIALIZE_ENUM(Resource,
                             {{invalid, nullptr},
                              {trunks, RESOURCE_NAMES[trunks]},
                              {boards, RESOURCE_NAMES[boards]},
                              {paper, RESOURCE_NAMES[paper]},
                              {goose, RESOURCE_NAMES[goose]},
                              {clay, RESOURCE_NAMES[clay]},
                              {stone, RESOURCE_NAMES[stone]},
                              {fuel, RESOURCE_NAMES[fuel]},
                              {iron, RESOURCE_NAMES[iron]},
                              {gold, RESOURCE_NAMES[gold]},
                              {coins, RESOURCE_NAMES[coins]},
                              {stock, RESOURCE_NAMES[stock]},
                              {bomb, RESOURCE_NAMES[bomb]},
                              {manager, RESOURCE_NAMES[manager]},
                              {pearl, RESOURCE_NAMES[pearl]},
                              {marble, RESOURCE_NAMES[marble]},
                              {wood_carving, RESOURCE_NAMES[wood_carving]},
                              {ring, RESOURCE_NAMES[ring]},
                              {sketch, RESOURCE_NAMES[sketch]},
                              {pottery, RESOURCE_NAMES[pottery]},
                              {statue, RESOURCE_NAMES[statue]}});
static const size_t RESOURCE_NAMES_SIZE =
    sizeof RESOURCE_NAMES / sizeof RESOURCE_NAMES[0];
static const size_t RESOURCE_TYPES = RESOURCE_NAMES_SIZE;

/// Retrieves the name for the enum representation of a resource.
/// @param[in] res
/// @return
/// The string name for the resource. "unknown" if invalid resource value.
static std::string to_string(Resource res)
{
  if (res < RESOURCE_NAMES_SIZE)
  {
    return RESOURCE_NAMES[static_cast<uint8_t>(res)];
  }
  return "unknown";
}

static bool is_valid(const Resource r)
{
  return ((0 <= r) && (RESOURCE_NAMES_SIZE > r));
}
} // namespace portable

static std::ostream &operator<<(std::ostream &output,
                                const portable::Resource &r)
{
  output << portable::to_string(r);
  return output;
}

#endif // end RESOURCE_H