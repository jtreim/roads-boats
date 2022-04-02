#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Player.h>
#include <portables/Resource.h>
#include <portables/Resource_cache.h>

namespace portable
{

Resource_cache::Resource_cache() { clear(); }

Resource_cache::Resource_cache(const Resource_cache &other)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      m_resources[c][r] = other.m_resources[c][r];
    }
  }
}

Resource_cache::~Resource_cache() { clear(); }

bool Resource_cache::operator==(Resource_cache &other)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      if (m_resources[c][r] != other.m_resources[c][r])
      {
        return false;
      }
    }
  }

  return true;
}

bool Resource_cache::operator==(Resource_cache const &other) const
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      if (m_resources[c][r] != other.m_resources[c][r])
      {
        return false;
      }
    }
  }

  return true;
}

bool Resource_cache::operator!=(Resource_cache &other)
{
  return !(*this == other);
}
bool Resource_cache::operator!=(Resource_cache const &other) const
{
  return !(*this == other);
}

Resource_cache Resource_cache::operator=(const Resource_cache &other)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      m_resources[c][r] = other.m_resources[c][r];
    }
  }
  return (*this);
}

Resource_cache Resource_cache::operator+(const Resource_cache &other)
{
  Resource_cache merged = Resource_cache();
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      merged.m_resources[c][r] = m_resources[c][r] + other.m_resources[c][r];
    }
  }
  return merged;
}

Resource_cache Resource_cache::operator+(const Resource_cache &other) const
{
  Resource_cache merged = Resource_cache();
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      merged.m_resources[c][r] = m_resources[c][r] + other.m_resources[c][r];
    }
  }
  return merged;
}

Resource_cache Resource_cache::
operator+(const std::map<Resource, uint16_t> &res_list)
{
  Resource_cache merged = Resource_cache(*this);
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource key = static_cast<Resource>(r);
    if (res_list.contains(key))
    {
      merged.m_resources[player::Color::neutral][r] += res_list.at(key);
    }
  }
  return merged;
}

Resource_cache Resource_cache::
operator+(const std::map<Resource, uint16_t> &res_list) const
{
  Resource_cache merged = Resource_cache(*this);
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource key = static_cast<Resource>(r);
    if (res_list.contains(key))
    {
      merged.m_resources[player::Color::neutral][r] += res_list.at(key);
    }
  }
  return merged;
}

void Resource_cache::operator+=(Resource_cache const &other)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      m_resources[c][r] += other.m_resources[c][r];
    }
  }
}

void Resource_cache::operator+=(std::map<Resource, uint16_t> const &res_list)
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource key = static_cast<Resource>(r);
    if (res_list.contains(key))
    {
      m_resources[player::Color::neutral][r] += res_list.at(key);
    }
  }
}

void Resource_cache::clear()
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      m_resources[c][r] = 0;
    }
  }
}

void Resource_cache::reset()
{
  for (uint8_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      m_resources[player::Color::neutral][r] += m_resources[c][r];
      m_resources[c][r] = 0;
    }
  }
}

uint16_t Resource_cache::get(const Resource res) const
{
  uint16_t total = 0;
  if (is_valid(res))
  {
    for (uint8_t c = 0; c < player::MAX_COLORS; c++)
    {
      total += m_resources[c][res];
    }
  }
  return total;
}

uint16_t Resource_cache::get_moveable(const Resource res,
                                      const player::Color player) const
{
  uint16_t moveable = 0;
  if ((is_valid(res)) && (player::is_valid(player)))
  {
    for (uint8_t c = 0; c < player::MAX_COLORS; c++)
    {
      if (player == c)
      {
        continue;
      }
      moveable += m_resources[c][res];
    }
  }
  return moveable;
}

std::map<Resource, uint16_t> Resource_cache::all() const
{
  std::map<Resource, uint16_t> all_res;
  for (size_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource key = static_cast<Resource>(r);
    all_res.insert({key, get(key)});
  }
  return all_res;
}

std::map<Resource, uint16_t>
Resource_cache::all_moveable(const player::Color p) const
{
  std::map<Resource, uint16_t> all_res;
  for (size_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource key = static_cast<Resource>(r);
    all_res.insert({key, get_moveable(key, p)});
  }
  return all_res;
}

common::Error Resource_cache::add(const Resource res,
                                  const player::Color player,
                                  const uint16_t amount)
{
  if ((!is_valid(res)) || (!player::is_valid(player)))
  {
    return common::ERR_INVALID;
  }

  m_resources[player][res] += amount;
  return common::ERR_NONE;
}

