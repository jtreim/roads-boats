#include <algorithm>
#include <iterator>
#include <vector>

#include <nlohmann/json.hpp>

#include <players/Color.h>
#include <portables/transporters/sea/Sea_transporter.h>
#include <tiles/Tile.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>


using namespace portable;

Sea_transporter::Sea_transporter() : m_just_docked(false), Transporter() {}

Sea_transporter::Sea_transporter(const Type type, const player::Color color,
                                 tile::Tile *tile,
                                 const tile::Border entered_border,
                                 const uint8_t max_capacity,
                                 const uint8_t remaining_movement,
                                 const bool just_docked)
    : m_just_docked(just_docked), Transporter(type, color, tile, entered_border,
                                              max_capacity, remaining_movement)
{
  if (!can_travel_on_sea(m_type))
  {
    // Mark transporter as unusable
    m_type = Type::invalid;
    m_color = player::Color::invalid;
    m_max_capacity = 0;
    m_remaining_movement = 0;
    m_max_movement = 0;
    m_tile = nullptr;
  }
}

Sea_transporter::Sea_transporter(const Sea_transporter &other)
    : m_just_docked(other.m_just_docked),
      Transporter(other.m_type, other.m_color, other.m_tile,
                  other.m_entered_border, other.m_max_capacity,
                  other.m_remaining_movement)
{
  if (!can_travel_on_sea(m_type))
  {
    // Mark transporter as unusable
    m_type = Type::invalid;
    m_color = player::Color::invalid;
    m_max_capacity = 0;
    m_remaining_movement = 0;
    m_max_movement = 0;
    m_tile = nullptr;
  }
}

void Sea_transporter::reset()
{
  // Even if we are starting the round docked, this means it can dock again
  // this round.
  m_just_docked = false;
}

common::Error Sea_transporter::drop_resource(const size_t idx,
                                             tile::Border border)
{
}