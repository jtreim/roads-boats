#ifndef HEX_H
#define HEX_H

#include <nlohmann/json.hpp>

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
  hex_point(const int8_t q, const int8_t r, const int8_t s);
  hex_point(const int8_t q, const int8_t r);
  hex_point(const hex_point &other);
  virtual ~hex_point();

  inline int8_t q() { return m_q; }
  inline int8_t r() { return m_r; }
  inline int8_t s() { return -m_q - m_r; }

  hex_point operator=(const hex_point &other);
  hex_point operator+(const hex_point &other) const;
  hex_point operator-(const hex_point &other) const;

  bool operator==(hex_point &other);
  bool operator!=(hex_point &other);
  void operator+=(hex_point const &other);
  void operator-=(hex_point const &other);

  /// Calculates the distance between this hex and another
  /// @param other
  /// @return distance from this hex to the other in axial distance
  int16_t hex_distance(const hex_point other) const;

  friend std::ostream &operator<<(std::ostream &os, hex_point &pos);
  nlohmann::json to_json();

protected:
  int8_t m_q;
  int8_t m_r;

private:
};
} // namespace tile

#endif