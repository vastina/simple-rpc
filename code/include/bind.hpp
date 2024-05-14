#ifndef __BIND_H__
#define __BIND_H__

#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "serialize.hpp"

namespace vastina {

// for more, see this, link : https://www.zhihu.com/question/636120604/answer/3345305152

template<typename fn, typename... Args>
struct func_traits;
template<typename fn, typename... Args>
struct func_traits<fn ( * )( Args... )>
{
  using return_type = fn;
  using args_type = std::tuple<typename std::decay<Args>::type...>; // std::tuple<Args...>;
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

private:
  constexpr static auto donothing { []( char*, void* ) {} }; // todo default trans
  template<typename fn, typename ret, typename args>
  void dobind( const std::string& id,
               fn func,
               const std::function<void( char*, args* )> req_tansf,
               const std::function<void( char*, ret* )> resp_tansf )
  {
    using ret_type = typename func_traits<fn>::return_type;
    using arg_type = typename func_traits<fn>::args_type;
    static_assert( std::is_same_v<ret_type, ret> );
    static_assert( std::is_same_v<arg_type, args> );
    handlers.insert( std::make_pair( id, [func, req_tansf, resp_tansf]( char* requst, char* response ) {
      arg_type requst_args {};
      req_tansf( requst, &requst_args ); // do deserialize here

      ret_type result = call( func, requst_args );
      resp_tansf( response, &result ); // do serialize here
    } ) );
  }

public:
  template<typename fn>
  void bind( const std::string& id, fn func )
  {
    if ( handlers.contains( id ) ) { // use -std=c++20 or higher standard
      // do something
    }
    using ret_type = typename func_traits<fn>::return_type;
    using arg_type = typename func_traits<fn>::args_type;
    dobind<fn, ret_type, arg_type>(
      id,
      func,
      []( char* requst_buf, arg_type* args ) { details::single_cpy<arg_type>( args, requst_buf ); },
      //{ args = reinterpret_cast<arg_type*>(requst_buf); },
      []( char* response_buf, ret_type* result ) { details::single_cpy<ret_type>( response_buf, result ); } );
  }

  void exec( std::string id, char* requst, char* response )
  {
    if ( !handlers.contains( id ) ) {
      // dosomething
      return;
    }
    return handlers.at( id )( requst, response );
  }
};

}; // namespace vastina

#endif