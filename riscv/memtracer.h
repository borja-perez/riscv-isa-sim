// See LICENSE for license details.

#ifndef _MEMTRACER_H
#define _MEMTRACER_H

#include <cstdint>
#include <string.h>
#include <vector>


enum access_type {
  LOAD,
  STORE,
  FETCH,
};

class memtracer_t
{
 public:
  memtracer_t() {}
  virtual ~memtracer_t() {}

  virtual bool interested_in_range(uint64_t begin, uint64_t end, access_type type) = 0;
  virtual bool trace(uint64_t addr, size_t bytes, access_type type, bool& hit, uint64_t& victim) = 0;
  
};


class memtracer_list_t : public memtracer_t
{
 public:
  bool empty() { return list.empty(); }
  bool interested_in_range(uint64_t begin, uint64_t end, access_type type)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      if ((*it)->interested_in_range(begin, end, type))
        return true;
    return false;
  }
  bool trace(uint64_t addr, size_t bytes, access_type type, bool& hit, uint64_t& victim)
  {
    bool res=false;
    bool global_hit=false;
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
    {
      bool local_hit=false;
      uint64_t local_victim;
      bool traced=(*it)->trace(addr, bytes, type, local_hit, local_victim);
      global_hit|=local_hit; //Hit if one hits
      res|=traced; //Traced if one traces
      if(local_victim!=0)
      {
        victim=local_victim;
      }
    }
    hit=global_hit;
    return res;
  }
  void hook(memtracer_t* h)
  {
    list.push_back(h);
  }
 private:
  std::vector<memtracer_t*> list;  

  
};

#endif
