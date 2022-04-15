#include <algorithm>
#include <iterator>
#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Player.h>
#include <portables/resources/Cache.h>
#include <portables/resources/Resource.h>

namespace portable
{

Cache::Cache() {}

Cache::Cache(const Cache &other)
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (other.m_resources.find(key) != other.m_resources.end())
    {
      m_resources.insert({key, other.m_resources.at(key)});
    }
  }
}

Cache::~Cache() { clear(); }

bool Cache::operator==(Cache &other)
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if ((m_resources.find(key) != m_resources.end()) !=
        (other.m_resources.find(key) != other.m_resources.end()))
    {
      return false;
    }
    if ((m_resources.find(key) != m_resources.end()) &&
        (m_resources.at(key) != other.m_resources.at(key)))
    {
      return false;
    }
  }

  return true;
}

bool Cache::operator==(Cache const &other) const
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if ((m_resources.find(key) != m_resources.end()) !=
        (other.m_resources.find(key) != other.m_resources.end()))
    {
      return false;
    }
    if ((m_resources.find(key) != m_resources.end()) &&
        (m_resources.at(key) != other.m_resources.at(key)))
    {
      return false;
    }
  }

  return true;
}

bool Cache::operator!=(Cache &other) { return !(*this == other); }
bool Cache::operator!=(Cache const &other) const { return !(*this == other); }

Cache Cache::operator=(const Cache &other)
{
  clear();
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (other.m_resources.find(key) != other.m_resources.end())
    {
      m_resources.insert({key, other.m_resources.at(key)});
    }
  }
  return (*this);
}

Cache Cache::operator+(const Cache &other)
{
  return (*this + other.m_resources);
}

Cache Cache::operator+(const Cache &other) const
{
  return (*this + other.m_resources);
}

Cache Cache::
operator+(const std::map<Resource::Type, std::vector<Resource>> &res_list)
{
  Cache merged = Cache(*this);
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if ((m_resources.find(key) != m_resources.end()) &&
        (res_list.find(key) != res_list.end()))
    {
      merged.m_resources.at(key).insert(merged.m_resources.at(key).end(),
                                        res_list.at(key).begin(),
                                        res_list.at(key).end());
    }
    else if (res_list.find(key) != res_list.end())
    {
      merged.m_resources.insert({key, res_list.at(key)});
    }
  }
  return merged;
}

Cache Cache::
operator+(const std::map<Resource::Type, std::vector<Resource>> &res_list) const
{
  Cache merged = Cache(*this);
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if ((m_resources.find(key) != m_resources.end()) &&
        (res_list.find(key) != res_list.end()))
    {
      merged.m_resources.at(key).insert(merged.m_resources.at(key).end(),
                                        res_list.at(key).begin(),
                                        res_list.at(key).end());
    }
    else if (res_list.find(key) != res_list.end())
    {
      merged.m_resources.insert({key, res_list.at(key)});
    }
  }
  return merged;
}

void Cache::operator+=(Cache const &other) { *this += other.m_resources; }

void Cache::
operator+=(std::map<Resource::Type, std::vector<Resource>> const &res_list)
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if ((res_list.find(key) != res_list.end()) &&
        (m_resources.find(key) != m_resources.end()))
    {
      m_resources.at(key).insert(m_resources.at(key).end(),
                                 res_list.at(key).begin(),
                                 res_list.at(key).end());
    }
    else if (res_list.find(key) != res_list.end())
    {
      m_resources.insert({key, res_list.at(key)});
    }
  }
}

void Cache::clear() { m_resources.clear(); }

void Cache::reset()
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (m_resources.find(key) != m_resources.end())
    {
      for (size_t i = 0; i < m_resources.at(key).size(); i++)
      {
        m_resources.at(key).at(i).reset();
      }
    }
  }
}

uint16_t Cache::count(const Resource::Type res) const
{
  uint16_t total = 0;
  if ((Resource::is_valid(res)) && (m_resources.find(res) != m_resources.end()))
  {
    total = static_cast<uint16_t>(m_resources.at(res).size());
  }
  return total;
}

uint16_t Cache::count_moveable(const Resource::Type res,
                               const player::Color player) const
{
  uint16_t total = 0;
  if ((Resource::is_valid(res)) && (player::is_valid(player)) &&
      (m_resources.find(res) != m_resources.end()))
  {
    if (player::Color::neutral == player)
    {
      total = static_cast<uint16_t>(m_resources.at(res).size());
    }
    else
    {
      for (auto r : m_resources.at(res))
      {
        total += (!r.was_carried_by(player) ? 1 : 0);
      }
    }
  }
  return total;
}

std::map<Resource::Type, std::vector<Resource>>
Cache::all_moveable(const player::Color p) const
{
  std::map<Resource::Type, std::vector<Resource>> result;
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (m_resources.find(key) == m_resources.end())
    {
      continue;
    }
    std::vector<Resource> moveable;
    for (auto res : m_resources.at(key))
    {
      if (!res.was_carried_by(p))
      {
        moveable.push_back(res);
      }
    }
    result.insert({key, moveable});
  }
}

