#include "libhal-xbee/xbee.hpp"

#include <algorithm>
#include <array>
#include <span>

#include <libhal-util/serial.hpp>
#include <libhal-util/streams.hpp>
#include <libhal/serial.hpp>

namespace hal::xbee {

xbee_radio::xbee_radio(hal::serial& p_serial)
  : m_serial(&p_serial)
{
}

result<xbee_radio> xbee_radio::create(hal::serial& p_serial)
{
  xbee_radio new_xbee(p_serial);
  return new_xbee;
}

hal::result<std::span<hal::byte>> xbee_radio::read()
{
  auto data_recieved = HAL_CHECK(m_serial->read(m_xbee_buffer)).data;
  return data_recieved;
}

void xbee_radio::write(std::span<const hal::byte> p_data)
{
  hal::write(*m_serial, p_data);
}

}  // namespace hal::xbee
