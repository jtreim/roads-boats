#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>

using namespace building;

static const std::string BUILDING_NAMES[] = {
    "woodcutter",     "oil_rig",
    "quarry",         "clay_pit",
    "mine",           "sawmill",
    "coal_burner",    "papermill",
    "stone_factory",  "mint",
    "stock_exchange", "truck_factory",
    "raft_factory",   "rowboat_factory",
    "steamer_factory"};
static const size_t BUILDING_NAMES_SIZE =
    sizeof BUILDING_NAMES / sizeof BUILDING_NAMES[0];

Building::Building(const Building &other) : m_p_type(other.get_type()) {}
Building::Building(const Type &type) : m_p_type(type) {}
Building::~Building() {}

std::string Building::get_name() const
{
  std::string retval = "unknown";
  if (BUILDING_NAMES_SIZE > m_p_type)
  {
    retval = BUILDING_NAMES[m_p_type];
  }
  return retval;
}

Building Building::operator=(const Building &other) { return Building(other); }
Building Building::operator=(const Type &other) { return Building(other); }

bool Building::operator==(Building const &other) const
{
  return m_p_type == other.get_type();
}

bool Building::operator==(Type const &t) const { return m_p_type == t; }

nlohmann::json Building::to_json() const
{
  nlohmann::json retval;
  retval["type"] = get_name();
  return retval;
}