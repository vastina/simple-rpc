#include "bind.hpp"

#include <iostream>
#include <tuple>

#include <cassert>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

int abc( float, double )
{
  return {};
}
auto lambda { [] {} };
auto lambdap { +[] { return .1f; } };
struct aaa
{
  char aa( int ) { return {}; }
  long operator()() { return {}; }
  double bb( float ) const { return {}; }
};

using std::cout;

template<typename... Args>
decltype(auto) index_sequence(std::tuple<Args...>& args){
  return std::index_sequence_for<Args...> {};
};
template<const std::size_t Nm, typename ty>
// ty need a concept here
void print_tuple(ty& args){
  if constexpr(Nm > 0){
    print_tuple<Nm - 1, ty>(args);
  } else {
    return (void)(cout << std::get<0>(args) << '\n');
  }
  cout << std::get<Nm>(args) << '\n';
}

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
    func_abc::args_type args{};
    cout << ::call(&abc, args) <<'\n';

    // no arg is OK
    func_lambdap::args_type args2{};
    cout << ::call(lambdap, args2) <<'\n';
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  {
    in_bracket::args_type args{ std::make_tuple(0x1234567812345678, 'v', 1919810.114514) };
    const auto isq { index_sequence(args) };
    print_tuple<isq.size()-1>(args);
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  {
    constexpr auto helper { [] { ::system( "rm -f tmp.out" ); } };

    constexpr auto printer_aa {
      []( const aaa_aa::args_type& args ) { cout << std::hex << std::get<0>( args ) << '\n'; } };
    helper();
    constexpr aaa_aa::args_type args { std::make_tuple( 0x12345678 ) };
    printer_aa( args );
    {
      int fd = open( "tmp.out", O_WRONLY | O_CREAT, 0644 );
      auto length = write( fd, (void*)&args, sizeof( args ) );
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
    printer_aa( args2 );

    constexpr auto printer_in { []( const in_bracket::args_type& args ) {
      cout << std::hex << std::get<0>( args ) << '\n';
      cout << std::hex << std::get<1>( args ) << '\n';
      cout << std::hex << std::get<2>( args ) << '\n';
    } };
    helper();
    constexpr in_bracket::args_type args3 { std::make_tuple( 0x1234567812345678, 'x', 3.14 ) };
    printer_in( args3 );
    {
      int fd = open( "tmp.out", O_WRONLY | O_CREAT, 0644 );
      auto length = write( fd, (void*)&args3, sizeof( args3 ) );
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
    printer_in( args4 );
  }
  // clang-format off
std::cout << "------------------------------------------------------------------------------------\n";
  // clang-format on
  CallTable ct {};
  char donothing {};
  ct.bind( "abc", &abc );
  ct.exec( "abc", &donothing, &donothing );
  // ct.bind("nothing", []{ return ; }); //void is unacceptable here
  // ct.exec("nothing", &donothing, &donothing);
  ct.bind( "nothing", [] { return 3.14f; } );
  ct.exec( "nothing", &donothing, &donothing );

  return 0;
}