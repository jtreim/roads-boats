#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/factories/Rowboat_factory.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Rowboat_factory::Rowboat_factory()
    : Secondary_producer(false, Type::rowboat_factory, 1)
{
}
Rowboat_factory::Rowboat_factory(const Rowboat_factory &other)
    : Secondary_producer(other.m_has_manager, Type::rowboat_factory,
                         other.m_production_max, other.m_production_current)
{
}
Rowboat_factory::~Rowboat_factory() {}

Rowboat_factory Rowboat_factory::operator=(const Rowboat_factory &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Rowboat_factory::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  if (m_production_max - m_production_current == 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::boards) >= 5));
}

common::Error Rowboat_factory::produce(
    portable::Cache &input,
    std::vector<portable::Transporter *> &nearby_transporters,
    std::vector<std::unique_ptr<portable::Portable>> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and rowboats that can still be
  // produced this turn. 5 boards makes 1 rowboat
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::boards) / 5),
               (int)(m_production_max - m_production_current)));

  common::Error err =
      input.remove(portable::Resource::Type::boards, to_produce * 5);

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      if (nearby_transporters.size() > 0)
      {
        // TODO produce rowboat(s) based on adjacent transporters
      }
    }
    m_production_current += to_produce;
  }

  return common::ERR_NONE;
}

bool Rowboat_factory::can_build(const portable::Cache &input,
                                const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()) && (tile->is_shore()));
}

std::string Rowboat_factory::to_string() const
{
  std::stringstream out;
  out << "<Rowboat_factory::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Rowboat_factory::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::rowboat_factory);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Rowboat_factory::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::rowboat_factory])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as rowboat factory type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as rowboat factory currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as rowboat factory max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building