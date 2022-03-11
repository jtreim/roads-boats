#ifndef HEX_H
#define HEX_H

#include <sstream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <tiles/components/Border.h>

/// Tiles rely heavily on a cube/axial (q, r, s) coordinate system for
/// hexagons. Each tile will have a coordinate tuple associated with its
/// location in the map. For more information, visit
/// https://www.redblobgames.com/grids/hexagons.

namespace tile
{
class Hex
{
public:
  Hex();
  Hex(const int8_t q, const int8_t r);
  Hex(const Hex &other);
  virtual ~Hex();

  inline int8_t q() { return m_p_q; }
  inline int8_t r() { return m_p_r; }
  inline int8_t s() { return -m_p_q - m_p_r; }

  inline int8_t q() const { return m_p_q; }
  inline int8_t r() const { return m_p_r; }
  inline int8_t s() const { return -m_p_q - m_p_r; }

  Hex operator=(const Hex &other);
  Hex operator+(const Hex &other) const;
  Hex operator-(const Hex &other) const;

  bool operator==(Hex &other);
  bool operator==(const Hex &other) const;
  bool operator!=(const Hex &other) const;
  bool operator!=(Hex &other);
  void operator+=(Hex const &other);
  void operator-=(Hex const &other);
  bool operator<(Hex const &other) const;
  bool operator<(Hex &other);
  bool operator>(Hex const &other) const;
  bool operator>(Hex &other);

  /// Calculates the distance between this hex and another
  /// @param other
  /// @return distance from this hex to the other in axial distance
  int16_t distance(const Hex other) const;

  /// Returns coordinates for moving input amount along q axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline Hex move_q(const int8_t amount) { return Hex(m_p_q, m_p_r - amount); }

  /// Returns coordinates for moving input amount along r axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline Hex move_r(const int8_t amount) { return Hex(m_p_q + amount, m_p_r); }

  /// Returns coordinates for moving input amount along s axis
  /// @param[in] amount
  /// @return The resulting coordinates
  inline Hex move_s(const int8_t amount)
  {
    return Hex(m_p_q - amount, m_p_r + amount);
  }

  /// Returns coordinates for neighbor in the input direction
  /// @param[in] d Direction of neighbor from this tile
  /// @return The neighbor's coordinates. Returns this point on invalid
  /// Direction.
  inline Hex neighbor(const Direction d)
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

    return Hex(q, r);
  }

  /// Returns coordinates for neighbor in the input direction
  /// @param[in] d Direction of neighbor from this tile
  /// @return The neighbor's coordinates. Returns this point on invalid
  /// Direction.
  inline Hex neighbor(const Direction d) const
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

    return Hex(q, r);
  }

  nlohmann::json to_json() const;
  inline std::string to_string() const
  {
    std::stringstream ss;
    ss << "(q=" << q() << ",r=" << r() << ",s=" << s() << ")";
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &os, const Hex &pos);

protected:
  int8_t m_p_q;
  int8_t m_p_r;

private:
};

/// Create a River object using the input json.
/// @param[in] j Input json
/// @param[out] hp Pointer to created Hex object. (0,0,0) on invalid
/// input.
/// @return
///   - common::ERR_NONE on success
///   - common::ERR_FAIL on invalid json object
static common::Error from_json(const nlohmann::json j, Hex &hp);
} // namespace tile

template <> struct std::hash<tile::Hex>
{
  std::size_t operator()(tile::Hex const &hp) const noexcept
  {
    std::size_t hq = std::hash<int8_t>{}(hp.q());
    std::size_t hr = std::hash<int8_t>{}(hp.r());
    return hq ^
           (hr << 1); // or use boost::hash_combine (see Discussion)
                      // https://en.cppreference.com/w/Talk:cpp/utility/hash
  }
};

#endif