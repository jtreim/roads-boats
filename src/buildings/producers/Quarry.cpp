#include <memory>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Quarry.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Quarry::Quarry() : Primary_producer(false, Type::quarry, 1) {}
Quarry::Quarry(const Quarry &other)
    : Primary_producer(other.m_is_powered, Type::quarry, other.m_production_max,
                       other.m_production_current)
{
}
Quarry::~Quarry() {}

Quarry Quarry::operator=(const Quarry &other)
{
  m_production_current = other.m_production_current;
  m_is_powered = other.m_is_powered;
  m_production_max = other.m_production_max;
  return *this;
}

common::Error
Quarry::produce(portable::Cache &input,
                std::vector<portable::Transporter *> &nearby_transporters,
                std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  uint8_t max = (m_is_powered ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = max - m_production_current;
  for (uint8_t i = 0; i < to_produce; i++)
  {
    output.push_back(new portable::Resource(portable::Resource::Type::stone));
  }
  m_production_current += to_produce;

  // TODO Ask nearby transporters (in turn order) whether they'd like the
  // resources to be added to their load.
  return common::ERR_NONE;
}

bool Quarry::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (nullptr != tile) && (tile::Terrain::rock == tile->get_terrain()));
}

common::Error Quarry::remove_construction_resources(portable::Cache &input)
{
  return input.remove(portable::Resource::Type::boards, 2);
}

std::string Quarry::to_string() const
{
  std::stringstream out;
  out << "<Quarry::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Quarry::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Building::Type::quarry);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["is_powered"] = m_is_powered;
  return j;
}

void Quarry::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::quarry])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Quarry type: " + j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool is_powered = j.at("is_powered").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Quarry currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!is_powered) && (max != 1)) || ((is_powered) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Quarry max_production= " << max
        << ", electricity=" << (is_powered ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_is_powered = is_powered;
}

} // namespace building