#include <memory>
#include <set>
#include <vector>

#include <tiles/Plains.h>

using namespace tile;

Plains::Plains() {}

Plains::Plains(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Plains::~Plains() {}