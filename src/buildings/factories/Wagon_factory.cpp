#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/factories/Wagon_factory.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Wagon_factory::Wagon_factory()
    : Secondary_producer(false, Type::wagon_factory, 1)
{
}
Wagon_factory::Wagon_factory(const Wagon_factory &other)
    : Secondary_producer(other.m_has_manager, Type::wagon_factory,
                         other.m_production_max, other.m_production_current)
{
}
Wagon_factory::~Wagon_factory() {}

Wagon_factory Wagon_factory::operator=(const Wagon_factory &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Wagon_factory::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (max - m_production_current == 0)
  {
    return false;
  }

  if (input.count(portable::Resource::Type::boards) < 2)
  {
    return false;
  }

  for (auto t : nearby_transporters)
  {
    if ((nullptr != t) &&
        (portable::Transporter::Type::donkey == t->get_type()))
    {
      return true;
    }
  }

  return false;
}

common::Error Wagon_factory::produce(
    portable::Cache &input,
    std::vector<portable::Transporter *> &nearby_transporters,
    std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and wagons that can still be
  // produced this turn. 2 boards & 1 donkey makes 1 wagon
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::boards) / 2),
               (int)(max - m_production_current)));

  uint8_t donkey_count = 0;
  for (auto t : nearby_transporters)
  {
    if ((nullptr != t) &&
        (portable::Transporter::Type::donkey == t->get_type()))
    {
      donkey_count++;
    }
  }

  to_produce =
      static_cast<uint8_t>(std::min((int)(donkey_count), (int)to_produce));

  common::Error err =
      input.remove(portable::Resource::Type::boards, to_produce * 2);
  if (!err)
  {
    input.remove(portable::Resource::Type::fuel, to_produce);
  }

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      if (nearby_transporters.size() > 0)
      {
        // TODO produce wagon(s) based on adjacent transporters
        // TODO remove a donkey for each one. Must happen after producing
        // the wagon.
      }
    }
    m_production_current += to_produce;
  }

  return common::ERR_NONE;
}

bool Wagon_factory::can_build(const portable::Cache &input,
                              const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

std::string Wagon_factory::to_string() const
{
  std::stringstream out;
  out << "<Wagon_factory::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Wagon_factory::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::wagon_factory);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Wagon_factory::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::wagon_factory])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as wagon factory type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as wagon factory currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as wagon factory max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building