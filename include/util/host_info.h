#pragma once

#include <string>
#include <cstdint>

#define HOSINFO_DEFAULT_ADDRESS "0.0.0.0"
#define HOSINFO_DEFAULT_PORT    50051

namespace util {

class HostInfo
{
private:
  std::string m_address;
  uint16_t    m_port;

public:
  HostInfo() : m_address(HOSINFO_DEFAULT_ADDRESS), m_port(HOSINFO_DEFAULT_PORT) {}
  HostInfo(const std::string& _addrPort) { set(_addrPort); }

  operator bool() const { return !m_address.empty(); }

  std::string to_str() const
  {
    return m_address + ":" + std::to_string(m_port);
  }

  const std::string& address() const { return m_address; }

  uint16_t port() const { return m_port; }

  void set_port(uint16_t _port) { m_port = _port; }

  void set_defaults() { m_address = HOSINFO_DEFAULT_ADDRESS; m_port = HOSINFO_DEFAULT_PORT; } 

  // Accepts "address" or "address:port" format, where address can be IPv4, IPv6 or hostname
  // If port is not specified, the current port value is retained
  void set(const std::string& _addrPort)
  {
    auto pos = _addrPort.find(']'); // Check for end of IPv6 address
    if ( pos != std::string::npos ) //  It is an IPv6 address
    {
      if ( _addrPort.size() > pos + 1 && _addrPort[pos + 1] == ':' )
      {
        m_address = _addrPort.substr(0, pos + 1);
        m_port = static_cast<uint16_t>(std::atoi(_addrPort.substr(pos + 2).c_str()));
      }
      else
      {
        m_address = _addrPort;
      }
    }
    else
    {
      // It could be an IPv4 address or hostname
      pos = _addrPort.rfind(':');
      if ( pos != std::string::npos )
      {
        m_address = _addrPort.substr(0, pos);
        m_port = static_cast<uint16_t>(std::atoi(_addrPort.substr(pos + 1).c_str()));
      }
      else
      {
        m_address = _addrPort;
      }
    }
  }
};

}
