#ifndef RIVER_H
#define RIVER_H

#include <memory>
#include <set>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/transporters/Transporter.h>
#include <tiles/components/Border.h>

namespace tile
{

class River : public std::enable_shared_from_this<River>
{
public:
  River();
  River(const std::set<Direction> river_points);
  River(const River &other);
  virtual ~River();

  // Clears river of bridges
  void reset();

  River operator=(River const &other);
  River operator=(River &other);
  bool operator==(River const &other) const;
  bool operator==(River &other);
  bool operator!=(River const &other) const;
  bool operator!=(River &other);

  bool inline can_build_bridge(Direction d)
  {
    return ((m_points.contains(d)) &&
            (m_bridges.size() < m_points.size() - 1) &&
            (!m_bridges.contains(d)));
  }

  inline bool has_point(const Direction d) const
  {
    return m_points.contains(d);
  }
  inline bool has_bridge(const Direction d) const
  {
    return m_bridges.contains(d);
  }
  inline std::set<Direction> get_points() { return m_points; }
  inline std::set<Direction> get_bridges() { return m_bridges; }
  inline std::set<Direction> get_points() const { return m_points; }
  inline std::set<Direction> get_bridges() const { return m_bridges; }

  bool splits_borders(const std::set<Border> borders) const;

  /// Returns a list of area borders this river creates from a otherwise empty
  /// tile.
  /// @return The list of area borders.
  std::vector<std::set<Border>>
  get_area_borders(std::set<Border> borders = ALL_BORDERS) const;

  /// Builds a bridge over the point at the input direction.
  /// @param[in] d
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL when unable to build bridge at input direction.
  ///   - common::ERR_INVALID on invalid direction.
  common::Error build(const Direction d);

  /// Check to see if river can rotate
  /// @return false if river has any bridges or transporters; true otherwise
  bool can_rotate() const;

  /// Rotates the river clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise.
  /// @return
  ///   - common::ERR_NONE on success
  ///   - common::ERR_FAIL on failed can_rotate check
  ///   - common::ERR_UNKNOWN otherwise
  common::Error rotate(int rotations);

  friend std::ostream &operator<<(std::ostream &os, tile::River const &river);
  friend void to_json(nlohmann::json &j, const River &river);
  friend void from_json(const nlohmann::json &j, River &river);

protected:
private:
  inline void set_points(const std::set<Direction> points)
  {
    m_points = points;
  }
  inline void set_bridges(const std::set<Direction> bridges)
  {
    m_bridges = bridges;
  }

  std::set<Direction> m_points;
  std::set<Direction> m_bridges;
};
} // namespace tile

#endif