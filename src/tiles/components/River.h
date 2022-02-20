#ifndef RIVER_H
#define RIVER_H

#include <memory>
#include <set>

#include <nlohmann/json.hpp>
#include <stduuid/include/uuid.h>

#include <portables/Transporter.h>
#include <tiles/components/Area.h>
#include <tiles/components/Border.h>

namespace tile
{
class Area;

class River : public std::enable_shared_from_this<River>
{
public:
  River(const std::set<Direction> river_points,
        const std::set<std::shared_ptr<Area>> areas);
  River(const River &other);
  virtual ~River();

  River operator=(const River &other) { return River(other); }
  bool operator==(River const &other) const;
  bool operator==(River &other);
  bool operator!=(River const &other) const;
  bool operator!=(River &other);

  /// Retrieves all areas a water transporter has access to from this river.
  /// This is any adjacent areas, and any areas accessible via bridges.
  /// @return All accessible areas.
  std::vector<std::shared_ptr<Area>> get_accessible_areas();

  bool inline can_build_bridge(Direction d)
  {
    // TODO: This still allows building another bridge between the same two
    // areas at the other end of a river.
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
  std::set<std::shared_ptr<Area>> m_p_areas;
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