#ifndef BUILDING_H
#define BUILDING_H

#include <nlohmann/json.hpp>

#include <common/Errors.h>

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
    steamer_factory,
    road,
    bridge,
    wall
  };

  Building(const Building &other);
  Building(const Type &r);
  ~Building();

  inline Type get_type() const { return m_p_type; };
  inline common::Error get_name(std::string &name) const;
  void to_json(nlohmann::json &j) const;

  Building operator=(const Building &other);
  Building operator=(const Type &other);
  bool operator==(Building const &other) const;
  bool operator==(Type const &t) const;

  // helpers
  friend std::ostream &operator<<(std::ostream &os, const Building &building);

protected:
private:
  Type m_p_type;
};

} // namespace building
#endif // end BUILDING_H