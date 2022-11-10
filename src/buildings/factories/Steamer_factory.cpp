#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/factories/Steamer_factory.h>
#include <common/Errors.h>
#include <players/Color.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <tiles/Tile.h>

namespace building
{
Steamer_factory::Steamer_factory()
    : Secondary_producer(false, Type::steamer_factory, 1)
{
}
Steamer_factory::Steamer_factory(const Steamer_factory &other)
    : Secondary_producer(other.m_has_manager, Type::steamer_factory,
                         other.m_production_max, other.m_production_current)
{
}
Steamer_factory::~Steamer_factory() {}

Steamer_factory Steamer_factory::operator=(const Steamer_factory &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Steamer_factory::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (max - m_production_current <= 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::fuel) >= 2) &&
          (input.count(portable::Resource::Type::iron) > 0));
}

common::Error Steamer_factory::produce(
    portable::Cache &input,
    std::vector<portable::Transporter *> &nearby_transporters,
    std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and Steamers that can still be
  // produced this turn. 1 iron & 2 fuel makes 1 Steamer
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::iron)),
               (int)(max - m_production_current)));
  to_produce = static_cast<uint8_t>(std::min(
      (int)(input.count(portable::Resource::Type::fuel) / 2), (int)to_produce));

  common::Error err = input.remove(portable::Resource::Type::iron, to_produce);
  if (!err)
  {
    input.remove(portable::Resource::Type::fuel, to_produce * 2);
  }

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      if (nearby_transporters.size() > 0)
      {
        // TODO produce Steamer(s) based on adjacent transporters
      }
    }
    m_production_current += to_produce;
  }

  return common::ERR_NONE;
}

bool Steamer_factory::can_build(const portable::Cache &input,
                                const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) >= 2) &&
          (nullptr != tile) && (tile::is_valid(tile->get_terrain())) &&
          (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()) && (tile->is_shore()));
}

common::Error
Steamer_factory::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::boards, 2);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::stone, 2);
  }
  return err;
}

std::string Steamer_factory::to_string() const
{
  std::stringstream out;
  out << "<Steamer_factory::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Steamer_factory::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::steamer_factory);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Steamer_factory::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::steamer_factory])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Steamer factory type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Steamer factory currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Steamer factory max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building