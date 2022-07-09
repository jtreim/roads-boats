#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Papermill.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Papermill::Papermill() : Secondary_producer(false, Type::papermill, 1) {}
Papermill::Papermill(const Papermill &other)
    : Secondary_producer(other.m_has_manager, Type::papermill,
                         other.m_production_max, other.m_production_current)
{
}
Papermill::~Papermill() {}

Papermill Papermill::operator=(const Papermill &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Papermill::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  if (m_production_max - m_production_current == 0)
  {
    return false;
  }

  return ((input.count(portable::Resource::Type::trunks) >= 2) ||
          (input.count(portable::Resource::Type::boards) >= 2));
}

common::Error
Papermill::produce(portable::Cache &input,
                   std::vector<portable::Transporter *> &nearby_transporters,
                   std::vector<std::unique_ptr<portable::Portable>> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and paper that can still be produced
  // this turn. 1 paper requires 2 trunks or 2 boards
  //
  // boards are used first, then trunks
  uint8_t board_production = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::boards) / 2),
               (int)(m_production_max - m_production_current)));

  common::Error err = common::ERR_NONE;

  if (board_production > 0)
  {
    err = input.remove(portable::Resource::Type::boards, board_production * 2);
    if (!err)
    {
      for (uint8_t i = 0; i < board_production; i++)
      {
        output.push_back(
            std::make_unique<portable::Resource>(portable::Resource::paper));
      }
      m_production_current += board_production;
    }
  }

  // If we can still produce with available trunks, use those next
  uint8_t trunk_production = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::trunks) / 2),
               (int)(m_production_max - m_production_current)));

  if ((!err) && (trunk_production > 0))
  {
    err = input.remove(portable::Resource::Type::trunks, trunk_production * 2);
    if (!err)
    {
      for (uint8_t i = 0; i < trunk_production; i++)
      {
        output.push_back(
            std::make_unique<portable::Resource>(portable::Resource::paper));
      }
      m_production_current += trunk_production;
    }
  }

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return err;
}

bool Papermill::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) > 0) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

common::Error Papermill::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::boards);
  if (!err)
  {
    input.remove(portable::Resource::Type::stone);
  }
  return err;
}

std::string Papermill::to_string() const
{
  std::stringstream out;
  out << "<Papermill::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Papermill::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::papermill);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Papermill::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::papermill])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Papermill type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Papermill currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 1)) || ((has_manager) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Papermill max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building