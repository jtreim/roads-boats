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

  inline bool is_shore() const { return false; };
  inline bool is_sea() const { return true; }

protected:
  /// Checks whether neighbor can be placed at the direction relative to the
  /// tile.
  /// @param[in] neighbor  Tile to be placed
  /// @param[in] direction  Direction neighbor would be to the tile
  /// @return
  ///   - common::Error::ERR_INVALID if either param is an invalid format
  ///   (null, nonexistant direction).
  ///   - common::Error::ERR_FAIL if there's already a neighbor in the given
  ///   direction, the neighbor's river points doesn't allow being added
  ///   there, or if the new neighbor matches ourselves or a neighbor we
  ///   already have.
  ///   - common::Error::ERR_NONE on valid placement.
  common::Error can_add_neighbor(std::shared_ptr<Tile> neighbor,
                                 Direction direction);

private:
};
} // namespace tile
#endif // end SEA_H