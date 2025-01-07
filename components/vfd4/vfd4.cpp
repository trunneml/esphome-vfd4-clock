#include "vfd4.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace vfd4 {

static const char *const TAG = "display.vfd4";
static const uint8_t VFD4_SHIFT_DELAY = 1;  // clock pause between commands, should be bigger then 300ns

//
//      A
//     ---
//  F |   | B      * X
//     -G-
//  E |   | C      * Y
//     ---
//      D   
// YABCDEFG
const uint8_t VFD4_UNKNOWN_CHAR = 0b00000000;
const uint8_t VFD4_ASCII_TO_RAW[] PROGMEM = {
    0b00000000,           // ' ', ord 0x20
    0b10110000,           // '!', ord 0x21
    0b00100010,           // '"', ord 0x22
    VFD4_UNKNOWN_CHAR,    // '#', ord 0x23
    VFD4_UNKNOWN_CHAR,    // '$', ord 0x24
    0b01001001,           // '%', ord 0x25
    VFD4_UNKNOWN_CHAR,    // '&', ord 0x26
    0b00000010,           // ''', ord 0x27
    0b01001110,           // '(', ord 0x28
    0b01111000,           // ')', ord 0x29
    0b01000000,           // '*', ord 0x2A
    VFD4_UNKNOWN_CHAR,    // '+', ord 0x2B
    0b00010000,           // ',', ord 0x2C
    0b00000001,           // '-', ord 0x2D
    0b00001000,           // '.', ord 0x2E
    0b00100101,           // '/', ord 0x2F
    0b01111110,           // '0', ord 0x30
    0b00110000,           // '1', ord 0x31
    0b01101101,           // '2', ord 0x32
    0b01111001,           // '3', ord 0x33
    0b00110011,           // '4', ord 0x34
    0b01011011,           // '5', ord 0x35
    0b01011111,           // '6', ord 0x36
    0b01110000,           // '7', ord 0x37
    0b01111111,           // '8', ord 0x38
    0b01111011,           // '9', ord 0x39
    0b01001000,           // ':', ord 0x3A
    0b01011000,           // ';', ord 0x3B
    0b00001100,           // '<', ord 0x3C
    0b00001001,           // '=', ord 0x3D
    0b00011000,           // '>', ord 0x3E
    0b01100101,           // '?', ord 0x3F
    0b01101111,           // '@', ord 0x40
    0b01110111,           // 'A', ord 0x41
    0b00011111,           // 'B', ord 0x42
    0b01001110,           // 'C', ord 0x43
    0b00111101,           // 'D', ord 0x44
    0b01001111,           // 'E', ord 0x45
    0b01000111,           // 'F', ord 0x46
    0b01011110,           // 'G', ord 0x47
    0b00110111,           // 'H', ord 0x48
    0b00110000,           // 'I', ord 0x49
    0b00111100,           // 'J', ord 0x4A
    VFD4_UNKNOWN_CHAR,    // 'K', ord 0x4B
    0b00001110,           // 'L', ord 0x4C
    VFD4_UNKNOWN_CHAR,    // 'M', ord 0x4D
    0b00010101,           // 'N', ord 0x4E
    0b01111110,           // 'O', ord 0x4F
    0b01100111,           // 'P', ord 0x50
    0b01111110,           // 'Q', ord 0x51
    0b00000101,           // 'R', ord 0x52
    0b01011011,           // 'S', ord 0x53
    0b00000111,           // 'T', ord 0x54
    0b00111110,           // 'U', ord 0x55
    0b00111110,           // 'V', ord 0x56
    0b00111111,           // 'W', ord 0x57
    0b00110111,           // 'X', ord 0x58
    0b00100111,           // 'Y', ord 0x59
    0b01101101,           // 'Z', ord 0x5A
    0b01001110,           // '[', ord 0x5B
    VFD4_UNKNOWN_CHAR,    // '\', ord 0x5C
    0b01111000,           // ']', ord 0x5D
    0b01000000,           // '^', ord 0x5E
    0b00001000,           // '_', ord 0x5F
    0b00100000,           // '`', ord 0x60
    0b01110111,           // 'a', ord 0x61
    0b00011111,           // 'b', ord 0x62
    0b00001101,           // 'c', ord 0x63
    0b00111101,           // 'd', ord 0x64
    0b01001111,           // 'e', ord 0x65
    0b01000111,           // 'f', ord 0x66
    0b01011110,           // 'g', ord 0x67
    0b00010111,           // 'h', ord 0x68
    0b00010000,           // 'i', ord 0x69
    0b00111100,           // 'j', ord 0x6A
    VFD4_UNKNOWN_CHAR,    // 'k', ord 0x6B
    0b00001110,           // 'l', ord 0x6C
    VFD4_UNKNOWN_CHAR,    // 'm', ord 0x6D
    0b00010101,           // 'n', ord 0x6E
    0b00011101,           // 'o', ord 0x6F
    0b01100111,           // 'p', ord 0x70
    VFD4_UNKNOWN_CHAR,    // 'q', ord 0x71
    0b00000101,           // 'r', ord 0x72
    0b01011011,           // 's', ord 0x73
    0b00000111,           // 't', ord 0x74
    0b00011100,           // 'u', ord 0x75
    0b00011100,           // 'v', ord 0x76
    VFD4_UNKNOWN_CHAR,  // 'w', ord 0x77
    VFD4_UNKNOWN_CHAR,  // 'x', ord 0x78
    0b00100111,           // 'y', ord 0x79
    VFD4_UNKNOWN_CHAR,  // 'z', ord 0x7A
    0b00110001,           // '{', ord 0x7B
    0b00000110,           // '|', ord 0x7C
    0b00000111,           // '}', ord 0x7D
    0b01100011,           // '~', ord 0x7E (degree symbol)
};

