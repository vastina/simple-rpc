#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "rpc.hpp"

#include <stdexcept>
#include <tuple>
#include <concepts>

#include <cstring>
#include <utility>

namespace vastina {

namespace details {

template<typename ty>
void single_cpy(void *dest, const void *src){
  std::memcpy(dest, src, sizeof(ty));
};

// a concept need here
template<const u32 Nm, const u32 index, typename ty>
decltype(auto) get_nth_ele(const ty& args){
  if constexpr(Nm == index){
    return std::get<Nm>(args);
  }
  if constexpr(Nm > index){
    get_nth_ele<Nm - 1, ty>(args);
  }
  throw std::out_of_range("Index out of range");
};

template<typename... Args>
// a param to help get index_sequence, not used directly
decltype(auto) index_sequence(const std::tuple<Args...>&){
  return std::index_sequence_for<Args...> {};
};

}


}

#endif