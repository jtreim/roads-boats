#ifndef RESOURCE_H
#define RESOURCE_H

#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include <players/Color.h>
#include <portables/Portable.h>

namespace portable
{
static const std::string RESOURCE_NAMES[] = {
    "trunks", "boards",       "paper", "goose",  "clay",    "stone",   "fuel",
    "iron",   "gold",         "coins", "stock",  "bomb",    "manager", "pearl",
    "marble", "wood_carving", "ring",  "sketch", "pottery", "statue"};
static const size_t RESOURCE_NAMES_SIZE =
    sizeof RESOURCE_NAMES / sizeof RESOURCE_NAMES[0];
static const size_t RESOURCE_TYPES = RESOURCE_NAMES_SIZE;

class Resource : public Portable
{
public:
  enum Type
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

  /// Retrieves the name for the enum representation of a resource.
  /// @param[in] res
  /// @return
  /// The string name for the resource. "unknown" if invalid resource value.
  static std::string to_string(const Type t)
  {
    if (static_cast<size_t>(t) < RESOURCE_NAMES_SIZE)
    {
      return RESOURCE_NAMES[static_cast<uint8_t>(t)];
    }
    return "unknown";
  }

  static Type from_string(const std::string s)
  {
    Type result = Type::invalid;
    for (size_t i = 0; i < RESOURCE_NAMES_SIZE; i++)
    {
      if (RESOURCE_NAMES[i] == s)
      {
        result = static_cast<Type>(i);
        break;
      }
    }
    return result;
  }

  static bool is_valid(const Type t)
  {
    return ((0 <= t) && (RESOURCE_NAMES_SIZE > static_cast<size_t>(t)));
  }

  Resource() : Portable(Portable::Object::resource), m_type(Type::invalid) {}

  Resource(const Type res_type)
      : Portable(Portable::Object::resource), m_type(res_type), m_is_held(false)
  {
  }
  Resource(const Type res_type, const std::set<player::Color> carriers)
      : Portable(carriers, Portable::Object::resource), m_type(res_type),
        m_is_held(false)
  {
  }
  ~Resource(){};

  bool operator==(Resource &other) { return m_type == other.m_type; }
  bool operator==(Resource const &other) const
  {
    return m_type == other.m_type;
  }
  bool operator!=(Resource &other) { return !(*this == other); }
  bool operator!=(Resource const &other) const { return !(*this == other); }
  Resource operator=(const Resource &other)
  {
    m_type = other.m_type;
    m_is_held = other.m_is_held;
    m_object = other.m_object;
    m_carriers = other.m_carriers;
    return (*this);
  }

  void drop() { m_is_held = false; }
  common::Error pickup(const player::Color color)
  {
    if (can_add_carrier(color))
    {
      m_is_held = true;
      return add_carrier(color);
    }
    return common::ERR_FAIL;
  }
  bool is_held() { return m_is_held; }

  inline Type get_type() { return m_type; }

  friend void to_json(nlohmann::json &j, const Resource &res)
  {
    std::vector<std::string> carriers;
    for (auto clr : res.m_carriers)
    {
      carriers.push_back(player::to_string(clr));
    }
    j["carriers"] = carriers;

    j["type"] = to_string(res.m_type);
  }
  friend void from_json(const nlohmann::json &j, Resource &res)
  {
    std::set<player::Color> carriers;
    for (auto clr : j.at("carriers").get<std::vector<player::Color>>())
    {
      if ((!player::is_valid(clr)) || (player::Color::neutral == clr))
      {
        std::stringstream msg;
        msg << player::to_string(clr)
            << " color listed as carrier of Resource!";
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
      carriers.insert(clr);
    }

    Resource::Type res_type = from_string(j.at("type").get<std::string>());
    if (!is_valid(res_type))
    {
      std::stringstream msg;
      msg << "Invalid resource type " << to_string(res_type) << " listed!";
      throw nlohmann::json::type_error::create(501, msg.str(), j);
    }
    res = Resource(res_type, carriers);
  }

protected:
private:
  bool m_is_held;
  Type m_type;

}; // namespace portable

static std::ostream &operator<<(std::ostream &output,
                                const portable::Resource::Type &r)
{
  output << portable::Resource::to_string(r);
  return output;
}

} // namespace portable

#endif // end RESOURCE_H