const uint8_t VFD4_ASCII_TO_DOT[] PROGMEM = {
    0b00000000,           // ' ', ord 0x20
    0b00000011,           // '!', ord 0x21
    0b00000010,           // '"', ord 0x22
    VFD4_UNKNOWN_CHAR,    // '#', ord 0x23
    VFD4_UNKNOWN_CHAR,    // '$', ord 0x24
    0b00000011,           // '%', ord 0x25
    VFD4_UNKNOWN_CHAR,    // '&', ord 0x26
    0b00000010,           // ''', ord 0x27
    0b00000011,           // '(', ord 0x28
    0b00000011,           // ')', ord 0x29
    0b00000010,           // '*', ord 0x2A
    0b00000010,           // '+', ord 0x2B
    0b00000001,           // ',', ord 0x2C
    0b00000010,           // '-', ord 0x2D
    0b00000001,           // '.', ord 0x2E
    0b00000011,           // '/', ord 0x2F
    VFD4_UNKNOWN_CHAR,    // '0', ord 0x30
    VFD4_UNKNOWN_CHAR,    // '1', ord 0x31
    VFD4_UNKNOWN_CHAR,    // '2', ord 0x32
    VFD4_UNKNOWN_CHAR,    // '3', ord 0x33
    VFD4_UNKNOWN_CHAR,    // '4', ord 0x34
    VFD4_UNKNOWN_CHAR,    // '5', ord 0x35
    VFD4_UNKNOWN_CHAR,    // '6', ord 0x36
    VFD4_UNKNOWN_CHAR,    // '7', ord 0x37
    VFD4_UNKNOWN_CHAR,    // '8', ord 0x38
    VFD4_UNKNOWN_CHAR,    // '9', ord 0x39
    0b00000011,           // ':', ord 0x3A
    0b00000011,           // ';', ord 0x3B
    VFD4_UNKNOWN_CHAR,    // '<', ord 0x3C
    0b00000011,           // '=', ord 0x3D
    VFD4_UNKNOWN_CHAR,    // '>', ord 0x3E
    VFD4_UNKNOWN_CHAR,    // '?', ord 0x3F
    VFD4_UNKNOWN_CHAR,    // '@', ord 0x40
    VFD4_UNKNOWN_CHAR,    // 'A', ord 0x41
    VFD4_UNKNOWN_CHAR,    // 'B', ord 0x42
    VFD4_UNKNOWN_CHAR,    // 'C', ord 0x43
    VFD4_UNKNOWN_CHAR,    // 'D', ord 0x44
    VFD4_UNKNOWN_CHAR,    // 'E', ord 0x45
    VFD4_UNKNOWN_CHAR,    // 'F', ord 0x46
    VFD4_UNKNOWN_CHAR,    // 'G', ord 0x47
    VFD4_UNKNOWN_CHAR,    // 'H', ord 0x48
    VFD4_UNKNOWN_CHAR,    // 'I', ord 0x49
    VFD4_UNKNOWN_CHAR,    // 'J', ord 0x4A
    VFD4_UNKNOWN_CHAR,    // 'K', ord 0x4B
    VFD4_UNKNOWN_CHAR,    // 'L', ord 0x4C
    VFD4_UNKNOWN_CHAR,    // 'M', ord 0x4D
    VFD4_UNKNOWN_CHAR,    // 'N', ord 0x4E
    VFD4_UNKNOWN_CHAR,    // 'O', ord 0x4F
    VFD4_UNKNOWN_CHAR,    // 'P', ord 0x50
    VFD4_UNKNOWN_CHAR,    // 'Q', ord 0x51
    VFD4_UNKNOWN_CHAR,    // 'R', ord 0x52
    VFD4_UNKNOWN_CHAR,    // 'S', ord 0x53
    VFD4_UNKNOWN_CHAR,    // 'T', ord 0x54
    VFD4_UNKNOWN_CHAR,    // 'U', ord 0x55
    VFD4_UNKNOWN_CHAR,    // 'V', ord 0x56
    VFD4_UNKNOWN_CHAR,    // 'W', ord 0x57
    VFD4_UNKNOWN_CHAR,    // 'X', ord 0x58
    VFD4_UNKNOWN_CHAR,    // 'Y', ord 0x59
    VFD4_UNKNOWN_CHAR,    // 'Z', ord 0x5A
    VFD4_UNKNOWN_CHAR,    // '[', ord 0x5B
    VFD4_UNKNOWN_CHAR,    // '\', ord 0x5C
    0b00000011,           // ']', ord 0x5D
    0b00000010,           // '^', ord 0x5E
    0b00000001,           // '_', ord 0x5F
    0b00000010,           // '`', ord 0x60
    VFD4_UNKNOWN_CHAR,    // 'a', ord 0x61
    VFD4_UNKNOWN_CHAR,    // 'b', ord 0x62
    VFD4_UNKNOWN_CHAR,    // 'c', ord 0x63
    VFD4_UNKNOWN_CHAR,    // 'd', ord 0x64
    VFD4_UNKNOWN_CHAR,    // 'e', ord 0x65
    VFD4_UNKNOWN_CHAR,    // 'f', ord 0x66
    VFD4_UNKNOWN_CHAR,    // 'g', ord 0x67
    VFD4_UNKNOWN_CHAR,    // 'h', ord 0x68
    VFD4_UNKNOWN_CHAR,    // 'i', ord 0x69
    VFD4_UNKNOWN_CHAR,    // 'j', ord 0x6A
    VFD4_UNKNOWN_CHAR,    // 'k', ord 0x6B
    VFD4_UNKNOWN_CHAR,    // 'l', ord 0x6C
    VFD4_UNKNOWN_CHAR,    // 'm', ord 0x6D
    VFD4_UNKNOWN_CHAR,    // 'n', ord 0x6E
    VFD4_UNKNOWN_CHAR,    // 'o', ord 0x6F
    VFD4_UNKNOWN_CHAR,    // 'p', ord 0x70
    VFD4_UNKNOWN_CHAR,    // 'q', ord 0x71
    VFD4_UNKNOWN_CHAR,    // 'r', ord 0x72
    VFD4_UNKNOWN_CHAR,    // 's', ord 0x73
    VFD4_UNKNOWN_CHAR,    // 't', ord 0x74
    VFD4_UNKNOWN_CHAR,    // 'u', ord 0x75
    VFD4_UNKNOWN_CHAR,    // 'v', ord 0x76
    VFD4_UNKNOWN_CHAR,    // 'w', ord 0x77
    VFD4_UNKNOWN_CHAR,    // 'x', ord 0x78
    VFD4_UNKNOWN_CHAR,    // 'y', ord 0x79
    VFD4_UNKNOWN_CHAR,    // 'z', ord 0x7A
    0b00000011,           // '{', ord 0x7B
    0b00000011,           // '|', ord 0x7C
    0b00000011,           // '}', ord 0x7D
    0b00000010,           // '~', ord 0x7E (degree symbol)
};
void VFD4Display::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VFD4...");

  this->clk_pin_->setup();               // OUTPUT
  this->clk_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->clk_pin_->digital_write(false);  // LOW
  this->data_pin_->setup();               // OUTPUT
  this->data_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->data_pin_->digital_write(false);  // LOW
  this->stb_pin_->setup();               // OUTPUT
  this->stb_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->stb_pin_->digital_write(false);  // LOW

  for (uint8_t i = 0; i < 4; i++)  // zero fill print buffer
    this->buffer_[i] = 0;

  this->current_seg_ = 0;
  // this->display();
}
void VFD4Display::dump_config() {
  ESP_LOGCONFIG(TAG, "VFD4:");
  ESP_LOGCONFIG(TAG, "  Intensity: %d", this->intensity_);
  LOG_PIN("  CLK Pin: ", this->clk_pin_);
  LOG_PIN("  DATA Pin: ", this->data_pin_);
  LOG_PIN("  STB Pin: ", this->stb_pin_);
  LOG_UPDATE_INTERVAL(this);
}

