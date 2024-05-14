#ifndef _SERIALIZE_H_
#define _SERIALIZE_H_

#include "rpc.hpp"

#include <cassert>
#include <concepts>
#include <iostream>
#include <stdexcept>
#include <tuple>

#include <cstring>
#include <utility>

namespace vastina {

namespace details {

template<typename ty>
void single_cpy( void* dest, const void* src )
{
  std::memcpy( dest, src, sizeof( ty ) );
};

// a concept need here
template<const u32 Nm, const u32 index, typename ty>
decltype( auto ) get_nth_ele( const ty& args )
{
  static_assert(Nm >= index);
  if constexpr ( Nm == index ) {
    return std::get<Nm>( args );
  }
  if constexpr ( Nm > index ) {
    get_nth_ele<Nm - 1, ty>( args );
  }
  throw std::out_of_range( "Index out of range" );
};

template<const std::size_t Nm, typename ty>
// ty need a concept here
void print_tuple( const ty& args )
{
  if constexpr ( Nm > 0 ) {
    print_tuple<Nm - 1, ty>( args );
  } else {
    return (void)( std::cout << std::hex << std::get<0>( args ) << '\n' );
  }
  std::cout << std::get<Nm>( args ) << '\n';
}

template<typename... Args>
// a param to help get index_sequence, not used directly
decltype( auto ) index_sequence( const std::tuple<Args...>& )
{
  return std::index_sequence_for<Args...> {};
};

template<typename... Args>
std::string make_args_binary(const Args&... args){
  std::string argstr{};
  const auto data {std::make_tuple(args...)};
  argstr.resize(sizeof(data));
  std::memcpy(argstr.data(), &data, sizeof( data ));

  return argstr;
}

}

}

#endif