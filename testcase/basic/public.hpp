#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "rpc.hpp"

#include <string>
#include <sys/uio.h>
#include <unistd.h>

namespace pub {

using namespace vastina;

constexpr i16 server_port = 5678;
constexpr i16 client_port = 6789;

// binary pack

typedef struct requst_buf_t
{
  std::string callee_ {};
  std::string args_ {};
  // whose name will exceed UINT8_MAX?
  inline void WriteIn( int fd )
  {
    struct iovec iv[4];
    u16 callee_length_ { (u16)callee_.size() };
    iv[0].iov_base = (void*)( &callee_length_ );
    iv[0].iov_len = sizeof( callee_length_ );
    auto args_length_ { (u32)args_.size() };
    iv[1].iov_base = (void*)( &args_length_ );
    iv[1].iov_len = sizeof( args_length_ );
    iv[2].iov_base = (void*)callee_.data();
    iv[2].iov_len = callee_.size();
    iv[3].iov_base = (void*)args_.data();
    iv[3].iov_len = args_.size();
    ::writev( fd, iv, 4 );
  }
  inline void ReadOut( int fd )
  {
    struct iovec iv[2];
    u16 callee_length_;
    {
      char buf[3];
      read( fd, buf, 2 );
      callee_length_ = ( buf[1] << 8 ) + buf[0];
      callee_.resize( callee_length_ );
      iv[0].iov_base = (void*)callee_.data();
      iv[0].iov_len = callee_length_;
    }
    u32 args_length_;
    {
      char buf[5];
      read( fd, buf, 4 );
      args_length_ = ( (u32)buf[3] << 24 ) + ( (u32)buf[2] << 16 ) + ( (u32)buf[1] << 8 ) + buf[0];
      args_.resize( args_length_ );
      iv[1].iov_base = (void*)args_.data();
      iv[1].iov_len = args_length_;
    }
    // ::read(fd, callee_.data(), callee_length_);
    // ::read(fd, args_.data(), args_length_);
    ::readv( fd, iv, 2 );
  }
} requst_buf_t;

typedef struct response_buf_t
{
  std::string result_;
} response_buf_t;

#pragma pack( 1 )
typedef struct Point
{
  i32 x;
  i32 y;
} Point;

typedef struct Circle
{
  Point center;
  i32 radius;
} Circle;

typedef struct Sth
{
  i32 x;
  i32 y;
  bool flag;
} Sth;
#pragma pack()

Sth return_sth( const Circle& c, const Point& p );

i32 addd( i32 a, i32 b );

};

#endif