void VFD4Display::update() {
  // for (uint8_t &i : this->buffer_)
  //   i = 0;
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}

float VFD4Display::get_setup_priority() const { return setup_priority::PROCESSOR; }
void VFD4Display::bit_delay_() { delayMicroseconds(VFD4_SHIFT_DELAY); }

void VFD4Display::display() {
  for (uint8_t i = 0; i < 5; i++) {
    this->set_7seg_(i, buffer_[i]);
    delayMicroseconds((i == 0) || (i == 4) ? 5000 : 2000);
  }
  this->set_7seg_(6, 0);
}

void VFD4Display::set_7seg_(int seg_pos, uint8_t seg_bits) {
  ESP_LOGV(TAG, "Print at %d", seg_pos);
  this->stb_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->stb_pin_->digital_write(false);
  this->shift_out_(seg_pos == 0); // 22   seg 4
  this->shift_out_(seg_pos == 1); // 21  seg 3
  this->shift_out_(seg_pos == 2); // 20     :
  this->shift_out_(seg_pos == 3); // 19  seg 2
  this->shift_out_(seg_pos == 4); // 18  seg 1
  if (seg_pos == 2) {
    for (uint8_t i= 0; i < 2; i++) {
      this->shift_out_((seg_bits & (1 << i)));
    }
    for (uint8_t i= 0; i < 7; i++) {
      this->shift_out_(false);
    }
  } else {
    this->shift_out_(false); // X
    this->shift_out_(false); // Y
    for (uint8_t i= 0; i < 7; i++) {
      this->shift_out_((seg_bits & (1 << i)));
    }
  }

  this->stb_pin_->digital_write(true);
  this->bit_delay_();
  this->stb_pin_->digital_write(false);
}

