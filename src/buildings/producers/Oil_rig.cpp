#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Oil_rig.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Oil_rig::Oil_rig() : Primary_producer(false, Type::oil_rig, 1) {}
Oil_rig::Oil_rig(const Oil_rig &other)
    : Primary_producer(other.m_is_powered, Type::oil_rig,
                       other.m_production_max, other.m_production_current)
{
}
Oil_rig::~Oil_rig() {}

Oil_rig Oil_rig::operator=(const Oil_rig &other)
{
  m_production_current = other.m_production_current;
  m_is_powered = other.m_is_powered;
  m_production_max = other.m_production_max;
  return *this;
}

common::Error
Oil_rig::produce(portable::Cache &input,
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
    output.push_back(new portable::Resource(portable::Resource::Type::fuel));
  }
  m_production_current += to_produce;

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return common::ERR_NONE;
}

bool Oil_rig::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 3) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) && (tile::Terrain::sea == tile->get_terrain()));
}

common::Error Oil_rig::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::stone);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::boards, 3);
  }
  return err;
}

std::string Oil_rig::to_string() const
{
  std::stringstream out;
  out << "<Oil_rig::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Oil_rig::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Building::Type::oil_rig);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["is_powered"] = m_is_powered;
  return j;
}

void Oil_rig::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::oil_rig])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Oil_rig type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool is_powered = j.at("is_powered").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Oil_rig currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!is_powered) && (max != 1)) || ((is_powered) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Oil_rig max_production= " << max
        << ", electricity=" << (is_powered ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_is_powered = is_powered;
}

} // namespace building