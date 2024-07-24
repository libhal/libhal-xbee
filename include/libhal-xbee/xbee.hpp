// Copyright 2024 Khalil Estell
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <array>

#include <libhal-util/steady_clock.hpp>
#include <libhal/functional.hpp>
#include <libhal/serial.hpp>
#include <libhal/steady_clock.hpp>

namespace hal::xbee {
class xbee_radio
{
public:
  /**
   * @brief Construct a new xbee radio object
   *
   * @param p_serial - serial port connected to xbee radio device
   * @param p_clock - clock used for timing configuring the xbee
   */
  xbee_radio(hal::serial& p_serial, hal::steady_clock& p_clock);

  std::span<hal::byte> read(std::span<hal::byte> p_buffer);
  void write(std::span<const hal::byte> p_data);
  void configure(std::string_view p_channel, std::string_view p_panid);

private:
  hal::serial* m_serial;
  hal::steady_clock* m_clock;
};

}  // namespace hal::xbee