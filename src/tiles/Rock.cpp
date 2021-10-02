#include <memory>
#include <vector>
#include <set>

#include <tiles/Rock.h>


using namespace tile;

Rock::Rock()
{
}

Rock::Rock(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Rock::~Rock()
{
}

bool Rock::add_neighbor(std::shared_ptr<Land> neighbor, Direction direction)
{
  return false;
}

bool Rock::add_neighbor(std::shared_ptr<Land> neighbor)
{
  return false;
}