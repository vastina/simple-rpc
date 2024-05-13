#ifndef __BIND_H__
#define __BIND_H__

#include "serialize.hpp"

#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace vastina {

// for more, see this, link : https://www.zhihu.com/question/636120604/answer/3345305152

template<typename fn, typename... Args>
struct func_traits;
template<typename fn, typename... Args>
struct func_traits<fn ( * )( Args... )>
{
  using return_type = fn;
  using args_type = std::tuple<Args...>;//std::tuple<typename std::decay<Args>::type...>;
  static void print_type()
  {
    std::cout << typeid( return_type ).name() << '\n';
    std::cout << typeid( args_type ).name() << '\n';
  }
};

template<typename ty>
struct func_traits<ty> : func_traits<decltype( &ty::operator() )>
{};

template<typename cls, typename fn, typename... Args>
struct func_traits<fn ( cls::* )( Args... )> : func_traits<fn ( * )( Args... )>
{};

template<typename cls, typename fn, typename... Args>
struct func_traits<fn ( cls::* )( Args... ) const> : func_traits<fn ( * )( Args... )>
{};

template<typename fn, typename... Args, std::size_t... I>
decltype( auto ) call_helper( fn f, std::tuple<Args...>&& params, std::index_sequence<I...> )
{
  return f( std::get<I>( params )... );
}

template<typename fn, typename... Args>
decltype( auto ) call( fn f, std::tuple<Args...>& args )
{
  return call_helper( f, std::forward<std::tuple<Args...>>( args ), std::index_sequence_for<Args...> {} );
}

class CallTable
{
  using Converter = std::function<void( char*, char* )>;
  std::unordered_map<std::string, Converter> handlers {};

public:
  CallTable() noexcept {}

public:
  
  constexpr static auto donothing { []( char*, void*) { }} ; //todo default trans
  template<typename fn>
  void bind( const std::string id, fn func ,
  const std::function<void(char*, void*)> req_tansf  = donothing,
  const std::function<void(char*, void*)> resp_tansf = donothing)
  {
// this is useless because I need c++20 at other places
//#if  __cplusplus >= 202002L
    if ( handlers.contains( id ) ) {
// #else
//     if ( handlers.count(id) == 1 )
// #endif
      // do something
    }
    using ret_type = typename func_traits<fn>::return_type;
    using arg_type = typename func_traits<fn>::args_type;
    handlers.insert( std::make_pair( id, [func, &req_tansf, &resp_tansf]( char* requst, char* response ) {
      arg_type requst_args {};
      req_tansf(requst, &requst_args);// do deserialize here

      ret_type result = call( func, requst_args );
      resp_tansf(response, &result);// do serialize here
    } ) );
  }

  void exec( std::string id, char* requst )
  {
    if ( !handlers.contains( id ) ) {
      // dosomething
      return;
    }
  // clang-format off
char response[BUFSIZ]; //todo tobe removed
  // clang-format on
    return handlers.at( id )( requst, response );
  }
};

}; // namespace vastina

#endif