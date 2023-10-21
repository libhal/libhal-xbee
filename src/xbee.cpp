#include "libhal-xbee/xbee.hpp"

#include <algorithm>
#include <array>
#include <span>

#include <libhal-util/serial.hpp>
#include <libhal-util/streams.hpp>
#include <libhal/serial.hpp>

namespace hal::xbee {

result<xbee_radio> xbee_radio::create(hal::serial& p_serial,
                                      hal::steady_clock& p_clock)
{
  xbee_radio new_xbee(p_serial, p_clock);
  return new_xbee;
}

hal::result<std::span<hal::byte>> xbee_radio::read()
{
  auto data_recieved = HAL_CHECK(m_serial->read(m_xbee_buffer)).data;
  return data_recieved;
}

hal::status xbee_radio::write(std::span<const hal::byte> p_data)
{
  hal::write(*m_serial, p_data);
  return hal::success();
}

hal::status xbee_radio::write(const char* str)
{
  auto length = strlen(str);
  std::span<const hal::byte> span(reinterpret_cast<const hal::byte*>(str),
                                  length);
  write(span);
  return hal::success();
}

hal::status xbee_radio::configure_xbee(const char* p_channel,
                                       const char* p_panid)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  hal::delay(*m_clock, 500ms);
  HAL_CHECK(write("+++"));
  hal::delay(*m_clock, 100ms);
  auto output = HAL_CHECK(m_serial->read(m_xbee_buffer)).data;
  hal::delay(*m_clock, 1000ms);

  if (output[0] != 'O' || output[1] != 'K') {
    return hal::new_error();
  }

  // // Set channel
  HAL_CHECK(write_command("ATCH", p_channel));
  hal::delay(*m_clock, 100ms);
  // Set PAN ID
  HAL_CHECK(write_command("ATID", p_panid));
  hal::delay(*m_clock, 100ms);
  // Save configuration
  HAL_CHECK(write("ATWR\r"));
  hal::delay(*m_clock, 100ms);
  // Exit command mode
  HAL_CHECK(write("ATCN\r"));
  hal::delay(*m_clock, 100ms);
  return hal::success();
}

hal::status xbee_radio::write_command(const char* command, const char* value)
{
  write(command);
  write(value);
  write("\r");
  return hal::success();
}

}  // namespace hal::xbee