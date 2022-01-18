#include <memory>
#include <set>
#include <vector>

#include <tiles/Rock.h>

using namespace tile;

Rock::Rock() {}

Rock::Rock(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Rock::~Rock() {}