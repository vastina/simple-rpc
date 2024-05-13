#include "public.hpp"
#include "tools.hpp"
#include <random>

namespace pub {

bool inCircle(const Circle &c, const Point &p){
  return (c.center.x - p.x) * (c.center.x - p.x) + (c.center.y - p.y) * (c.center.y - p.y) 
    <= c.radius * c.radius;
}

Sth return_sth(const Circle &c, const Point &p){
  Sth s {};
  auto rd {std::default_random_engine()};
  s.x = p.x + rd() % 100;
  s.y = p.y + rd() % 100;
  s.flag = inCircle(c, p) && (rd() % 3 != 0);
  return s;
}

}

int main(){
  vastina::CallTable ct{};
  
  using traits =  vastina::func_traits<decltype(&pub::return_sth)>;
  ct.bind(std::string("return_sth"), &pub::return_sth,
  []( char* reqs, void* args ) {
    vastina::details::single_cpy<traits::args_type>(args, reqs); },
  []( char* resp, void* result ) {
    vastina::details::single_cpy<traits::return_type>(resp, result); }
  );

  int sock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, pub::server_port);
  int clnt = accept(sock, nullptr, nullptr);
  
  int count = 0;
  while (true) {

    pub::requst_buf_t req{};
    req.ReadOut(clnt);

    if(req.args_.size() >= sizeof(traits::args_type)){
      char response[sizeof(traits::return_type)+1];
      ct.exec(req.callee_, req.args_.data(), response);
      ::write(clnt, response, sizeof(traits::return_type) );
      
      if(count++ >= 10) break;
    }
  }

  ::close(sock);
  return 0;
}