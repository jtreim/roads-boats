#include <memory>
#include <set>
#include <vector>

#include <tiles/Plain.h>

using namespace tile;

Plain::Plain() {}

Plain::Plain(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Plain::~Plain() {}