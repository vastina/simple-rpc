#include "bind.hpp"
#include "public.hpp"
#include "rpc.hpp"
#include "serialize.hpp"
#include "tools.hpp"
#include <fcntl.h>
#include <random>

using namespace vastina;

int main(){
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

  int clntsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, pub::client_port);
  Connect(clntsock, AF_INET, pub::server_port);

  int count = 0;
  auto rd {std::default_random_engine()};
  while(true){
    using traits =  func_traits<decltype(&pub::return_sth)>;
    
    pub::requst_buf_t req {"return_sth"};
    {
      traits::args_type args {std::make_tuple(pub::Circle{{(i32)rd()%10, (i32)rd()%10}, 5}
        , pub::Point{(i32)rd(), (i32)rd()})};
      req.args_.resize(sizeof(traits::args_type));
      details::single_cpy<traits::args_type>(req.args_.data(), &args);
    }
    
    req.WriteIn(clntsock);

    traits::return_type res {};
    ::read(clntsock, &res, sizeof(traits::return_type));
    std::cout << res.x << " " << res.y << " " << res.flag << std::endl;

    if(count++ >= 10) break;
  }

  ::close(clntsock);
}