common::Error Resource_cache::remove(const Resource res, const uint16_t amount)
{
  if (!is_valid(res))
  {
    return common::ERR_INVALID;
  }
  if (get(res) < amount)
  {
    return common::ERR_FAIL;
  }

  // Try using just the unmoved resources first
  if (m_resources[player::Color::neutral][res] >= amount)
  {
    m_resources[player::Color::neutral][res] -= amount;
  }
  else
  {
    uint16_t remaining = amount;
    remaining -= m_resources[player::Color::neutral][res];
    m_resources[player::Color::neutral][res] = 0;
    // Unable to remove it using the unmoved resources. Work through each other
    // portion until enough has been removed.
    uint8_t current_stash = 0;
    while ((current_stash < player::MAX_COLORS) && (0 < remaining))
    {
      if (m_resources[current_stash][res] >= remaining)
      {
        m_resources[current_stash][res] -= remaining;
        remaining = 0;
        break;
      }
      remaining -= m_resources[current_stash][res];
      m_resources[current_stash][res] = 0;
      current_stash += 1;
    }

    if (remaining > 0)
    {
      // We somehow didn't have enough to remove, despite checking...
      // We should never get here.
      return common::ERR_UNKNOWN;
    }
  }

  return common::ERR_NONE;
}

common::Error Resource_cache::move(const Resource res,
                                   const player::Color player,
                                   const uint16_t amount)
{
  if ((!is_valid(res)) || (!player::is_valid(player)))
  {
    return common::ERR_INVALID;
  }
  if (get_moveable(res, player) < amount)
  {
    return common::ERR_FAIL;
  }

  // Try using just the unmoved resources first
  if (m_resources[player::Color::neutral][res] >= amount)
  {
    m_resources[player::Color::neutral][res] -= amount;
  }
  else
  {
    // Unable to remove it using the unmoved resources. Work through each other
    // portion until enough has been removed.
    uint8_t current_stash = 0;
    uint16_t remaining = amount;
    while ((current_stash < player::MAX_PLAYER_COLORS) && (0 < remaining))
    {
      if (current_stash == player)
      {
        current_stash += 1;
        continue;
      }

      if (m_resources[current_stash][res] >= remaining)
      {
        m_resources[current_stash][res] -= remaining;
        remaining = 0;
        break;
      }

      remaining -= m_resources[current_stash][res];
      m_resources[current_stash][res] = 0;
      current_stash += 1;
    }

    if (remaining > 0)
    {
      // We somehow didn't have enough to move, despite checking...
      // We should never get here.
      return common::ERR_UNKNOWN;
    }
  }

  return common::ERR_NONE;
}

uint32_t Resource_cache::size() const
{
  uint32_t total = 0;
  for (uint8_t c = 0; c < player::MAX_PLAYER_COLORS; c++)
  {
    for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      total += static_cast<uint32_t>(m_resources[c][r]);
    }
  }
  return total;
}

std::ostream &operator<<(std::ostream &os, Resource_cache const &res_cache)
{
  os << "<Resource_cache::size=" << res_cache.size() << ">";
  return os;
}

void to_json(nlohmann::json &j, const Resource_cache &res_cache)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    player::Color color = static_cast<player::Color>(c);
    std::string clr_key = player::to_string(color);
    nlohmann::json cache;
    for (size_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      std::string res_key = to_string(static_cast<Resource>(r));
      cache[res_key] = res_cache.m_resources[c][r];
    }
    j[clr_key] = cache;
  }
}

void from_json(const nlohmann::json &j, Resource_cache &res_cache)
{
  for (uint8_t c = 0; c < player::MAX_COLORS; c++)
  {
    player::Color color = static_cast<player::Color>(c);
    std::string clr_key = player::to_string(color);
    for (size_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
    {
      Resource res = static_cast<Resource>(r);
      std::string res_key = RESOURCE_NAMES[r];
      uint16_t amount = j.at(clr_key).at(res_key).get<uint16_t>();
      if (common::ERR_NONE != res_cache.add(res, color, amount))
      {
        res_cache.clear();
        std::stringstream msg;
        msg << "Failed adding resource amount: "
            << "res=" << res_key << ", player=" << clr_key
            << ", amount=" << amount;
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
    }
  }
}
} // namespace portable