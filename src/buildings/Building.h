#ifndef BUILDING_H
#define BUILDING_H

#include <memory>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <portables/Portable.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>

namespace building
{
class Building
{
public:
  enum Type
  {
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
    truck_factory,
    raft_factory,
    rowboat_factory,
    steamer_factory
  };

  Building(const Building &other);
  Building(const Type &r);
  ~Building();

  inline Type get_type() const { return m_type; };
  inline std::string get_name() const;

  bool operator==(Building const &other) const;
  bool operator==(Type const &t) const;
  bool operator!=(Building const &other) const;
  bool operator!=(Type const &t) const;

  /// Determines whether the building can produce with the given resources
  /// @param[in] input  List of resources available to the building
  virtual bool can_produce(const std::vector<portable::Resource> input) = 0;

  /// Produces resource/transporter with the input resources.
  /// @param[in] input  Resources building can use to produce.
  /// @param[in] nearby_transporters  Transporters that currently have access to
  /// this building.
  /// @param[out] output  Result of production. null on failure to produce
  /// @return
  ///   - common::ERR_NONE on success.
  ///   - common::ERR_INVALID on invalid resource or transporter input.
  ///   - common::ERR_FAIL on insufficient resources given as input.
  virtual common::Error produce(
      std::vector<portable::Resource> &input,
      std::vector<std::shared_ptr<portable::Transporter>> &nearby_transporters,
      std::shared_ptr<portable::Portable> &output) = 0;

  // helpers
  friend std::ostream &operator<<(std::ostream &os,
                                  const building::Building &b);

protected:
private:
  Type m_type;
};

} // namespace building

#endif // end BUILDING_H