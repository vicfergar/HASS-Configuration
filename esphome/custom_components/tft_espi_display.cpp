#include "tft_espi_display.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tft_espi {

static const char *TAG = "tft_espi";

//TFT_eSPI tft;

/*static const uint8_t SSD1306_COMMAND_DISPLAY_OFF = 0xAE;
static const uint8_t SSD1306_COMMAND_DISPLAY_ON = 0xAF;
static const uint8_t SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV = 0xD5;
static const uint8_t SSD1306_COMMAND_SET_MULTIPLEX = 0xA8;
static const uint8_t SSD1306_COMMAND_SET_DISPLAY_OFFSET = 0xD3;
static const uint8_t SSD1306_COMMAND_SET_START_LINE = 0x40;
static const uint8_t SSD1306_COMMAND_CHARGE_PUMP = 0x8D;
static const uint8_t SSD1306_COMMAND_MEMORY_MODE = 0x20;
static const uint8_t SSD1306_COMMAND_SEGRE_MAP = 0xA0;
static const uint8_t SSD1306_COMMAND_COM_SCAN_INC = 0xC0;
static const uint8_t SSD1306_COMMAND_COM_SCAN_DEC = 0xC8;
static const uint8_t SSD1306_COMMAND_SET_COM_PINS = 0xDA;
static const uint8_t SSD1306_COMMAND_SET_CONTRAST = 0x81;
static const uint8_t SSD1306_COMMAND_SET_PRE_CHARGE = 0xD9;
static const uint8_t SSD1306_COMMAND_SET_VCOM_DETECT = 0xDB;
static const uint8_t SSD1306_COMMAND_DISPLAY_ALL_ON_RESUME = 0xA4;
static const uint8_t SSD1306_COMMAND_DEACTIVATE_SCROLL = 0x2E;
static const uint8_t SSD1306_COMMAND_COLUMN_ADDRESS = 0x21;
static const uint8_t SSD1306_COMMAND_PAGE_ADDRESS = 0x22;

static const uint8_t SSD1306_NORMAL_DISPLAY = 0xA6;*/

void TFT_eSPI::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFT_eSPI...");
  //tft = new TFT_eSPI(width_, height_);
  //this->spi_setup();
  //this->dc_pin_->setup();  // OUTPUT
  //this->dc_pin_->setup();  // OUTPUT
  //this->dc_pin_->setup();  // OUTPUT
  //this->dc_pin_->setup();  // OUTPUT
  //
  //this->init_reset_();
  //this->init_internal_(this->get_buffer_length_());*/

  /*this->command(SSD1306_COMMAND_DISPLAY_OFF);
  this->command(SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV);
  this->command(0x80);  // suggested ratio

  this->command(SSD1306_COMMAND_SET_MULTIPLEX);
  this->command(this->get_height_internal() - 1);

  this->command(SSD1306_COMMAND_SET_DISPLAY_OFFSET);
  this->command(0x00);                                   // no offset
  this->command(SSD1306_COMMAND_SET_START_LINE | 0x00);  // start at line 0
  this->command(SSD1306_COMMAND_CHARGE_PUMP);
  if (this->external_vcc_)
    this->command(0x10);
  else
    this->command(0x14);

  this->command(SSD1306_COMMAND_MEMORY_MODE);
  this->command(0x00);
  this->command(SSD1306_COMMAND_SEGRE_MAP | 0x01);
  this->command(SSD1306_COMMAND_COM_SCAN_DEC);

  this->command(SSD1306_COMMAND_SET_COM_PINS);
  switch (this->model_) {
    case SSD1306_MODEL_128_32:
    case SH1106_MODEL_128_32:
    case SSD1306_MODEL_96_16:
    case SH1106_MODEL_96_16:
      this->command(0x02);
      break;
    case SSD1306_MODEL_128_64:
    case SH1106_MODEL_128_64:
    case SSD1306_MODEL_64_48:
    case SH1106_MODEL_64_48:
      this->command(0x12);
      break;
  }

  this->command(SSD1306_COMMAND_SET_CONTRAST);
  switch (this->model_) {
    case SSD1306_MODEL_128_32:
    case SH1106_MODEL_128_32:
      this->command(0x8F);
      break;
    case SSD1306_MODEL_128_64:
    case SH1106_MODEL_128_64:
    case SSD1306_MODEL_64_48:
    case SH1106_MODEL_64_48:
      if (this->external_vcc_)
        this->command(0x9F);
      else
        this->command(0xCF);
      break;
    case SSD1306_MODEL_96_16:
    case SH1106_MODEL_96_16:
      if (this->external_vcc_)
        this->command(0x10);
      else
        this->command(0xAF);
      break;
  }

  this->command(SSD1306_COMMAND_SET_PRE_CHARGE);
  if (this->external_vcc_)
    this->command(0x22);
  else
    this->command(0xF1);

  this->command(SSD1306_COMMAND_SET_VCOM_DETECT);
  this->command(0x40);

  this->command(SSD1306_COMMAND_DISPLAY_ALL_ON_RESUME);
  this->command(SSD1306_NORMAL_DISPLAY);

  this->command(SSD1306_COMMAND_DEACTIVATE_SCROLL);

  this->command(SSD1306_COMMAND_DISPLAY_ON);*/
}
void TFT_eSPI::display() {
  /*if (this->is_sh1106_()) {
    this->write_display_data();
    return;
  }

  this->command(SSD1306_COMMAND_COLUMN_ADDRESS);
  switch (this->model_) {
    case SSD1306_MODEL_64_48:
      this->command(0x20);
      this->command(0x20 + this->get_width_internal() - 1);
      break;
    default:
      this->command(0);  // Page start address, 0
      this->command(this->get_width_internal() - 1);
      break;
  }

  this->command(SSD1306_COMMAND_PAGE_ADDRESS);
  // Page start address, 0
  this->command(0);
  // Page end address:
  this->command((this->get_height_internal() / 8) - 1);

  this->write_display_data();*/
}
void TFT_eSPI::update() {
  this->do_update_();
  this->display();
}
int TFT_eSPI::get_height_internal() {
  return height_;
}
int TFT_eSPI::get_width_internal() {
  return width_;
}
size_t TFT_eSPI::get_buffer_length_() {
  return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) / 8u;
}

void HOT TFT_eSPI::draw_absolute_pixel_internal(int x, int y, int color) {
  /*if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  uint16_t pos = x + (y / 8) * this->get_width_internal();
  uint8_t subpos = y & 0x07;
  if (color) {
    this->buffer_[pos] |= (1 << subpos);
  } else {
    this->buffer_[pos] &= ~(1 << subpos);
  }*/
}
void TFT_eSPI::fill(int color) {
  /*uint8_t fill = color ? 0xFF : 0x00;
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
    this->buffer_[i] = fill;*/
}
void TFT_eSPI::init_reset_() {
  /*if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(1);
    // Trigger Reset
    this->reset_pin_->digital_write(false);
    delay(10);
    // Wake up
    this->reset_pin_->digital_write(true);
  }*/
}
void TFT_eSPI::dump_config() {
  /*LOG_DISPLAY("", "SPI SSD1306", this);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_UPDATE_INTERVAL(this);*/
}
bool TFT_eSPI::is_device_msb_first() { return true; }
bool TFT_eSPI::is_device_high_speed() { return true; }

}  // namespace ssd1306_base
}  // namespace esphome
