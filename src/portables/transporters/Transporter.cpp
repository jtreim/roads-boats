#include <algorithm>
#include <iterator>
#include <vector>

#include <nlohmann/json.hpp>

#include <players/Color.h>
#include <portables/transporters/Transporter.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>


using namespace portable;

Transporter::Transporter()
    : m_type(Type::invalid), m_color(player::invalid), m_tile(nullptr),
      m_entered_border(tile::Border::invalid_border), m_max_capacity(0),
      m_remaining_movement(0), m_max_movement(0), Portable(Object::transporter)
{
}

Transporter::Transporter(const Type type, const player::Color color,
                         tile::Tile *tile, const tile::Border entered_border,
                         const uint8_t max_capacity,
                         const uint8_t remaining_movement)
    : m_type(type), m_color(color), m_tile(tile),
      m_entered_border(entered_border), m_carried_transporter(nullptr),
      m_max_capacity(max_capacity), m_remaining_movement(remaining_movement),
      m_max_movement(0), Portable(Object::transporter)
{
}

Transporter::Transporter(const Transporter &other)
    : m_type(other.m_type), m_color(other.m_color), m_tile(other.m_tile),
      m_entered_border(other.m_entered_border), m_cargo(other.m_cargo),
      m_carried_transporter(other.m_carried_transporter),
      m_max_capacity(other.m_max_capacity),
      m_remaining_movement(other.m_remaining_movement),
      m_max_movement(other.m_max_movement), Portable(other.m_object)
{
}

Transporter::~Transporter() {}

void Transporter::reset()
{
  clean();
  for (size_t i = 0; i < m_cargo.size(); i++)
  {
    m_cargo.at(i)->reset(&m_color);
  }
  m_remaining_movement = m_max_movement;
  Portable::reset(); // Inherited reset. Clears carriers
}

void Transporter::clean()
{
  for (size_t i = m_cargo.size() - 1; i >= 0; i--)
  {
    if ((!Portable::is_valid(m_cargo.at(i)->get_object())) ||
        (!Resource::is_valid(m_cargo.at(i)->get_type())))
    {
      auto it = m_cargo.begin();
      std::advance(it, i);
      m_cargo.erase(it);
    }
  }

  if ((nullptr != m_tile) && (tile::Terrain::invalid == m_tile->get_terrain()))
  {
    delete m_tile;
    m_tile = nullptr;
  }
}

bool Transporter::can_pickup_resource(portable::Resource *res)
{
  return ((nullptr != res) && (portable::Resource::is_valid(res->get_type())) &&
          (res->can_add_carrier(m_color)) &&
          (m_cargo.size() < m_max_capacity) &&
          (nullptr == m_carried_transporter) && (0 == m_carriers.size()));
}

common::Error Transporter::pickup_resource(portable::Resource *&res)
{
  if (!can_pickup_resource(res))
  {
    return common::ERR_FAIL;
  }

  common::Error err = res->pickup(m_color);
  if (!err)
  {
    m_cargo.push_back(res);
  }
  return err;
}

bool Transporter::can_drop_resource(const size_t idx, tile::Border border)
{
  bool valid_tile_drop = false;
  if ((nullptr != m_tile) && (idx >= 0) && (idx < m_cargo.size()))
  {
    // Check that the transporter has access to the area with the input border
    tile::Border area_border =
        (tile::is_valid(border) ? border : m_entered_border);
    auto accessible_areas = m_tile->get_accessible_areas(area_border);
  }
  return ((valid_tile_drop) && (tile::is_valid(m_entered_border)) &&
          (m_cargo.size() > 0));
}

common::Error Transporter::drop_resource(const size_t idx, tile::Border border)
{
  if (!can_drop_resource(idx))
  {
    return common::ERR_FAIL;
  }

  std::shared_ptr<tile::Area> area = m_tile->get_area(m_entered_border);
  if (tile::Border::invalid_border != border)
  {
    area = m_tile->get_area(border);
  }

  common::Error err = area->add_resource(m_cargo.at(idx));
  if (!err)
  {
    auto it = m_cargo.begin();
    std::advance(it, idx);
    m_cargo.erase(it);
  }

  return err;
}

bool Transporter::can_pickup_transporter(const Transporter *other)
{
  if ((nullptr == m_tile) || nullptr == other->m_tile)
  {
    return false;
  }

  // TODO: this may not work correctly with sea transporters on rivers...
  auto other_transporter_area =
      other->m_tile->get_area(other->m_entered_border);
  auto transporter_area = m_tile->get_area(m_entered_border);
  return ((m_cargo.size() == 0) && (nullptr == m_carried_transporter) &&
          (nullptr != other) && (nullptr == other->m_carried_transporter) &&
          (0 == m_carriers.size()) && (other->m_color == m_color) &&
          (other->m_tile == m_tile) &&
          (*other_transporter_area == *transporter_area) &&
          (other->m_cargo.size() == 0) && (other->m_carriers.size() == 0) &&
          (other->m_remaining_movement == 0) &&
          (other->can_add_carrier(m_color)));
}

common::Error Transporter::pickup_transporter(Transporter *other)
{
  if (!can_pickup_transporter(other))
  {
    return common::ERR_FAIL;
  }

  common::Error err = other->add_carrier(m_color);
  if (!err)
  {
    other->end_movement();
    m_carried_transporter = other;
  }

  return err;
}

bool Transporter::can_drop_transporter(const tile::Border border) {}

common::Error Transporter::drop_transporter(const tile::Border border) {}

nlohmann::json Transporter::to_json() const
{
  // TODO: Dump transporter data to json
  nlohmann::json retval;
  return retval;
}