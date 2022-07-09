#include <algorithm>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Stone_factory.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Stone_factory::Stone_factory()
    : Secondary_producer(false, Type::stone_factory, 6)
{
}
Stone_factory::Stone_factory(const Stone_factory &other)
    : Secondary_producer(other.m_has_manager, Type::stone_factory,
                         other.m_production_max, other.m_production_current)
{
}
Stone_factory::~Stone_factory() {}

Stone_factory Stone_factory::operator=(const Stone_factory &other)
{
  m_production_current = other.m_production_current;
  m_has_manager = other.m_has_manager;
  m_production_max = other.m_production_max;
  return *this;
}

bool Stone_factory::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  if (m_production_max - m_production_current == 0)
  {
    return false;
  }

  return (input.count(portable::Resource::Type::clay) >= 1);
}

common::Error Stone_factory::produce(
    portable::Cache &input,
    std::vector<portable::Transporter *> &nearby_transporters,
    std::vector<std::unique_ptr<portable::Portable>> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  // Determine minimum factor between input and stone that can still be
  // produced this turn. 1 clay makes 2 stone
  uint8_t to_produce = static_cast<uint8_t>(
      std::min((int)(input.count(portable::Resource::Type::clay) * 2),
               (int)(m_production_max - m_production_current)));

  common::Error err =
      input.remove(portable::Resource::Type::clay, to_produce / 2);

  if (!err)
  {
    for (uint8_t i = 0; i < to_produce; i++)
    {
      output.push_back(std::make_unique<portable::Resource>(
          portable::Resource::Type::stone));
    }
    m_production_current += to_produce;
  }

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return err;
}

bool Stone_factory::can_build(const portable::Cache &input,
                              const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 2) &&
          (nullptr != tile) && (tile::Terrain::sea != tile->get_terrain()) &&
          (tile::Terrain::desert != tile->get_terrain()));
}

common::Error
Stone_factory::remove_construction_resources(portable::Cache &input)
{
  return input.remove(portable::Resource::Type::boards, 2);
}

std::string Stone_factory::to_string() const
{
  std::stringstream out;
  out << "<Stone_factory::max=" << m_production_max
      << ", current=" << m_production_current << ">";
  return out.str();
}

nlohmann::json Stone_factory::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Type::stone_factory);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["has_manager"] = m_has_manager;
  return j;
}

void Stone_factory::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::stone_factory])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Stone_factory type: " +
            j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool has_manager = j.at("has_manager").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Stone_factory currently produced: "
        << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!has_manager) && (max != 6)) || ((has_manager) && (max != 12)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Stone_factory max_production= " << max
        << ", has_manager=" << (has_manager ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_has_manager = has_manager;
}

} // namespace building