#include <assert.h>

#include <nlohmann/json.hpp>

#include <tiles/Hex.h>

using namespace tile;

hex_point::hex_point() : m_q(0), m_r(0) {}
hex_point::hex_point(const int8_t q, const int8_t r, const int8_t s)
    : m_q(q), m_r(r)
{
  assert(q + r + s == 0);
}
hex_point::hex_point(const int8_t q, const int8_t r) : m_q(q), m_r(r) {}
hex_point::hex_point(const hex_point &other) : m_q(other.m_q), m_r(other.m_r) {}

hex_point::~hex_point() {}

hex_point hex_point::operator=(const hex_point &other)
{
  return hex_point(other);
}

hex_point hex_point::operator+(const hex_point &other) const
{
  return hex_point(m_q + other.m_q, m_r + other.m_r);
}

hex_point hex_point::operator-(const hex_point &other) const
{
  return hex_point(m_q - other.m_q, m_r - other.m_r);
}

bool hex_point::operator==(hex_point &other)
{
  return ((q() == other.q()) && (r() == other.r()) && (s() == other.s()));
}

bool hex_point::operator!=(hex_point &other) { return !(*this == other); }

void hex_point::operator+=(hex_point const &other)
{
  m_q += other.m_q;
  m_r += other.m_r;
}

void hex_point::operator-=(hex_point const &other)
{
  m_q -= other.m_q;
  m_r -= other.m_r;
}

int16_t hex_point::hex_distance(const hex_point other) const
{
  hex_point diff(*this - other);
  return (abs((int16_t)diff.m_q) + abs((int16_t)diff.m_r) +
          abs((int16_t)diff.s())) /
         2;
}

std::ostream &operator<<(std::ostream &os, hex_point &pos)
{
  os << "q:" << pos.q() << ", r:" << pos.r() << ", s:" << pos.s();
  return os;
}

nlohmann::json hex_point::to_json()
{
  nlohmann::json retval;
  retval["q"] = q();
  retval["r"] = r();
  retval["s"] = s();
  return retval;
}