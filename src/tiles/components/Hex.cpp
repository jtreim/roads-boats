#include <assert.h>

#include <nlohmann/json.hpp>

#include <tiles/components/Hex.h>

namespace tile
{
Hex::Hex() : m_p_q(0), m_p_r(0) {}
Hex::Hex(const int q, const int r) : m_p_q(q), m_p_r(r) {}
Hex::Hex(const Hex &other) : m_p_q(other.m_p_q), m_p_r(other.m_p_r) {}

Hex::~Hex() {}

Hex Hex::operator=(const Hex &other)
{
  m_p_q = other.m_p_q;
  m_p_r = other.m_p_r;
  return (*this);
}

Hex Hex::operator+(const Hex &other) const
{
  return Hex(m_p_q + other.m_p_q, m_p_r + other.m_p_r);
}

Hex Hex::operator-(const Hex &other) const
{
  return Hex(m_p_q - other.m_p_q, m_p_r - other.m_p_r);
}

bool Hex::operator==(Hex &other)
{
  return ((q() == other.q()) && (r() == other.r()) && (s() == other.s()));
}

bool Hex::operator!=(Hex &other) { return !(*this == other); }

bool Hex::operator==(const Hex &other) const
{
  return ((q() == other.q()) && (r() == other.r()) && (s() == other.s()));
}

bool Hex::operator!=(const Hex &other) const { return !(*this == other); }

void Hex::operator+=(Hex const &other)
{
  m_p_q += other.m_p_q;
  m_p_r += other.m_p_r;
}

void Hex::operator-=(Hex const &other)
{
  m_p_q -= other.m_p_q;
  m_p_r -= other.m_p_r;
}

bool Hex::operator<(Hex const &other) const
{
  if (m_p_q != other.m_p_q)
  {
    return m_p_q < other.m_p_q;
  }
  return m_p_r < other.m_p_r;
}
bool Hex::operator<(Hex &other)
{
  if (m_p_q != other.m_p_q)
  {
    return m_p_q < other.m_p_q;
  }
  return m_p_r < other.m_p_r;
}
bool Hex::operator>(Hex const &other) const
{
  if (m_p_q != other.m_p_q)
  {
    return m_p_q > other.m_p_q;
  }
  return m_p_r > other.m_p_r;
}
bool Hex::operator>(Hex &other)
{
  if (m_p_q != other.m_p_q)
  {
    return m_p_q > other.m_p_q;
  }
  return m_p_r > other.m_p_r;
}

int16_t Hex::distance(const Hex other) const
{
  Hex diff(*this - other);
  return (abs((int16_t)diff.m_p_q) + abs((int16_t)diff.m_p_r) +
          abs((int16_t)diff.s())) /
         2;
}

nlohmann::json Hex::to_json() const
{
  nlohmann::json retval;
  retval["q"] = m_p_q;
  retval["r"] = m_p_r;
  retval["s"] = (-m_p_q - m_p_r);
  return retval;
}

std::ostream &operator<<(std::ostream &os, const tile::Hex &pos)
{
  os << "(q:" << static_cast<int>(pos.q())
     << ", r:" << static_cast<int>(pos.r())
     << ", s:" << static_cast<int>(pos.s()) << ")";
  return os;
}
} // namespace tile

// TODO: implement from_json