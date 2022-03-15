#include <assert.h>

#include <nlohmann/json.hpp>

#include <tiles/components/Hex.h>

namespace tile
{
Hex::Hex() : m_q(0), m_r(0) {}
Hex::Hex(const int q, const int r) : m_q(q), m_r(r) {}
Hex::Hex(const Hex &other) : m_q(other.m_q), m_r(other.m_r) {}

Hex::~Hex() {}

Hex Hex::operator=(const Hex &other)
{
  m_q = other.m_q;
  m_r = other.m_r;
  return (*this);
}

Hex Hex::operator+(const Hex &other) const
{
  return Hex(m_q + other.m_q, m_r + other.m_r);
}

Hex Hex::operator-(const Hex &other) const
{
  return Hex(m_q - other.m_q, m_r - other.m_r);
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
  m_q += other.m_q;
  m_r += other.m_r;
}

void Hex::operator-=(Hex const &other)
{
  m_q -= other.m_q;
  m_r -= other.m_r;
}

bool Hex::operator<(Hex const &other) const
{
  if (m_q != other.m_q)
  {
    return m_q < other.m_q;
  }
  return m_r < other.m_r;
}
bool Hex::operator<(Hex &other)
{
  if (m_q != other.m_q)
  {
    return m_q < other.m_q;
  }
  return m_r < other.m_r;
}
bool Hex::operator>(Hex const &other) const
{
  if (m_q != other.m_q)
  {
    return m_q > other.m_q;
  }
  return m_r > other.m_r;
}
bool Hex::operator>(Hex &other)
{
  if (m_q != other.m_q)
  {
    return m_q > other.m_q;
  }
  return m_r > other.m_r;
}

int16_t Hex::distance(const Hex other) const
{
  Hex diff(*this - other);
  return (abs((int16_t)diff.m_q) + abs((int16_t)diff.m_r) +
          abs((int16_t)diff.s())) /
         2;
}

std::ostream &operator<<(std::ostream &os, const Hex &pos)
{
  os << "(q:" << static_cast<int>(pos.q())
     << ", r:" << static_cast<int>(pos.r())
     << ", s:" << static_cast<int>(pos.s()) << ")";
  return os;
}

void to_json(nlohmann::json &j, const Hex &hex)
{
  j["q"] = hex.m_q;
  j["r"] = hex.m_r;
}

void from_json(const nlohmann::json &j, Hex &hex)
{
  int q = j.at("q").get<int>();
  int r = j.at("r").get<int>();
  hex = Hex(q, r);
}
} // namespace tile
