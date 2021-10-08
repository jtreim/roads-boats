#include <memory>
#include <set>
#include <vector>

#include "tiles/Mountain.h"

using namespace tile;

Mountain::Mountain() {}

Mountain::Mountain(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Mountain::~Mountain() {}

bool Mountain::add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction)
{
  return false;
}

bool Mountain::add_neighbor(std::shared_ptr<Tile> neighbor) { return false; }