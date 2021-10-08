#include <memory>
#include <set>
#include <vector>

#include "tiles/Forest.h"

using namespace tile;

Forest::Forest() {}

Forest::Forest(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Forest::~Forest() {}

bool Forest::add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction)
{
  return false;
}

bool Forest::add_neighbor(std::shared_ptr<Tile> neighbor) { return false; }