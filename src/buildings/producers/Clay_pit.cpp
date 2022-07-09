#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Clay_pit.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Clay_pit::Clay_pit() : Primary_producer(false, Type::clay_pit, 1) {}
Clay_pit::Clay_pit(const Clay_pit &other)
    : Primary_producer(other.m_is_powered, Type::clay_pit,
                       other.m_production_max, other.m_production_current)
{
}
Clay_pit::~Clay_pit() {}

Clay_pit Clay_pit::operator=(const Clay_pit &other)
{
  m_production_current = other.m_production_current;
  m_is_powered = other.m_is_powered;
  m_production_max = other.m_production_max;
  return *this;
}

common::Error
Clay_pit::produce(portable::Cache &input,
                  std::vector<portable::Transporter *> &nearby_transporters,
                  std::vector<std::unique_ptr<portable::Portable>> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  uint8_t to_produce = m_production_max - m_production_current;

  for (uint8_t i = 0; i < to_produce; i++)
  {
    output.push_back(
        std::make_unique<portable::Resource>(portable::Resource::Type::clay));
  }
  m_production_current += to_produce;

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return common::ERR_NONE;
}

bool Clay_pit::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()) && (tile->is_shore()));
}

std::string Clay_pit::to_string() const
{
  std::stringstream out;
  out << "<Clay_pit::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Clay_pit::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Building::Type::clay_pit);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["is_powered"] = m_is_powered;
  return j;
}

void Clay_pit::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::clay_pit])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Clay_pit type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool is_powered = j.at("is_powered").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Clay_pit currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!is_powered) && (max != 1)) || ((is_powered) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Clay_pit max_production= " << max
        << ", electricity=" << (is_powered ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_is_powered = is_powered;
}

} // namespace building