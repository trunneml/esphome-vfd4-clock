#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/time.h"

#include <vector>

namespace esphome {
namespace vfd4 {

class VFD4Display;

using vfd4_writer_t = std::function<void(VFD4Display &)>;

class VFD4Display : public PollingComponent {
 public:
  void set_writer(vfd4_writer_t &&writer) { this->writer_ = writer; }
  void setup() override;
  void dump_config() override;
  void update() override;
  void set_clk_pin(GPIOPin *pin) { this->clk_pin_ = pin; }
  void set_data_pin(GPIOPin *pin) { this->data_pin_ = pin; }
  void set_stb_pin(GPIOPin *pin) { this->stb_pin_ = pin; }

  float get_setup_priority() const override;


  /// Evaluate the printf-format and print the result at the given position.
  uint8_t printf(uint8_t pos, const char *format, ...) __attribute__((format(printf, 3, 4)));
  /// Evaluate the printf-format and print the result at position 0.
  uint8_t printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

  /// Print `str` at the given position.
  uint8_t print(uint8_t pos, const char *str);
  /// Print `str` at position 0.
  uint8_t print(const char *str);

  void set_intensity(uint8_t intensity) { this->intensity_ = intensity; }
  void set_on(bool on) { this->on_ = on; }

  void display();

  /// Evaluate the strftime-format and print the result at the given position.
  uint8_t strftime(uint8_t pos, const char *format, ESPTime time) __attribute__((format(strftime, 3, 0)));
  /// Evaluate the strftime-format and print the result at position 0.
  uint8_t strftime(const char *format, ESPTime time) __attribute__((format(strftime, 2, 0)));

 protected:
  void bit_delay_();
  void set_7seg_(int seg_pos, uint8_t seg_bits);
  void shift_out_(bool val);

  uint8_t intensity_;
  GPIOPin *clk_pin_;
  GPIOPin *stb_pin_;
  GPIOPin *data_pin_;

  bool on_{true};
  optional<vfd4_writer_t> writer_{};
  uint8_t buffer_[5] = {0};
  uint8_t current_seg_{0};
};

}  // namespace vfd4
}  // namespace esphome
