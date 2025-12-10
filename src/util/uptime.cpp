#include "util/uptime.h"
#include <iomanip>
#include <sstream>

using namespace data_shield::util;

Uptime::Uptime(uint64_t _epoch/* = 0*/)
{
  set(_epoch);
}

Uptime& Uptime::set(uint64_t _epoch)
{
  this->seconds = _epoch % 60;
  _epoch /= 60;
  this->minutes = _epoch % 60;
  _epoch /= 60;
  this->hours = _epoch % 24;
  this->days = _epoch / 24;
  return *this;
}

uint64_t Uptime::to_epoch() const
{
  return ((this->days * 24 + this->hours) * 60 + this->minutes) * 60 + this->seconds; 
}

std::string Uptime::to_str() const
{
  std::stringstream out;
  if ( this->days > 0 )
      out << this->days << " days ";
  out << std::setfill('0') << std::setw(2) << this->hours << ":"
      << std::setfill('0') << std::setw(2) << this->minutes << ":"
      << std::setfill('0') << std::setw(2) << this->seconds;
  return out.str();
}
