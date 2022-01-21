#include <memory>
#include <set>
#include <vector>

#include <tiles/Sea.h>

using namespace tile;

Sea::Sea() {}

Sea::~Sea() {}

common::Error Sea::can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                    Direction direction)
{
  common::Error err = common::ERR_NONE;
  // Check that input is valid, that we don't already have a neighbor in
  // that direction, and that we're not already neighbors with the new neighbor.
  if ((nullptr == neighbor) || (!is_valid_direction(direction)))
  {
    err = common::ERR_INVALID;
  }
  else if ((m_p_neighbors[direction]) || (*this == *neighbor))
  {
    err = common::ERR_FAIL;
  }
  else
  {
    for (int d = 0; d < m_max_directions; d++)
    {
      if ((m_p_neighbors[d]) && (*(m_p_neighbors[d]) == *neighbor))
      {
        err = common::ERR_FAIL;
        break;
      }
    }
  }

  return err;
}