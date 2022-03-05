#include <assert.h>

#include <nlohmann/json.hpp>

#include <tiles/components/Hex_point.h>

using namespace tile;

hex_point::hex_point() : m_p_q(0), m_p_r(0) {}
hex_point::hex_point(const int8_t q, const int8_t r) : m_p_q(q), m_p_r(r) {}
hex_point::hex_point(const hex_point &other)
    : m_p_q(other.m_p_q), m_p_r(other.m_p_r)
{
}

hex_point::~hex_point() {}

hex_point hex_point::operator+(const hex_point &other) const
{
  return hex_point(m_p_q + other.m_p_q, m_p_r + other.m_p_r);
}

hex_point hex_point::operator-(const hex_point &other) const
{
  return hex_point(m_p_q - other.m_p_q, m_p_r - other.m_p_r);
}

bool hex_point::operator==(hex_point &other)
{
  return ((q() == other.q()) && (r() == other.r()) && (s() == other.s()));
}

bool hex_point::operator!=(hex_point &other) { return !(*this == other); }

bool hex_point::operator==(const hex_point &other) const
{
  return ((q() == other.q()) && (r() == other.r()) && (s() == other.s()));
}

bool hex_point::operator!=(const hex_point &other) const
{
  return !(*this == other);
}

void hex_point::operator+=(hex_point const &other)
{
  m_p_q += other.m_p_q;
  m_p_r += other.m_p_r;
}

void hex_point::operator-=(hex_point const &other)
{
  m_p_q -= other.m_p_q;
  m_p_r -= other.m_p_r;
}

int16_t hex_point::distance(const hex_point other) const
{
  hex_point diff(*this - other);
  return (abs((int16_t)diff.m_p_q) + abs((int16_t)diff.m_p_r) +
          abs((int16_t)diff.s())) /
         2;
}

std::ostream &operator<<(std::ostream &os, hex_point &pos)
{
  os << "(q:" << pos.q() << ", r:" << pos.r() << ", s:" << pos.s() << ")";
  return os;
}

nlohmann::json hex_point::to_json() const
{
  nlohmann::json retval;
  retval["q"] = m_p_q;
  retval["r"] = m_p_r;
  retval["s"] = (-m_p_q - m_p_r);
  return retval;
}

// TODO: implement from_json