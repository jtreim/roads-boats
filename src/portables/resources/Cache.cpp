#include <algorithm>
#include <iterator>
#include <map>
#include <ostream>

#include <nlohmann/json.hpp>

#include <common/Errors.h>
#include <players/Color.h>
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
      std::vector<std::unique_ptr<Resource>> res_list;
      m_resources.insert({key, std::move(res_list)});
      for (size_t i = 0; i < other.m_resources.at(key).size(); i++)
      {
        if (nullptr != other.m_resources.at(key).at(i))
        {
          m_resources.at(key).push_back(std::make_unique<Resource>(
              *(other.m_resources.at(key).at(i).get())));
        }
      }
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
        (m_resources.at(key).size() != other.m_resources.at(key).size()))
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
        (m_resources.at(key).size() != other.m_resources.at(key).size()))
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
      std::vector<std::unique_ptr<Resource>> res_list;
      for (size_t i = 0; i < other.m_resources.at(key).size(); i++)
      {
        Resource *r_ptr = other.m_resources.at(key).at(i).get();
        res_list.push_back(std::make_unique<Resource>(*r_ptr));
      }
      m_resources.insert({key, std::move(res_list)});
    }
  }
  return (*this);
}

Cache Cache::operator+(const Cache &other)
{
  Cache retval;
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource::Type key = static_cast<portable::Resource::Type>(i);
    std::vector<std::unique_ptr<portable::Resource>> r_list;
    if (other.m_resources.find(key) != other.m_resources.end())
    {
      for (const auto &res : other.m_resources.at(key))
      {
        if (nullptr != res)
        {
          r_list.push_back(std::make_unique<portable::Resource>(*res.get()));
        }
      }
    }
    if (m_resources.find(key) != other.m_resources.end())
    {
      for (const auto &res : m_resources.at(key))
      {
        if (nullptr != res)
        {
          r_list.push_back(std::make_unique<portable::Resource>(*res.get()));
        }
      }
    }

    if (!r_list.empty())
    {
      retval.m_resources.insert({key, std::move(r_list)});
    }
  }

  return retval;
}

Cache Cache::operator+(const Cache &other) const
{
  Cache retval;
  for (size_t i = 0; i < portable::RESOURCE_NAMES_SIZE; i++)
  {
    portable::Resource::Type key = static_cast<portable::Resource::Type>(i);
    std::vector<std::unique_ptr<portable::Resource>> r_list;
    if (other.m_resources.find(key) != other.m_resources.end())
    {
      for (const auto &res : other.m_resources.at(key))
      {
        if (nullptr != res)
        {
          r_list.push_back(std::make_unique<portable::Resource>(*res.get()));
        }
      }
    }
    if (m_resources.find(key) != other.m_resources.end())
    {
      for (const auto &res : m_resources.at(key))
      {
        if (nullptr != res)
        {
          r_list.push_back(std::make_unique<portable::Resource>(*res.get()));
        }
      }
    }

    if (!r_list.empty())
    {
      retval.m_resources.insert({key, std::move(r_list)});
    }
  }

  return retval;
}

Cache Cache::operator+(const std::vector<Resource *> &res_list)
{
  Cache merged = Cache(*this);
  for (auto r_ptr : res_list)
  {
    if (nullptr == r_ptr)
    {
      continue;
    }
    Resource::Type key = r_ptr->get_type();
    if (merged.m_resources.find(key) == m_resources.end())
    {
      std::vector<std::unique_ptr<portable::Resource>> r_list;
      merged.m_resources.insert({key, std::move(r_list)});
    }
    merged.m_resources.at(key).push_back(
        std::make_unique<portable::Resource>(*r_ptr));
  }
  return merged;
}

Cache Cache::operator+(const std::vector<Resource *> &res_list) const
{
  Cache merged = Cache(*this);
  for (auto r_ptr : res_list)
  {
    if (nullptr == r_ptr)
    {
      continue;
    }
    Resource::Type key = r_ptr->get_type();
    if (merged.m_resources.find(key) == m_resources.end())
    {
      std::vector<std::unique_ptr<Resource>> r_list;
      merged.m_resources.insert({key, std::move(r_list)});
    }
    merged.m_resources.at(key).push_back(std::make_unique<Resource>(*r_ptr));
  }
  return merged;
}

