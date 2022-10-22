#ifndef SECONDARY_H
#define SECONDARY_H

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
class Secondary_producer : public Building
{
public:
  Secondary_producer(const bool has_manager, const Type &t,
                     const uint8_t production_max,
                     const uint8_t production_current = 0)
      : m_has_manager(has_manager),
        Building(t, production_max, production_current)
  {
  }

  /// Resets the building for the next production phase.
  void reset() {
    m_production_current = 0;
    m_has_manager = false;
  }

  uint8_t count_remaining_production() const
  {
    uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
    return (max - m_production_current);
  }

  /// Determines whether the building's production can be doubled with
  /// electricity
  bool can_add_electricity() const { return false; }

  /// Determines whether the building's production can be doubled with a
  /// manager
  bool can_add_manager() const { return !m_has_manager; }

  /// Tries to double the production with electricity
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error add_electricity() { return common::ERR_FAIL; }

  /// Tries to double the production with a manager
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error add_manager()
  {
    common::Error result = common::ERR_FAIL;
    if (can_add_manager())
    {
      m_has_manager = true;
      result = common::ERR_NONE;
    }
    return result;
  }

  /// Tries to remove the electricity helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error remove_electricity() { return common::ERR_FAIL; }

  /// Tries to remove the manager helping the building production
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failure
  common::Error remove_manager()
  {
    common::Error result = common::ERR_FAIL;
    if ((m_has_manager) && (0 == m_production_current))
    {
      m_has_manager = false;
      result = common::ERR_NONE;
    }
    return result;
  }

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
          std::vector<portable::Portable *> &output) = 0;

  virtual std::string to_string() const = 0;
  virtual nlohmann::json to_json() const = 0;
  virtual void from_json(nlohmann::json const &j) = 0;

protected:
  bool m_has_manager;

private:
};
} // namespace building
#endif