#ifndef HEX_POINT_H
#define HEX_POINT_H

#include <nlohmann/json.hpp>

#include <common/Errors.h>

#include <tiles/components/Border.h>

/// Tiles rely heavily on a cube/axial (q, r, s) coordinate system for
/// hexagons. Each tile will have a coordinate tuple associated with its
/// location in the map. For more information, visit
/// https://www.redblobgames.com/grids/hexagons.

namespace tile
{
class hex_point
{
public:
  hex_point();
  hex_point(const int8_t q, const int8_t r);
  hex_point(const hex_point &other);
  hex_point operator=(const hex_point &other) { return hex_point(other); }
  virtual ~hex_point();

  inline int8_t q() { return m_p_q; }
  inline int8_t r() { return m_p_r; }
  inline int8_t s() { return -m_p_q - m_p_r; }

  inline int8_t q() const { return m_p_q; }
  inline int8_t r() const { return m_p_r; }
  inline int8_t s() const { return -m_p_q - m_p_r; }

  hex_point operator+(const hex_point &other) const;
  hex_point operator-(const hex_point &other) const;

  bool operator==(hex_point &other);
  bool operator==(const hex_point &other) const;
  bool operator!=(const hex_point &other) const;
  bool operator!=(hex_point &other);
  void operator+=(hex_point const &other);
  void operator-=(hex_point const &other);

  /// Calculates the distance between this hex and another
  /// @param other
  /// @return distance from this hex to the other in axial distance
  int16_t distance(const hex_point other) const;

  /// Returns coordinates for moving input amount along q axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline hex_point move_q(const int8_t amount)
  {
    return hex_point(m_p_q, m_p_r - amount);
  }

  /// Returns coordinates for moving input amount along r axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline hex_point move_r(const int8_t amount)
  {
    return hex_point(m_p_q + amount, m_p_r);
  }

  /// Returns coordinates for moving input amount along s axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline hex_point move_s(const int8_t amount)
  {
    return hex_point(m_p_q - amount, m_p_r + amount);
  }

  /// Returns coordinates for neighbor in the input direction
  /// @param[in] d Direction of neighbor from this tile
  /// @return The neighbor's coordinates. Returns this point on invalid
  /// Direction.
  inline hex_point neighbor(const Direction d)
  {
    int8_t q = m_p_q;
    int8_t r = m_p_r;
    switch (d)
    {
    case Direction::north_west:
      r -= 1;
      break;
    case Direction::north_east:
      q += 1;
      r -= 1;
      break;
    case Direction::east:
      q += 1;
      break;
    case Direction::south_east:
      r += 1;
      break;
    case Direction::south_west:
      q -= 1;
      r += 1;
      break;
    case Direction::west:
      q -= 1;
      break;
    default:
      break;
    }

    return hex_point(q, r);
  }

  nlohmann::json to_json() const;

protected:
  int8_t m_p_q;
  int8_t m_p_r;

private:
};

/// Create a River object using the input json.
/// @param[in] j Input json
/// @param[out] hp Pointer to created hex_point object. (0,0,0) on invalid
/// input.
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on invalid json object
static common::Error from_json(const nlohmann::json j, hex_point &hp);
} // namespace tile

std::ostream &operator<<(std::ostream &os, const tile::hex_point &pos);
// template <> struct std::hash<hex_point>
// {
//   std::size_t operator()(hex_point const &hp) const noexcept
//   {
//     std::size_t hq = std::hash<int8_t>{}(hp.q());
//     std::size_t hr = std::hash<int8_t>{}(hp.r());
//     return hq ^
//            (hr << 1); // or use boost::hash_combine (see Discussion)
//                       // https://en.cppreference.com/w/Talk:cpp/utility/hash
//   }
// };

#endif