void Cache::operator+=(Cache const &other)
{
  for (size_t i = 0; i < RESOURCE_NAMES_SIZE; i++)
  {
    Resource::Type key = static_cast<Resource::Type>(i);
    if (other.m_resources.find(key) == other.m_resources.end())
    {
      continue;
    }
    if (m_resources.find(key) == m_resources.end())
    {
      std::vector<std::unique_ptr<Resource>> r_list;
      m_resources.insert({key, std::move(r_list)});
    }

    for (const auto &r_ptr : other.m_resources.at(key))
    {
      if (nullptr != r_ptr)
      {
        m_resources.at(key).push_back(std::make_unique<Resource>(*r_ptr));
      }
    }
  }
}

void Cache::operator+=(std::vector<Resource *> const &res_list)
{
  for (auto r_ptr : res_list)
  {
    if (nullptr == r_ptr)
    {
      continue;
    }
    Resource::Type key = r_ptr->get_type();
    if (m_resources.find(key) == m_resources.end())
    {
      std::vector<std::unique_ptr<Resource>> r_list;
      m_resources.insert({key, std::move(r_list)});
    }
    m_resources.at(key).push_back(std::make_unique<Resource>(*r_ptr));
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
        if (nullptr == m_resources.at(key).at(i))
        {
          continue;
        }
        m_resources.at(key).at(i)->reset();
      }
    }
  }

  clean();
}

void Cache::clean()
{
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (m_resources.find(key) == m_resources.end())
    {
      continue;
    }

    for (auto i = m_resources.at(key).end() - 1;
         i >= m_resources.at(key).begin(); --i)
    {
      if (nullptr == *i)
      {
        m_resources.at(key).erase(i);
      }
    }

    if (m_resources.at(key).empty())
    {
      m_resources.erase(key);
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
      for (size_t i = 0; i < m_resources.at(res).size(); i++)
      {
        total +=
            (!m_resources.at(res).at(i).get()->was_carried_by(player) ? 1 : 0);
      }
    }
  }
  return total;
}

std::vector<Resource *> Cache::all() const
{
  std::vector<Resource *> result;
  for (uint8_t r = 0; r < portable::RESOURCE_NAMES_SIZE; r++)
  {
    portable::Resource::Type res_type =
        static_cast<portable::Resource::Type>(r);
    if (m_resources.find(res_type) != m_resources.end())
    {
      for (uint8_t i = 0; i < m_resources.at(res_type).size(); i++)
      {
        result.push_back(m_resources.at(res_type).at(i).get());
      }
    }
  }
  return result;
}

std::vector<Resource *> Cache::all_moveable(const player::Color p) const
{
  std::vector<Resource *> result;
  for (uint8_t r = 0; r < RESOURCE_NAMES_SIZE; r++)
  {
    Resource::Type key = static_cast<Resource::Type>(r);
    if (m_resources.find(key) == m_resources.end())
    {
      continue;
    }
    for (uint8_t i = 0; i < m_resources.at(key).size(); i++)
    {
      if (!m_resources.at(key).at(i)->was_carried_by(p))
      {
        result.push_back(m_resources.at(key).at(i).get());
      }
    }
  }
  return result;
}

common::Error Cache::add(Resource *&res)
{
  if ((nullptr == res) || (!Resource::is_valid(res->get_type())))
  {
    return common::ERR_INVALID;
  }

  if (m_resources.find(res->get_type()) != m_resources.end())
  {
    m_resources.at(res->get_type()).push_back(std::make_unique<Resource>(*res));
  }
  else
  {
    std::vector<std::unique_ptr<Resource>> r_list;
    r_list.push_back(std::make_unique<Resource>(*res));
    m_resources.insert({res->get_type(), std::move(r_list)});
  }
  return common::ERR_NONE;
}

common::Error Cache::add(const Resource::Type res)
{
  if (!Resource::is_valid(res))
  {
    return common::ERR_INVALID;
  }

  if (m_resources.find(res) != m_resources.end())
  {
    m_resources.at(res).push_back(std::make_unique<Resource>(res));
  }
  else
  {
    std::vector<std::unique_ptr<Resource>> r_list;
    r_list.push_back(std::make_unique<Resource>(res));
    m_resources.insert({res, std::move(r_list)});
  }
  return common::ERR_NONE;
}

