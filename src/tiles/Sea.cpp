#include <memory>
#include <set>
#include <vector>

#include <tiles/Sea.h>

using namespace tile;

Sea::Sea()
{
  for (uint8_t i = 0; i < Tile::m_max_directions; i++)
  {
    m_p_river_points.insert(static_cast<Direction>(i));
  }
}

Sea::~Sea() {}