void VFD4Display::shift_out_(bool val) {
  this->data_pin_->digital_write(val);
  this->bit_delay_();

  this->clk_pin_->digital_write(true);
  this->bit_delay_();

  this->clk_pin_->digital_write(false);
  this->bit_delay_();
}

uint8_t VFD4Display::print(uint8_t start_pos, const char *str) {
  ESP_LOGV(TAG, "Print at %d: %s", start_pos, str);
  uint8_t pos = start_pos;
  for (; *str != '\0'; str++) {
    if (pos >= 5) {
      ESP_LOGE(TAG, "String is too long for the display!");
      break;
    }
    uint8_t data = VFD4_UNKNOWN_CHAR;
    if (pos != 2) {
      if (*str >= ' ' && *str <= '~')
      data = progmem_read_byte(&VFD4_ASCII_TO_RAW[*str - ' ']);

      if (data == VFD4_UNKNOWN_CHAR) {
        ESP_LOGW(TAG, "Encountered character '%c' with no VFD4 representation while translating string!", *str);
      }
      // Remap segments, for compatibility with MAX7219 segment definition which is
      // XABCDEFG, but VFD4 is upside down
      // Resulting_DEFABCG

      // XABCDEFG > _DEFABCG
      data = 0 |  // no X
              ((data & 0x40) ? 0x8 : 0) |   // A 
              ((data & 0x20) ? 0x4 : 0) |   // B
              ((data & 0x10) ? 0x2 : 0) |   // C
              ((data & 0x8) ? 0x40 : 0) |   // D
              ((data & 0x4) ? 0x20 : 0) |   // E
              ((data & 0x2) ? 0x10 : 0) |   // F
              ((data & 0x1) ? 0x1 : 0);     // G
      // data = (data & 0b01111111);
      if (*str == '1' && pos > 0 && this->buffer_[pos-1] == 0b00000001) {
        this->buffer_[pos-1] = data | 0b00000001;
      } else {
        this->buffer_[pos++] = data;
      }
    } else {
      if (*str >= ' ' && *str <= '~')
      data = progmem_read_byte(&VFD4_ASCII_TO_DOT[*str - ' ']);

      if (data == VFD4_UNKNOWN_CHAR) {
        ESP_LOGW(TAG, "Encountered character '%c' with no VFD4 representation while translating string!", *str);
      }
      // XY > YX
      data = ((data & 0x2) ? 0x1 : 0) |   // X
             ((data & 0x1) ? 0x2 : 0);    // Y
      this->buffer_[pos++] = data;
    }
  }
  return pos - start_pos;
}
uint8_t VFD4Display::print(const char *str) { return this->print(0, str); }
uint8_t VFD4Display::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t VFD4Display::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}

uint8_t VFD4Display::strftime(uint8_t pos, const char *format, ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t VFD4Display::strftime(const char *format, ESPTime time) { return this->strftime(0, format, time); }

}  // namespace vfd4
}  // namespace esphome
