#include "libhal-xbee/xbee.hpp"

#include <libhal/timeout.hpp>
#include <span>

#include <libhal-util/serial.hpp>
#include <libhal-util/streams.hpp>
#include <libhal/serial.hpp>

namespace hal::xbee {

xbee_radio::xbee_radio(hal::serial& p_serial, hal::steady_clock& p_clock)
  : m_serial(&p_serial)
  , m_clock(&p_clock)
{
}

std::span<hal::byte> xbee_radio::read(std::span<hal::byte> p_buffer)
{
  return m_serial->read(p_buffer).data;
}

void xbee_radio::write(std::span<const hal::byte> p_data)
{
  m_serial->write(p_data);
}

void xbee_radio::configure(std::string_view p_channel, std::string_view p_panid)
{

  using namespace std::chrono_literals;
  using namespace hal::literals;

  std::array<hal::byte, 32> buffer{};

  hal::delay(*m_clock, 500ms);
  hal::write(*m_serial, "+++", hal::never_timeout());
  hal::delay(*m_clock, 100ms);
  auto output = m_serial->read(buffer).data;
  hal::delay(*m_clock, 1000ms);

  int retry_count = 0;
  const int max_retries = 5;  // Define a maximum number of retries

  while (retry_count < max_retries) {
    // Try to enter command mode
    hal::write(*m_serial, "+++", hal::never_timeout());
    hal::delay(*m_clock, 100ms);
    output = (m_serial->read(buffer)).data;
    hal::delay(*m_clock, 1000ms);

    if (output[0] == 'O' && output[1] == 'K') {
      hal::print(*m_serial, "Radio Success\r");
      break;
    } else if (output[0] == 'E' && output[1] == 'R' && output[2] == 'R') {
      hal::print(*m_serial, "Error Occurred\r");
      retry_count++;
      hal::delay(*m_clock, 2000ms);  // Optional delay before retrying
    } else {
      // TODO: determine what should go here.
      return hal::new_error();
    }
  }

  if (retry_count == max_retries) {
    hal::print(*m_serial,
               "Failed to enter command mode after multiple attempts\r");
    // Handle the failure here, perhaps by returning an error or taking other
    // actions
  }

  auto write_command = [this](std::string_view command,
                              std::string_view value) {
    hal::write(*m_serial, command, hal::never_timeout());
    hal::write(*m_serial, value, hal::never_timeout());
    hal::write(*m_serial, "\r", hal::never_timeout());
  };

  // Set channel
  write_command("ATCH", p_channel);
  hal::delay(*m_clock, 100ms);

  // Set PAN ID
  write_command("ATID", p_panid);
  hal::delay(*m_clock, 100ms);

  // Set Baud Rate to 115200
  hal::write(*m_serial, "ATBD7", hal::never_timeout());
  hal::delay(*m_clock, 100ms);

  // Save configuration
  hal::write(*m_serial, "ATWR\r", hal::never_timeout());
  hal::delay(*m_clock, 100ms);

  // Exit command mode
  hal::write(*m_serial, "ATCN\r", hal::never_timeout());
  hal::delay(*m_clock, 100ms);
}
}  // namespace hal::xbee