#ifndef __BIND_H__
#define __BIND_H__

#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

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
  template<typename fn>
  void bind( const std::string id, fn func )
  {
    if ( handlers.contains( id ) ) { // for contains, use -std=c++20 or higher standard
      // do something
    }
    using ret_type = typename func_traits<fn>::return_type;
    using arg_type = typename func_traits<fn>::args_type;
    handlers.insert( std::make_pair( id, [func]( char* requst, char* response ) {
      arg_type requst_args {};
      ( []( char* requst_buf, arg_type& args ) {
        // do deserialize here
      } )( requst, requst_args );

      ret_type result = call( func, requst_args );
      // clang-format off
std::cout << result << '\n';
      // clang-format on
      ( []( char* response_buf, ret_type results ) {
        // do serialize here
      } )( response, result );
    } ) );
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

#endif