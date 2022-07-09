#ifndef BUILDING_H
#define BUILDING_H

#include <memory>

#include <common/Errors.h>
#include <nlohmann/json.hpp>
#include <portables/Portable.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

namespace building
{
struct Wall
{
  player::Color color;
  uint8_t thickness;

  Wall(player::Color c, uint8_t t) : color(c), thickness(t) {}
  Wall() : color(player::Color::invalid), thickness(0) {}
  ~Wall() {}

  bool operator==(const Wall other) const
  {
    return ((color == other.color) && (thickness == other.thickness));
  }
  bool operator==(const Wall other)
  {
    return ((color == other.color) && (thickness == other.thickness));
  }
  bool operator!=(const Wall other) const { return !(*this == other); }
  bool operator!=(const Wall other) { return !(*this == other); }

  Wall operator=(const Wall &other)
  {
    color = other.color;
    thickness = other.thickness;
    return (*this);
  }
};

class Building
{
public:
  enum Type
  {
    invalid = -1,
    woodcutter = 0,
    oil_rig,
    quarry,
    clay_pit,
    mine,
    sawmill,
    coal_burner,
    papermill,
    stone_factory,
    mint,
    stock_exchange,
    wagon_factory,
    truck_factory,
    raft_factory,
    rowboat_factory,
    steamer_factory
  };

  Building(const Type &t, uint8_t production_max,
           uint8_t production_current = 0)
      : m_type(t), m_production_max(production_max),
        m_production_current(production_current){};
  virtual ~Building() {}

  inline Type get_type() const { return m_type; }

  bool operator==(Type const &t) const { return m_type == t; }
  bool operator!=(Type const &t) const { return m_type == t; }

  /// Resets the building for the next production phase.
  virtual void reset() { m_production_current = 0; }

  /// Returns a count of how many more resources the building can produce before
  /// resetting.
  uint8_t count_remaining_production() const
  {
    return (m_production_max - m_production_current);
  }

  /// Determines whether the building's production can be doubled with
  /// electricity
  virtual bool can_add_electricity() const = 0;

  /// Determines whether the building's production can be doubled with a manager
  virtual bool can_add_manager() const = 0;

  /// Tries to double the production with electricity
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  virtual common::Error add_electricity() = 0;

  /// Tries to double the production with a manager
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  virtual common::Error add_manager() = 0;

  /// Tries to remove the electricity helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  virtual common::Error remove_electricity() = 0;

  /// Tries to remove the manager helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  virtual common::Error remove_manager() = 0;

  /// Determines whether the building can produce with the given resources
  /// @param[in] input  List of resources available to the building
  virtual bool can_produce(
      const portable::Cache &input,
      const std::vector<portable::Transporter *> nearby_transporters) = 0;

  /// Produces resource/transporter with the input resources.
  /// @param[in] input  Resources building can use to produce.
  /// @param[in] nearby_transporters  Transporters that currently have access to
  /// this building.
  /// @param[out] output  Result of production. null on failure to produce
  /// @return
  ///   - common::ERR_NONE on success.
  ///   - common::ERR_INVALID on invalid resource or transporter input.
  ///   - common::ERR_FAIL on insufficient resources given as input.
  virtual common::Error
  produce(portable::Cache &input,
          std::vector<portable::Transporter *> &nearby_transporters,
          std::vector<std::unique_ptr<portable::Portable>> &output) = 0;

  // Helpers
  virtual std::string to_string() const = 0;
  friend std::ostream &operator<<(std::ostream &os, const Building &b)
  {
    os << b.to_string();
    return os;
  }
  virtual nlohmann::json to_json() const = 0;
  virtual void from_json(nlohmann::json const &j) = 0;

protected:
  Type m_type;
  uint8_t m_production_max;
  uint8_t m_production_current;

private:
};
static const std::string BUILDING_NAMES[] = {
    "woodcutter",    "oil_rig",      "quarry",          "clay_pit",
    "mine",          "sawmill",      "coal_burner",     "papermill",
    "stone_factory", "mint",         "stock_exchange",  "wagon_factory",
    "truck_factory", "raft_factory", "rowboat_factory", "steamer_factory"};
static const size_t BUILDING_NAMES_SIZE =
    sizeof BUILDING_NAMES / sizeof BUILDING_NAMES[0];

static bool is_valid(const Building::Type t)
{
  return ((0 <= static_cast<int>(t)) &&
          (BUILDING_NAMES_SIZE > static_cast<size_t>(t)));
}

static std::string to_string(const Building::Type t)
{
  if (is_valid(t))
  {
    return BUILDING_NAMES[t];
  }
  return "unknown";
}
} // namespace building

#endif // end BUILDING_H