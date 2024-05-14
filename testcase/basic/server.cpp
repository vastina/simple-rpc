#include "bind.hpp"
#include "public.hpp"
#include "tools.hpp"
#include <cstdlib>
#include <ctime>

namespace pub {

bool inCircle( const Circle& c, const Point& p )
{
  return ( c.center.x - p.x ) * ( c.center.x - p.x ) + ( c.center.y - p.y ) * ( c.center.y - p.y )
         <= c.radius * c.radius;
}

Sth return_sth( const Circle& c, const Point& p )
{
  Sth s {};
  std::srand( std::time( nullptr ) );
  // auto rd {std::default_random_engine()};
  // s.x = p.x + rd() % 100;
  // s.y = p.y + rd() % 100;
  s.x = p.x + std::rand() % 10000;
  s.y = p.y + std::rand() % 10000;
  s.flag = inCircle( c, p ) && ( std::rand() % 3 != 0 );
  return s;
}

i32 addd( i32 a, i32 b )
{
  return a + b;
}

}

int main()
{
  vastina::CallTable ct {};
  int sock = CreateSeverSocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, pub::server_port );
  int clnt = accept( sock, nullptr, nullptr );

  {
    using traits = vastina::func_traits<decltype( &pub::return_sth )>;
    ct.bind( std::string( "return_sth" ), &pub::return_sth );

    int count = 0;
    while ( true ) {

      pub::requst_buf_t req {};
      req.ReadOut( clnt );

      if ( req.args_.size() >= sizeof( traits::args_type ) ) {
        char response[sizeof( traits::return_type ) + 1];
        ct.exec( req.callee_, req.args_.data(), response );
        ::write( clnt, response, sizeof( traits::return_type ) );

        if ( count++ >= 10 )
          break;
      }
    }
  }

  {
    using traits = vastina::func_traits<decltype( &pub::addd )>;
    ct.bind( std::string( "addd" ), &pub::addd );

    int count = 0;
    while ( true ) {

      pub::requst_buf_t req {};
      req.ReadOut( clnt );

      if ( req.args_.size() >= sizeof( traits::args_type ) ) {
        char response[sizeof( traits::return_type ) + 1];
        ct.exec( req.callee_, req.args_.data(), response );
        ::write( clnt, response, sizeof( traits::return_type ) );

        if ( count++ >= 10 )
          break;
      }
    }
  }
  ::close( sock );
  return 0;
}