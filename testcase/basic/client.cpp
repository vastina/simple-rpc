#include "bind.hpp"
#include "public.hpp"
#include "serialize.hpp"
#include "tools.hpp"
#include <cassert>
#include <cstdlib>
#include <fcntl.h>
#include <format>
#include <random>

using namespace vastina;

int main()
{
  // {
  //   using traits =  vastina::func_traits<decltype(&pub::return_sth)>;
  //   traits::args_type args {std::make_tuple(pub::Circle{{114, 514}, 1919}, pub::Point{8, 10})};

  //   pub::requst_buf_t req {"return_sth"};
  //   req.args_.resize(sizeof(traits::args_type));
  //   vastina::details::single_cpy<traits::args_type>(req.args_.data(), &args);
  //   int fd = open("test.out", O_CREAT|O_WRONLY, 0644);
  //   req.WriteIn(fd);
  //   ::close(fd);

  //   pub::requst_buf_t requ {};
  //   fd = open("test.out", O_RDONLY);
  //   requ.ReadOut(fd);
  //   ::close(fd);
  //   fd = open("test.new.out", O_CREAT|O_WRONLY, 0644);
  //   requ.WriteIn(fd);
  //   ::close(fd);

  //   return 0;
  // }

  int clntsock = CreateClientSocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, pub::client_port );
  Connect( clntsock, AF_INET, pub::server_port );
  auto rd { std::default_random_engine() };
  {
    int count = 0;
    while ( true ) {

      pub::requst_buf_t req { "return_sth" ,details::make_args_binary(
        pub::Circle { { (i32)rd() % 114, (i32)rd() % 514 }, 100 },
        pub::Point { (i32)rd(), (i32)rd() }) 
      };
      req.WriteIn( clntsock );

      func_traits<decltype( &pub::return_sth )>::return_type res {};
      ::read( clntsock, &res, sizeof( res ) );
      std::cout << res.x << " " << res.y << " " << res.flag << std::endl;

      if ( count++ >= 10 )
        break;
    }
  }

  {
    int count = 0;
    while ( true ) {
      auto x { (i32)rd() % 114514 };
      auto y { (i32)rd() % 114514 };
      pub::requst_buf_t req { "addd", details::make_args_binary(x, y) };

      std::cout << std::format( "calling addd({}, {})\n", x, y );
      req.WriteIn( clntsock );

      func_traits<decltype( &pub::addd )>::return_type res {};
      ::read( clntsock, &res, sizeof( res ) );
      std::cout << std::format( "asserting({} == {}+{})\n", res, x, y );
      assert( res == x + y );

      if ( count++ >= 10 )
        break;
    }
  }

  ::close( clntsock );
  return 0;
}