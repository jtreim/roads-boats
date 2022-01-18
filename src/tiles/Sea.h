#ifndef SEA_H
#define SEA_H

#include <memory>
#include <set>

#include <tiles/Tile.h>

namespace tile
{
class Sea : public Tile
{
public:
  Sea();

  ~Sea();

  bool is_shore() const { return false; };

protected:
  /// Checks whether neighbor can be placed at the direction relative to the
  /// tile.
  /// @param[in] neighbor  Tile to be placed
  /// @param[in] direction  Direction neighbor would be to the tile
  /// @return
  ///    - true if a neighbor can be added in the given direction
  ///    - false otherwise
  inline bool can_add_neighbor(std::shared_ptr<Tile> neighbor,
                               Direction direction)
  {
    return ((nullptr != neighbor) && (is_valid_direction(direction)) &&
            (!m_p_neighbors[direction]));
  }

private:
};
} // namespace tile
#endif // end SEA_H