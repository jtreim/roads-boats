#include <memory>
#include <set>
#include <vector>

#include <tiles/Desert.h>

using namespace tile;

Desert::Desert() {}

Desert::Desert(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Desert::~Desert() {}

bool Desert::add_neighbor(std::shared_ptr<Tile> neighbor, Direction direction)
{
  return false;
}

bool Desert::add_neighbor(std::shared_ptr<Tile> neighbor) { return false; }