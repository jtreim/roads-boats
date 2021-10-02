#include <memory>
#include <vector>
#include <set>

#include <tiles/Plain.h>


using namespace tile;

Plain::Plain()
{
}

Plain::Plain(std::set<Direction> p_river_points)
{
  m_p_river_points = p_river_points;
}

Plain::~Plain()
{
}

bool Plain::add_neighbor(std::shared_ptr<Land> neighbor, Direction direction)
{
  return false;
}

bool Plain::add_neighbor(std::shared_ptr<Land> neighbor)
{
  return false;
}