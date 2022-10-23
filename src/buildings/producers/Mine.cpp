#include <algorithm>
#include <random>
#include <sstream>

#include <nlohmann/json.hpp>

#include <buildings/producers/Mine.h>
#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>

namespace building
{
Mine::Mine() : Primary_producer(false, Building::Type::mine, 1)
{
  add_to_mine(3, 3);
}

Mine::Mine(const Mine_type type)
    : Primary_producer(false, Building::Type::mine, 1)
{
  switch (type)
  {
    case Mine_type::big:
      add_to_mine(5, 5);
      break;
    case Mine_type::specialized_gold:
      add_to_mine(4, 0);
      break;
    case Mine_type::specialized_iron:
      add_to_mine(0, 4);
      break;
    default:
      add_to_mine(3, 3);
      break;
  }
}
Mine::Mine(const Mine &other)
    : Primary_producer(other.m_is_powered, Building::Type::mine,
                       other.m_production_max, other.m_production_current),
      m_remaining_resources(other.m_remaining_resources)
{
}
Mine::~Mine() {}

Mine Mine::operator=(const Mine &other)
{
  m_production_current = other.m_production_current;
  m_is_powered = other.m_is_powered;
  m_production_max = other.m_production_max;
  m_remaining_resources = other.m_remaining_resources;
  return *this;
}

bool Mine::can_produce(
    const portable::Cache &input,
    const std::vector<portable::Transporter *> nearby_transporters)
{
  uint8_t max = (m_is_powered ? m_production_max * 2 : m_production_max);
  return ((m_remaining_resources.size() > 0) &&
          (max > m_production_current));
}

common::Error
Mine::produce(portable::Cache &input,
              std::vector<portable::Transporter *> &nearby_transporters,
              std::vector<portable::Portable *> &output)
{
  if (!can_produce(input, nearby_transporters))
  {
    return common::ERR_FAIL;
  }

  uint8_t max = (m_is_powered ? m_production_max * 2 : m_production_max);
  uint8_t to_produce = max - m_production_current;
  to_produce = static_cast<uint8_t>(
      std::min((int)to_produce, (int)m_remaining_resources.size()));

  for (uint8_t i = 0; i < to_produce; i++)
  {
    portable::Resource::Type next = m_remaining_resources.back();
    output.push_back(new portable::Resource(next));
    m_remaining_resources.pop_back();
  }
  m_production_current += to_produce;

  // TODO Ask each nearby transporter (in turn order) whether they'd like the
  // resources to be added to their load.
  return common::ERR_NONE;
}

bool Mine::can_build(const portable::Cache &input, const tile::Tile *tile)
{
  return ((input.count(portable::Resource::Type::boards) >= 3) &&
          (input.count(portable::Resource::Type::stone) > 0) &&
          (nullptr != tile) &&
          (tile::Terrain::mountain == tile->get_terrain()));
}

common::Error Mine::remove_construction_resources(portable::Cache &input)
{
  common::Error err = input.remove(portable::Resource::Type::stone);
  if (!err)
  {
    err = input.remove(portable::Resource::Type::boards, 3);
  }
  return err;
}

uint8_t Mine::count(const portable::Resource::Type t) const
{
  uint8_t total = 0;
  for (auto res : m_remaining_resources)
  {
    if (t == res)
    {
      total++;
    }
  }
  return total;
}

bool Mine::can_build_shaft(const portable::Cache &input) const
{
  return ((m_remaining_resources.size() == 0) &&
          (input.count(portable::Resource::Type::iron) > 0) &&
          (input.count(portable::Resource::Type::fuel) > 0));
}

common::Error Mine::build_shaft(portable::Cache &input, const Mine_type m)
{
  common::Error err = common::ERR_NONE;
  if (!can_build_shaft(input))
  {
    err = common::ERR_FAIL;
  }
  else
  {
    switch (m)
    {
    case Mine_type::regular: // A regular mine has 3 gold & 3 iron
      add_to_mine(3, 3);
      break;
    case Mine_type::specialized_gold: // specialized mines produce 4 of either
                                      // gold or iron.
      add_to_mine(4, 0);
      break;
    case Mine_type::specialized_iron:
      add_to_mine(0, 4);
      break;
    case Mine_type::big: // big mines produce 5 gold & 5 iron
      add_to_mine(5, 5);
      break;
    default:
      err = common::ERR_INVALID;
      break;
    }
  }

  return err;
}

std::string Mine::to_string() const
{
  std::stringstream out;
  out << "<Mine::max=" << m_production_max
      << ", current=" << m_production_current
      << ", gold left=" << count(portable::Resource::Type::gold)
      << ", iron left=" << count(portable::Resource::Type::iron) << ">";
  return out.str();
}

nlohmann::json Mine::to_json() const
{
  nlohmann::json j;
  j["type"] = building::to_string(Building::Type::mine);
  j["production_current"] = m_production_current;
  j["production_max"] = m_production_max;
  j["is_powered"] = m_is_powered;
  j["gold"] = count(portable::Resource::Type::gold);
  j["iron"] = count(portable::Resource::Type::iron);
  return j;
}

void Mine::from_json(nlohmann::json const &j)
{
  if (j.at("type").get<std::string>() != BUILDING_NAMES[Type::mine])
  {
    throw nlohmann::json::type_error::create(
        501,
        "Invalid type given as Mine type: " + j.at("type").get<std::string>(),
        j);
  }
  uint8_t current = j.at("production_current").get<uint8_t>();
  uint8_t max = j.at("production_max").get<uint8_t>();
  bool is_powered = j.at("is_powered").get<bool>();
  if (current > max)
  {
    std::stringstream msg;
    msg << "Invalid amount given as Mine currently produced: " << current;
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  if (((!is_powered) && (max != 1)) || ((is_powered) && (max != 2)))
  {
    std::stringstream msg;
    msg << "Invalid amount given as Mine max_production= " << max
        << ", electricity=" << (is_powered ? "true" : "false");
    throw nlohmann::json::type_error::create(501, msg.str(), j);
  }
  m_production_current = current;
  m_production_max = max;
  m_is_powered = is_powered;

  uint8_t gold = j.at("gold").get<uint8_t>();
  uint8_t iron = j.at("iron").get<uint8_t>();

  for (uint8_t i = 0; i < gold; i++)
  {
    m_remaining_resources.push_back(portable::Resource::Type::gold);
  }
  for (uint8_t i = 0; i < iron; i++)
  {
    m_remaining_resources.push_back(portable::Resource::Type::iron);
  }

  if ((gold > 0) && (iron > 0))
  {
    std::random_device rd;
    std::default_random_engine rng(rd());

    std::shuffle(m_remaining_resources.begin(), m_remaining_resources.end(),
                 rng);
  }
}

void Mine::add_to_mine(uint8_t gold_amount, uint8_t iron_amount)
{
  for (uint8_t i = 0; i < gold_amount; i++)
  {
    m_remaining_resources.push_back(portable::Resource::Type::gold);
  }
  for (uint8_t i = 0; i < iron_amount; i++)
  {
    m_remaining_resources.push_back(portable::Resource::Type::iron);
  }

  if ((gold_amount > 0) && (iron_amount > 0))
  {
    std::random_device rd;
    std::default_random_engine rng(rd());

    std::shuffle(m_remaining_resources.begin(), m_remaining_resources.end(),
                 rng);
  }
}

} // namespace building