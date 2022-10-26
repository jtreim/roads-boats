#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Mint.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Mint::Mint() : Secondary_producer(false, Type::mint, 1) {}
Mint::Mint(const Mint &other)
    : Secondary_producer(other.m_has_manager, Type::mint,
                         other.m_production_max, other.m_production_current)
{
}
Mint::~Mint() {}

Mint Mint::operator=(const Mint &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Mint::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (max - m_production_current <= 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::fuel) >= 1) &&
          (input.count(portable::Resource::Type::gold) >= 2));
}

common::Error
Mint::produce(portable::Cache &input,
              std::vector<portable::Transporter *> &nearby_transporters,
              std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between fuel, gold, and # of coins that can
  // still be produced this turn
  // 1 coin requires 1 fuel
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)input.count(portable::Resource::Type::fuel),
               (int)(max - m_production_current)));
  // 1 coin requires 2 gold
  to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::gold) / 2),
               (int)to_produce));

  common::Error err = input.remove(portable::Resource::Type::fuel, to_produce);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::gold, to_produce * 2);
  }

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      output.push_back(new portable::Resource(
          portable::Resource::Type::coins));
    }

    m_production_current += to_produce;
  }

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return err;
}

bool Mint::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) >= 1) &&
          (nullptr != tile) &&
          (tile::is_valid(tile->get_terrain())) &&
          (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

common::Error Mint::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::stone);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::boards, 2);
  }
  return err;
}

std::string Mint::to_string() const
{
  std::stringstream out;
  out << "<Mint::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Mint::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::mint);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Mint::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::mint])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Mint type: " + j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Mint currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Mint max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building