#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Stock_exchange.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Stock_exchange::Stock_exchange()
    : Secondary_producer(false, Type::stock_exchange, 6)
{
}
Stock_exchange::Stock_exchange(const Stock_exchange &other)
    : Secondary_producer(other.m_has_manager, Type::stock_exchange,
                         other.m_production_max, other.m_production_current)
{
}
Stock_exchange::~Stock_exchange() {}

Stock_exchange Stock_exchange::operator=(const Stock_exchange &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Stock_exchange::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  if (m_production_max - m_production_current <= 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::paper) >= 1) &&
          (input.count(portable::Resource::Type::coins) >= 2));
}

common::Error Stock_exchange::produce(
    portable::Cache &input,
    std::vector<portable::Transporter *> &nearby_transporters,
    std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between paper, coins, and # of stocks that can
  // still be produced this turn
  // 1 stock requires 1 paper
  uint8_t max = (m_has_manager ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)input.count(portable::Resource::Type::paper),
               (int)(max - m_production_current)));
  // 1 stock requires 2 coins
  to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::coins) / 2),
               (int)to_produce));

  common::Error err =
      input.remove(portable::Resource::Type::coins, 2 * to_produce);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::paper, to_produce);
  }

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      output.push_back(new portable::Resource(
          portable::Resource::Type::stock));
    }
    m_production_current += to_produce;
  }

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return err;
}

bool Stock_exchange::can_build(const portable::Cache &input,
                               const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::stone) >= 3) &&
          (nullptr != tile) &&
          (tile::is_valid(tile->get_terrain())) &&
          (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

common::Error
Stock_exchange::remove_construction_resources(portable::Cache &input)
{
  return input.remove(portable::Resource::Type::stone, 3);
}

std::string Stock_exchange::to_string() const
{
  std::stringstream out;
  out << "<Stock_exchange::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Stock_exchange::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::stock_exchange);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Stock_exchange::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::stock_exchange])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Stock_exchange type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Stock_exchange currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Stock_exchange max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building