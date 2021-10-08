#include <memory>
#include <set>
#include <vector>

#include "tiles/Sea.h"

using namespace tile;

Sea::Sea()
{
  for (uint8_t i = 0; i < Tile::m_max_directions; i++)
  {
    m_p_river_points.insert(static_cast<Direction>(i));
  }
}

Sea::Sea(std::set<Direction> p_river_points)
{
  for (uint8_t i = 0; i < Tile::m_max_directions; i++)
  {
    m_p_river_points.insert(static_cast<Direction>(i));
  }
}

Sea::~Sea() {}

bool Sea::add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction)
{
  return false;
}

bool Sea::add_neighbor(std::shared_ptr<Tile> neighbor) { return false; }

bool Sea::is_shore() const { return false; }