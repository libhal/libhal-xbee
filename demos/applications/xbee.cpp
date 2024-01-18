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

#include <libhal-xbee/xbee.hpp>

#include <libhal-util/serial.hpp>
#include <libhal-util/steady_clock.hpp>

#include "../hardware_map.hpp"

hal::status application(hardware_map& p_map)
{
  using namespace std::chrono_literals;
  using namespace hal::literals;

  auto& clock = *p_map.clock;
  auto& console = *p_map.console;
  auto& xbee = *p_map.xbee;

  hal::print(console, "Initializing XBEE Radio...\n");
  auto xbee_module = HAL_CHECK(hal::xbee::xbee_radio::create(xbee, clock));
  hal::print(console, "XBEE Radio created! \n");

  HAL_CHECK(xbee_module.configure_xbee("C", "2015"));  // Channel C, PANID 2015
  hal::print(console, "Configured XBEE Radio \n");
  hal::delay(clock, 500ms);

  hal::print(console, "Demo Application Starting...\n\n");

  while (true) {
    hal::delay(clock, 1000ms);
    auto recieved_data = HAL_CHECK(xbee_module.read());
    hal::print(console,
               "\n=================== RECIEVED DATA ===================\n");
    hal::print(console, recieved_data);

    std::string_view message = "Hello from the other side";
    xbee_module.write(hal::as_bytes(message));
    hal::print(console,
               "\n=================== TRANSMITTED DATA ===================\n");
    hal::print(console, message);
  }

  return hal::success();
}
