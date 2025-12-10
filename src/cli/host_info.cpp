#include "cli/host_info.h"

using namespace data_shield::cli;

HostInfo::HostInfo() : m_address(HOSINFO_DEFAULT_ADDRESS), m_port(HOSINFO_DEFAULT_PORT)
{
}

HostInfo::HostInfo(const std::string &_addrPort) : m_address(""), m_port(0)
{
  set(_addrPort);
}

HostInfo::operator bool() const
{
  return !m_address.empty();
}

std::string HostInfo::to_str() const
{
  return m_address + ":" + std::to_string(m_port);
}

void HostInfo::set_port(uint16_t _port)
{
  m_port = _port;
}

void HostInfo::set_defaults()
{
  m_address = HOSINFO_DEFAULT_ADDRESS;
  m_port = HOSINFO_DEFAULT_PORT;
}

void HostInfo::set(const std::string &_addrPort)
{
  auto pos = _addrPort.find(']'); // Check for end of IPv6 address
  if (pos != std::string::npos)   //  It is an IPv6 address
  {
    if (_addrPort.size() > pos + 1 && _addrPort[pos + 1] == ':')
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
    if (pos != std::string::npos)
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