common::Error Cache::add(std::vector<Resource *> &res_list)
{
  // Validate resources list before we add any
  for (size_t i = 0; i < res_list.size(); i++)
  {
    if ((nullptr == res_list.at(i)) ||
        (!Resource::is_valid(res_list.at(i)->get_type())))
    {
      return common::ERR_INVALID;
    }
  }

  for (size_t i = 0; i < res_list.size(); i++)
  {
    Resource::Type res_type = res_list.at(i)->get_type();
    if (m_resources.find(res_type) != m_resources.end())
    {
      m_resources.at(res_type).push_back(
          std::make_unique<Resource>(*(res_list.at(i))));
    }
    else
    {
      std::vector<std::unique_ptr<Resource>> new_list;
      new_list.push_back(std::make_unique<Resource>(*(res_list.at(i))));
      m_resources.insert({res_type, std::move(new_list)});
    }
  }
  return common::ERR_NONE;
}

common::Error Cache::add(std::vector<Portable *> &res_list)
{
  std::vector<Resource *> res_to_add;
  common::Error err = common::ERR_FAIL;
  // Convert all resource pointers from input to something usable.
  for (size_t i = 0; i < res_list.size(); i++)
  {
    if (res_list.at(i)->get_object() == Portable::Object::resource)
    {
      res_to_add.push_back(static_cast<Resource *>(res_list.at(i)));
    }
  }

  if (res_to_add.size() > 0)
  {
    err = add(res_to_add);
  }
  return err;
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
                         std::vector<std::unique_ptr<Resource>> &result,
                         const uint16_t amount)
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
    for (uint16_t i = 0; i < amount; i++)
    {
      size_t idx = m_resources.at(res).size() - i;
      result.push_back(std::move(m_resources.at(res).at(idx)));
    }
    m_resources.at(res).erase(m_resources.at(res).end() - amount,
                              m_resources.at(res).end());
  }

  return err;
}

common::Error Cache::get(const Resource::Type res, const player::Color clr,
                         std::vector<std::unique_ptr<Resource>> &result,
                         const uint16_t amount)
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
    std::vector<std::unique_ptr<Resource>> unmoved;
    std::vector<std::unique_ptr<Resource>> to_remove;
    size_t pre_move = m_resources.at(res).size();
    size_t idx = 0;
    while ((idx != m_resources.at(res).size()) && (to_remove.size() < amount))
    {
      if (m_resources.at(res).at(idx)->was_carried_by(clr))
      {
        unmoved.push_back(std::move(m_resources.at(res).at(idx)));
      }
      else
      {
        to_remove.push_back(std::move(m_resources.at(res).at(idx)));
      }
      idx++;
    }

    if ((!err) && (to_remove.size() != amount))
    {
      // We somehow didn't remove enough resources, despite checking first.
      err = common::ERR_FAIL;
    }
    else if (!err)
    {
      m_resources.at(res) = std::move(unmoved);
      // m_resources.erase(res);
      // m_resources.insert({res, std::move(unmoved)});
      // for (size_t i = 0; i < to_remove.size(); i++)
      // {
      //   result.push_back(std::move(to_remove.at(i)));
      // }
      result = std::move(to_remove);
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
      std::string res_key = Resource::to_string(key);
      for (size_t i = 0; i < res_cache.m_resources.at(key).size(); i++)
      {
        if (nullptr != res_cache.m_resources.at(key).at(i))
        {
          Resource res = *(res_cache.m_resources.at(key).at(i));
          nlohmann::json res_json;
          to_json(res_json, res);
          j[res_key].push_back(res_json);
        }
      }
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

    std::vector<Resource> res_json_list = value.get<std::vector<Resource>>();
    std::vector<std::unique_ptr<Resource>> res_list;
    for (auto resource : res_json_list)
    {
      if (t != resource.get_type())
      {
        std::stringstream msg;
        msg << "Resource type mismatch in " << key
            << " list: " << Resource::to_string(resource.get_type());
        throw nlohmann::json::type_error::create(501, msg.str(), j);
      }
      res_list.push_back(std::make_unique<Resource>(resource));
    }
    res_cache.m_resources.insert({t, std::move(res_list)});
  }
}
} // namespace portable