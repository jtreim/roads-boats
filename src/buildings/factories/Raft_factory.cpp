#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/factories/Raft_factory.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Raft_factory::Raft_factory() : Secondary_producer(false, Type::raft_factory, 1)
{
}
Raft_factory::Raft_factory(const Raft_factory &other)
    : Secondary_producer(other.m_has_manager, Type::raft_factory,
                         other.m_production_max, other.m_production_current)
{
}
Raft_factory::~Raft_factory() {}

Raft_factory Raft_factory::operator=(const Raft_factory &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Raft_factory::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (max - m_production_current <= 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::trunks) >= 2));
}

common::Error
Raft_factory::produce(portable::Cache &input,
                      std::vector<portable::Transporter *> &nearby_transporters,
                      std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and Rafts that can still be
  // produced this turn. 2 trunks makes 1 Raft
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::trunks) / 2),
               (int)(max - m_production_current)));

  common::Error err =
      input.remove(portable::Resource::Type::trunks, to_produce * 2);

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      if (nearby_transporters.size() > 0)
      {
        // TODO produce raft(s) based on adjacent transporters
      }
    }
    m_production_current += to_produce;
  }

  return common::ERR_NONE;
}

bool Raft_factory::can_build(const portable::Cache &input,
                             const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 1) &&
          (input.count(portable::Resource::Type::stone) >= 1) &&
          (nullptr != tile) &&
          (tile::is_valid(tile->get_terrain())) &&
          (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()) &&
          (tile->is_shore()));
}

common::Error Raft_factory::remove_construction_resources(
  portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::boards, 1);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::stone, 1);
  }
  return err;
}

std::string Raft_factory::to_string() const
{
  std::stringstream out;
  out << "<Raft_factory::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Raft_factory::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::raft_factory);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Raft_factory::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::raft_factory])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as raft factory type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as raft factory currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as raft factory max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building