common::Error Cache::add(Resource res)
{
  if (!Resource::is_valid(res.get_type()))
  {
    return common::ERR_INVALID;
  }

  if (m_resources.find(res.get_type()) != m_resources.end())
  {
    m_resources.at(res.get_type()).push_back(res);
  }
  else
  {
    std::vector<Resource> r_list;
    r_list.push_back(res);
    m_resources.insert({res.get_type(), r_list});
  }
  return common::ERR_NONE;
}

common::Error
Cache::add(std::map<Resource::Type, std::vector<Resource>> &res_list)
{
  for (std::map<Resource::Type, std::vector<Resource>>::iterator it =
           res_list.begin();
       it != res_list.end(); ++it)
  {
    if (!Resource::is_valid(it->first))
    {
      return common::ERR_INVALID;
    }
    for (auto res : res_list.at(it->first))
    {
      if (it->first != res.get_type())
      {
        return common::ERR_INVALID;
      }
    }
  }
  (*this += res_list);
  return common::ERR_NONE;
}

common::Error Cache::remove(const Resource::Type res, const uint16_t amount)
{
  if (!Resource::is_valid(res))
  {
    return common::ERR_INVALID;
  }
  if (count(res) < amount)
  {
    return common::ERR_FAIL;
  }

  m_resources.at(res).erase(m_resources.at(res).end() - amount,
                            m_resources.at(res).end());

  return common::ERR_NONE;
}

common::Error Cache::get(const Resource::Type res,
                         std::vector<Resource> &result, const uint16_t amount)
{
  common::Error err = common::ERR_NONE;
  if (!Resource::is_valid(res))
  {
    err = common::ERR_INVALID;
  }
  else if (count(res) < amount)
  {
    err = common::ERR_FAIL;
  }

  if ((common::ERR_NONE == err) && (0 != amount))
  {
    std::copy(m_resources.at(res).end() - amount, m_resources.at(res).end(),
              std::back_inserter(result));

    m_resources.at(res).erase(m_resources.at(res).end() - amount,
                              m_resources.at(res).end());
  }

  return err;
}

common::Error Cache::get(const Resource::Type res, const player::Color clr,
                         std::vector<Resource> &result, const uint16_t amount)
{
  common::Error err = common::ERR_NONE;
  if ((!Resource::is_valid(res)) || (!player::is_valid(clr)))
  {
    err = common::ERR_INVALID;
  }
  else if (count_moveable(res, clr) < amount)
  {
    err = common::ERR_FAIL;
  }

  if ((common::ERR_NONE == err) && (0 != amount))
  {
    std::vector<Resource> unmoved;
    std::vector<Resource> to_remove;
    size_t pre_move = m_resources.at(res).size();
    size_t idx = 0;
    while ((idx != m_resources.at(res).size()) && (to_remove.size() < amount))
    {
      if (m_resources.at(res).at(idx).was_carried_by(clr))
      {
        unmoved.push_back(m_resources.at(res).at(idx));
      }
      else
      {
        common::Error add_carrier_err =
            m_resources.at(res).at(idx).add_carrier(clr);
        if (add_carrier_err)
        {
          err = add_carrier_err;
          break;
        }
        to_remove.push_back(m_resources.at(res).at(idx));
      }
      idx++;
    }

    if ((common::ERR_NONE == err) && (to_remove.size() != amount))
    {
      // We somehow didn't remove enough resources, despite checking first.
      err = common::ERR_FAIL;
    }
    else if (common::ERR_NONE == err)
    {
      m_resources.at(res) = unmoved;
      result.insert(to_remove.begin(), to_remove.end(), result.end());
    }
  }

  return err;
}

uint32_t Cache::size() const
{
  uint32_t total = 0;
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (m_resources.find(key) != m_resources.end())
    {
      total += static_cast<uint32_t>(m_resources.at(key).size());
    }
  }
  return total;
}

std::ostream &operator<<(std::ostream &os, Cache const &res_cache)
{
  os << "<Cache::size=" << res_cache.size() << ">";
  return os;
}

void to_json(nlohmann::json &j, const Cache &res_cache)
{
  for (size_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (res_cache.m_resources.find(key) != res_cache.m_resources.end())
    {
      j[Resource::to_string(key)] = res_cache.m_resources.at(key);
    }
  }
}

void from_json(const nlohmann::json &j, Cache &res_cache)
{
  for (auto &[key, value] : j.items())
  {
    Resource::Type t = Resource::from_string(key);
    if (Resource::Type::invalid == t)
    {
      std::stringstream msg;
      msg << "Invalid resource key " << key << " listed in cache!";
      throw nlohmann::json::type_error::create(501, msg.str(), j);
    }

    std::vector<Resource> res_list = value.get<std::vector<Resource>>();
    for (auto resource : res_list)
    {
      if (t != resource.get_type())
      {
        std::stringstream msg;
        msg << "Resource type mismatch in " << key
            << " list: " << Resource::to_string(resource.get_type());
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
    }
    res_cache.m_resources.insert({t, res_list});
  }
}
} // namespace portable