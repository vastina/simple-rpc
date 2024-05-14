// the main.cpp is just for test

#include "bind.hpp"
#include "serialize.hpp"

#include <cstdio>
#include <iostream>
#include <tuple>

#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

int abc( float a, double b )
{
  return (int)a + (int)b;
}
constexpr auto lambda { [] {} };
auto lambdap { +[] { return .1f; } };
struct aaa
{
  char aa( int ) { return {}; }
  long operator()() { return {}; }
  double bb( float ) const { return {}; }
};

using std::cout;
using namespace vastina;

template<typename... Args>
decltype( auto ) index_sequence( const std::tuple<Args...>& )
{
  return std::index_sequence_for<Args...> {};
};

int main()
{
  using func_abc = func_traits<decltype( &abc )>;
  using func_lambda = func_traits<decltype( lambda )>;
  using func_lambdap = func_traits<decltype( lambdap )>;
  using aaa_aa = func_traits<decltype( &aaa::aa )>;
  using aaa_op = func_traits<decltype( &aaa::operator() )>;
  using aaa_bb = func_traits<decltype( &aaa::bb )>;
  using in_bracket = func_traits<decltype( []( long, char, double ) { return (void*)( 0x114514 ); } )>;

  func_abc::print_type();
  func_lambda::print_type();
  func_lambdap::print_type();
  aaa_aa::print_type();
  aaa_op::print_type();
  aaa_bb::print_type();
  in_bracket::print_type();

  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  {
    func_abc::args_type args {};
    cout << ::call( &abc, args ) << '\n';

    // no arg is OK
    func_lambdap::args_type args2 {};
    cout << ::call( lambdap, args2 ) << '\n';
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  {
    in_bracket::args_type args { std::make_tuple( 0x1234567812345678, 'v', 1919810.114514 ) };
    const auto isq { index_sequence( args ) };
    details::print_tuple<isq.size() - 1>( args );
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  {
    constexpr auto helper { [] { ::system( "rm -f tmp.out" ); } };

    helper();
    constexpr aaa_aa::args_type args { std::make_tuple( 0x12345678 ) };
    const auto isq { details::index_sequence( args ) };
    details::print_tuple<isq.size() - 1>( args );
    {
      int fd = open( "tmp.out", O_WRONLY | O_CREAT, 0644 );
      write( fd, (void*)&args, sizeof( args ) );
      ::close( fd );
    }
    aaa_aa::args_type args2 {};
    {
      int fd = open( "tmp.out", O_RDONLY );
      char buf[sizeof( args2 ) + 1];
      auto length = read( fd, (void*)buf, sizeof( args2 ) );
      std::memcpy( (void*)&args2, (void*)buf, length );
      ::close( fd );
    }
    const auto isq2 { details::index_sequence( args2 ) };
    details::print_tuple<isq2.size() - 1>( args2 );

    helper();
    constexpr in_bracket::args_type args3 { std::make_tuple( 0x1234567812345678, 'x', 3.14 ) };
    const auto isq3 { details::index_sequence( args3 ) };
    details::print_tuple<isq3.size() - 1>( args3 );
    {
      int fd = open( "tmp.out", O_WRONLY | O_CREAT, 0644 );
      write( fd, (void*)&args3, sizeof( args3 ) );
      ::close( fd );
    }
    in_bracket::args_type args4 {};
    {
      int fd = open( "tmp.out", O_RDONLY );
      char buf[sizeof( args4 ) + 1];
      auto length = read( fd, (void*)buf, sizeof( args4 ) );
      std::memcpy( (void*)&args4, (void*)buf, length );
      ::close( fd );
    }
    const auto isq4 { details::index_sequence( args4 ) };
    details::print_tuple<isq4.size() - 1>( args4 );
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on

  CallTable ct {};

  ct.bind( "abc", &abc );

  func_abc::args_type args { std::make_tuple( 3.14f, 11.4514 ) };

  char response[sizeof( func_abc::return_type ) + 1];
  ct.exec( "abc", (char*)&args, response );
  // Pretend to be communicating here... requst above should be read from socket too...
  int fd = ::open( "sent.out", O_CREAT | O_WRONLY, 0644 );
  ::write( fd, response, strlen( response ) );
  ::close( fd );

  int fdc = ::open( "sent.out", O_RDONLY );
  func_abc::return_type res {};
  ::read( fdc, &res, sizeof( res ) );

  cout << res << '\n';

  // client : deserialize--->result   args -----serialize------> requst buffer
  //           /\                                                 |
  //            |                                                 |
  //            |                                                 |
  //            |                                                 |
  //            |                                                 \/
  // server :response<---serialize---result<----exec---args<---deserialize

  return 0;
}