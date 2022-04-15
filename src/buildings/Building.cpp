#include <nlohmann/json.hpp>

#include <buildings/Building.h>
#include <common/Errors.h>

namespace building
{

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

Building::Building(const Building &other) : m_type(other.get_type()) {}
Building::Building(const Type &type) : m_type(type) {}
Building::~Building() {}

std::string Building::get_name() const
{
  std::string retval = "unknown";
  if (BUILDING_NAMES_SIZE > m_type)
  {
    retval = BUILDING_NAMES[m_type];
  }
  return retval;
}

bool Building::operator==(Building const &other) const
{
  return m_type == other.get_type();
}
bool Building::operator==(Type const &t) const { return m_type == t; }
bool Building::operator!=(Building const &other) const
{
  return !(*this == other);
}
bool Building::operator!=(Type const &t) const { return !(*this == t); }

std::ostream &operator<<(std::ostream &os, const building::Building &b)
{
  os << "<Building::name=" << b.get_name() << ", type=" << b.get_type() << ">";
  return os;
}
} // namespace building