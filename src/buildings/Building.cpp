#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>

using namespace building;

static const std::string BUILDING_NAMES[] = {"woodcutter",
                                             "oil_rig",
                                             "quarry",
                                             "clay_pit",
                                             "mine",
                                             "sawmill",
                                             "coal_burner",
                                             "papermill",
                                             "stone_factory",
                                             "mint",
                                             "stock_exchange",
                                             "truck_factory",
                                             "raft_factory",
                                             "rowboat_factory",
                                             "steamer_factory",
                                             "road",
                                             "bridge",
                                             "wall"};
static const size_t BUILDING_NAMES_SIZE =
    sizeof BUILDING_NAMES / sizeof BUILDING_NAMES[0];

Building::Building(const Building &other) : m_p_type(other.get_type()) {}
Building::Building(const Type &type) : m_p_type(type) {}

common::Error Building::get_name(std::string &name) const
{
  common::Error err = common::ERR_FAIL;
  if (BUILDING_NAMES_SIZE > m_p_type)
  {
    name = BUILDING_NAMES[m_p_type];
    err = common::ERR_NONE;
  }
  return err;
}

Building Building::operator=(const Building &other) { return Building(other); }
Building Building::operator=(const Type &other) { return Building(other); }

bool Building::operator==(Building const &other) const
{
  return m_p_type == other.get_type();
}

bool Building::operator==(Type const &t) const { return m_p_type == t; }