#ifndef RIVER_H
#define RIVER_H

#include <memory>
#include <set>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/Transporter.h>
#include <tiles/components/Border.h>

namespace tile
{
class Area;

class River : public std::enable_shared_from_this<River>
{
public:
  River(const std::set<Direction> river_points);
  River(const River &other);
  virtual ~River();

  inline uuids::uuid get_id() { return m_p_id; }

  bool operator==(River const &other) const;
  bool operator==(River &other);
  bool operator!=(River const &other) const;
  bool operator!=(River &other);

  bool inline can_build_bridge(Direction d)
  {
    // TODO: This still allows building another bridge between the same two
    // areas at another point of a river.
    return ((m_p_points.contains(d)) && !(m_p_bridges.contains(d)));
  }

  inline bool has_bridge(const Direction d) const
  {
    return m_p_bridges.contains(d);
  }
  inline std::set<Direction> get_points() { return m_p_points; }
  inline bool has_point(const Direction d) const
  {
    return m_p_points.contains(d);
  }

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
  common::Error build_bridge(const Direction d);

  /// Rotates the river clockwise the number of rotations.
  /// @param[in] rotations The number of clockwise rotations to perform. A
  /// negative value rotates counter-clockwise.
  void rotate(int8_t rotations);

  nlohmann::json to_json() const;

protected:
private:
  uuids::uuid m_p_id;
  std::set<Direction> m_p_points;
  std::set<Direction> m_p_bridges;
  std::vector<std::shared_ptr<portable::Transporter>> m_p_transporters;
};

/// Create a River object using the input json.
/// @param[in] j Input json
/// @param[out] r Pointer to created River object. Null on invalid json input.
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on invalid json object
static common::Error from_json(const nlohmann::json j,
                               std::shared_ptr<River> &r);
} // namespace tile

#endif