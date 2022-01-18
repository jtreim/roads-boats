#include <memory>
#include <set>
#include <vector>

#include <tiles/Mountain.h>

using namespace tile;

Mountain::Mountain() {}

Mountain::Mountain(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Mountain::~Mountain() {}