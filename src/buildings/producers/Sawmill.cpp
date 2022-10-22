#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Sawmill.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Sawmill::Sawmill() : Secondary_producer(false, Type::sawmill, 6) {}
Sawmill::Sawmill(const Sawmill &other)
    : Secondary_producer(other.m_has_manager, Type::sawmill,
                         other.m_production_max, other.m_production_current)
{
}
Sawmill::~Sawmill() {}

Sawmill Sawmill::operator=(const Sawmill &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Sawmill::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (max - m_production_current <= 0)
  {
    return false;
  }

  return (input.count(portable::Resource::Type::trunks) > 0);
}

common::Error
Sawmill::produce(portable::Cache &input,
                 std::vector<portable::Transporter *> &nearby_transporters,
                 std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and boards that can still be
  // produced this turn. 1 trunk makes 2 boards
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::trunks) * 2),
               (int)(max - m_production_current)));

  common::Error err =
      input.remove(portable::Resource::Type::trunks, to_produce / 2);
  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      output.push_back(new portable::Resource(
          portable::Resource::Type::boards));
    }
    m_production_current += to_produce;
  }

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return err;
}

bool Sawmill::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

common::Error Sawmill::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::boards, 2);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::stone, 1);
  }
  return err;
}

std::string Sawmill::to_string() const
{
  std::stringstream out;
  out << "<Sawmill::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Sawmill::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::sawmill);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Sawmill::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::sawmill])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Sawmill type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Sawmill currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 6)) || ((has_manager) && (max != 12)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Sawmill max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building