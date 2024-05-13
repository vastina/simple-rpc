#ifndef __TOOL_H_
#define __TOOL_H_

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <errno.h>
#include <format>
#include <iostream>
#include <string_view>
#include <sys/socket.h>
#include <sys/unistd.h>

#define INVALID_SOCKET -1

template<typename... Args>
void errorhandling( const std::string_view fmt_str, Args&&... args )
{
  auto fmt_args { std::make_format_args( args... ) };
  std::string outstr { std::vformat( fmt_str, fmt_args ) };
  fputs( outstr.c_str(), stderr );
};

template<typename... Args>
void print( const std::string_view fmt_str, Args&&... args )
{
  auto fmt_args { std::make_format_args( args... ) };
  std::string outstr { std::vformat( fmt_str, fmt_args ) };
  fputs( outstr.c_str(), stdout );
};

inline void showtime()
{
  auto currentTime = std::chrono::system_clock::now();
  std::time_t currentTime_t = std::chrono::system_clock::to_time_t( currentTime );
  std::tm localTime = *std::localtime( &currentTime_t );
  std::cout << "Current local time: " << localTime.tm_year + 1900 << "-" // 年份，需要加上 1900
            << localTime.tm_mon + 1 << "-"                               // 月份，需要加上 1
            << localTime.tm_mday << " "                                  // 日
            << localTime.tm_hour << ":"                                  // 时
            << localTime.tm_min << ":"                                   // 分
            << localTime.tm_sec << ".\n\n";                              // 秒
  //<< microseconds.count() << std::endl; // 微秒
}

inline void getTimebyStr( std::string& str )
{
  auto currentTime = std::chrono::system_clock::now();
  std::time_t currentTime_t = std::chrono::system_clock::to_time_t( currentTime );
  std::tm localTime = *std::localtime( &currentTime_t );
  str += std::format( "Current local time: {}-{}-{} {}:{}:{}\n",
                      localTime.tm_year + 1900,
                      localTime.tm_mon + 1,
                      localTime.tm_mday,
                      localTime.tm_hour,
                      localTime.tm_min,
                      localTime.tm_sec );
}

constexpr std::string_view localhost = "127.0.0.1";

inline int CreateSeverSocket( int domain, int type, int protocol, unsigned short PORT )
{
  int so = socket( domain, type, protocol );
  if ( so == INVALID_SOCKET ) {
    errorhandling( "invalid socket: {}\n", errno );
    return INVALID_SOCKET;
  }
  sockaddr_in addr {};
  addr.sin_family = domain;
  addr.sin_port = htons( PORT );
  addr.sin_addr.s_addr = inet_addr( localhost.data() );
  if ( bind( so, (struct sockaddr*)&addr, sizeof( addr ) ) == -1 ) {
    errorhandling( "connect fail with code: {}\n", errno );
    return false;
  }
  // 监听
  listen( so, 100 );
  return so;
}

inline int CreateClientSocket( int domain, int type, int protocol, short PORT )
{
  int so = socket( domain, type, protocol );
  if ( so == INVALID_SOCKET ) {
    errorhandling( "invalid socket: {}\n", errno );
    return INVALID_SOCKET;
  }
  sockaddr_in addr {};
  memset( &addr, 0, sizeof( addr ) );
  addr.sin_family = domain;
  if ( -1 != PORT ) {
    addr.sin_port = htons( PORT );
  } else {
    addr.sin_port = htons( INADDR_ANY );
  }
  addr.sin_addr.s_addr = inet_addr( localhost.data() );
  if ( bind( so, (struct sockaddr*)&addr, sizeof( addr ) ) == -1 ) {
    errorhandling( "connect fail with code: {}\n", errno );
    return INVALID_SOCKET;
  }

  return so;
}

inline void Connect( int sock, int domain, short PORT )
{
  struct sockaddr_in serveaddr;
  memset( &serveaddr, 0, sizeof( serveaddr ) );
  serveaddr.sin_family = domain;
  serveaddr.sin_addr.s_addr = inet_addr( localhost.data() );
  serveaddr.sin_port = htons( PORT );
  if ( connect( sock, (struct sockaddr*)&serveaddr, sizeof( serveaddr ) ) == -1 ) {
    errorhandling( "connect fail with code: {}\n", errno );
  }
}

#endif