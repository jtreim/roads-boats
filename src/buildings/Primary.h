#ifndef PRIMARY_H
#define PRIMARY_H

#include <memory>
#include <ostream>

#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

namespace building
{
class Primary_producer : public Building
{
public:
  Primary_producer(const bool is_powered, const Type &t,
                   const uint8_t production_max,
                   const uint8_t production_current = 0)
      : m_is_powered(is_powered),
        Building(t, production_max, production_current)
  {
  }

  /// Resets the building for the next production phase.
  void reset() {
    m_production_current = 0;
    m_is_powered = false;
  }

  uint8_t count_remaining_production() const
  {
    uint8_t max = (m_is_powered ? m_production_max * 2 : m_production_max);
    return (max - m_production_current);
  }

  /// Determines whether the building's production can be doubled with
  /// electricity
  bool can_add_electricity() const { return !m_is_powered; }

  /// Tries to double the production with electricity
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error add_electricity()
  {
    common::Error result = common::ERR_FAIL;
    if (can_add_electricity())
    {
      m_is_powered = true;
      result = common::ERR_NONE;
    }
    return result;
  }

  /// Tries to remove the electricity helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error remove_electricity()
  {
    common::Error result = common::ERR_FAIL;
    if ((m_is_powered) && (0 == m_production_current))
    {
      m_is_powered = false;
      result = common::ERR_NONE;
    }
    return result;
  }

  /// Determines whether the building's production can be doubled with a
  /// manager
  bool can_add_manager() const { return false; }

  /// Tries to double the production with a manager
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error add_manager() { return common::ERR_FAIL; }

  /// Tries to remove the manager helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error remove_manager() { return common::ERR_FAIL; }

  /// Determines whether the building can produce with the given resources
  /// @param[in] input  List of resources available to the building
  virtual bool
  can_produce(const portable::Cache &input,
              const std::vector<portable::Transporter *> nearby_transporters)
  {
    uint8_t max = (m_is_powered ? m_production_max * 2 : m_production_max);
    return (m_production_current < max);
  }

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
          std::vector<portable::Portable *> &output) = 0;

  virtual std::string to_string() const = 0;
  virtual nlohmann::json to_json() const = 0;
  virtual void from_json(nlohmann::json const &j) = 0;

protected:
  bool m_is_powered;

private:
};
} // namespace building
#endif