#pragma once

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace tft_espi {

class TFT_eSPI : public PollingComponent, public display::DisplayBuffer,
                 public spi::SPIDevice {
 public:
  void setup() override;

  void display();

  void update() override;

  void dump_config() override;

  void set_width(int width) { this->width_ = width; }
  void set_reset_pin(int height) { this->height_ = height; }
  void set_external_vcc(GPIOPin *bl_pin) { this->bl_pin_ = bl_pin; }
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }

  //float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  void fill(int color) override;

 protected:
  void init_reset_();

  void draw_absolute_pixel_internal(int x, int y, int color) override;

  int get_height_internal() override;
  int get_width_internal() override;
  size_t get_buffer_length_();

  bool is_device_msb_first() override;
  bool is_device_high_speed() override;

  int width_{135};
  int height_{240};
  GPIOPin *bl_pin_{nullptr};
  GPIOPin *dc_pin_;
};

}  // namespace tft_espi
}  // namespace esphome
