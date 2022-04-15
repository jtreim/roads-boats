#ifndef PORTABLE_H
#define PORTABLE_H

#include <set>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Player.h>

namespace portable
{
class Portable
{
public:
  enum Object
  {
    invalid = -1,
    resource = 0,
    transporter
  };

  static std::string to_string(Object t)
  {
    switch (t)
    {
    case Object::resource:
      return "resource";
    case Object::transporter:
      return "transporter";
    default:
      return "unknown";
    }
  }

  static bool is_valid(Object t) { return ((0 <= t) && (2 > t)); }

  NLOHMANN_JSON_SERIALIZE_ENUM(Object, {{invalid, nullptr},
                                        {resource, "resource"},
                                        {transporter, "transporter"}});

  Portable(std::set<player::Color> carriers, Object obj)
      : m_carriers(carriers), m_object(obj)
  {
    if (m_carriers.contains(player::Color::neutral))
    {
      m_carriers.erase(player::Color::neutral);
    }
    if (m_carriers.contains(player::Color::invalid))
    {
      m_carriers.erase(player::Color::invalid);
    }
  }
  Portable(Object obj) : m_object(obj) {}
  virtual ~Portable() {}

  virtual void reset() { m_carriers.clear(); }

  virtual bool can_add_carrier(const player::Color player) const
  {
    return ((player::is_valid(player)) && (!m_carriers.contains(player)));
  };

  virtual common::Error add_carrier(const player::Color player)
  {
    if (!player::is_valid(player))
    {
      return common::ERR_INVALID;
    }
    if (m_carriers.contains(player))
    {
      return common::ERR_FAIL;
    }

    if (player != player::Color::neutral)
    {
      m_carriers.insert(player);
    }
    return common::ERR_NONE;
  };

  virtual common::Error remove_carrier(const player::Color player)
  {
    if (!player::is_valid(player))
    {
      return common::ERR_INVALID;
    }
    if (!m_carriers.contains(player))
    {
      return common::ERR_FAIL;
    }

    m_carriers.erase(player);
    return common::ERR_NONE;
  };

  inline bool was_carried() const { return (0 < m_carriers.size()); }

  inline bool was_carried_by(player::Color player) const
  {
    return m_carriers.contains(player);
  }

  inline Object get_object() const { return m_object; }

  inline std::set<player::Color> get_carriers() const { return m_carriers; }

protected:
  std::set<player::Color> m_carriers;
  Object m_object;

private:
};
} // namespace portable

#endif