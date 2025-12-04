#include <iomanip>
#include <sstream>
#include <cstdint>

struct Uptime
{
    uint64_t seconds : 6;
    uint64_t minutes : 6;
    uint64_t hours   : 5;
    uint64_t days    : 47;

    Uptime(uint64_t _epoch = 0);
    Uptime& set(uint64_t _epoch);
    uint64_t to_epoch() const;
    std::string to_string() const;
};

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

std::string Uptime::to_string() const
{
  std::stringstream out;
  if ( this->days > 0 )
      out << this->days << " days ";
  out << std::setfill('0') << std::setw(2) << this->hours << ":"
      << std::setfill('0') << std::setw(2) << this->minutes << ":"
      << std::setfill('0') << std::setw(2) << this->seconds;
  return out.str();
}
