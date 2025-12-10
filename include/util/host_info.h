#pragma once

#include <string>
#include <cstdint>

#define HOSINFO_DEFAULT_ADDRESS "0.0.0.0"
#define HOSINFO_DEFAULT_PORT    50051

namespace data_shield::util {

class HostInfo
{
private:
  std::string m_address;
  uint16_t    m_port;

public:
  const std::string& address() const { return m_address; }
  uint16_t port() const { return m_port; }

public:
  HostInfo();
  HostInfo(const std::string& _addrPort);

  operator bool() const;
  std::string to_str() const;
  void set_defaults();
  // Accepts "address" or "address:port" format, where address can be IPv4, IPv6 or hostname
  // If port is not specified, the current port value is retained
  void set(const std::string& _addrPort);
  void set_port(uint16_t _port);

};

} // namespace data_shield::util
