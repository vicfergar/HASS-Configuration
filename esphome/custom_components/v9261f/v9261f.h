#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace v9261f {

// Default ratios
#define V9261F_CURRENT_FACTOR           2793600.0f
#define V9261F_VOLTAGE_FACTOR           282214.0f
#define V9261F_POWER_FACTOR             329.2083f
#define V9261F_ENERGY_FACTOR            27936.0f
#define V9261F_FREQUENCY_FACTOR         2621.52f

#define HeadValue 0xFE

#define RacBroadcast 0x00
#define RacRead      0x01
#define RacWrite     0x02

// Calibration Registers
#define WARTU   0x0132
#define WARTI   0x012C
#define WARTM   0x012D
#define WAPT    0x012E
#define WWAPT   0x012F
#define EGYTH   0x0181
#define CTH     0x0182
#define BPFPARA 0x0125

// System Control Register
#define SysCtrl 0x0180

// Metering Control Registers
#define MTPARA0 0x0183
#define MTPARA1 0x0184

// Analog Control Registers
#define ANCtrl0 0x0185
#define ANCtrl1 0x0186
#define ANCtrl2 0x0187

// Data Registers
#define AAP     0x0119
#define AAQ     0x011A
#define AARTU   0x011B
#define AARTI   0x011C
#define SAFREQ  0x011D
#define AFREQ   0x011E
#define PEGY    0x01A1
#define NEGY    0x01A2
#define PCFCNT  0x01A3
#define NCFCNT  0x01A4

union DataPacket {
  uint8_t raw[8];
  struct {
    uint8_t head; // 0xFE according to docs.
    uint8_t control;
    uint8_t address;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
    uint8_t checksum;
  };
} __attribute__((packed));

enum States : uint8_t {
  NONE = 0,
  BROADCAST_1,
  BROADCAST_2,
  SETUP,
  READ_TOTALS,
  INIT,
  READY,
};

class V9261F : public PollingComponent, public uart::UARTDevice {
 public:
  void set_voltage_sensor(sensor::Sensor *voltage_sensor) { voltage_sensor_ = voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_energy_sensor(sensor::Sensor *energy_sensor) { energy_sensor_ = energy_sensor; }
  void set_frequency_sensor(sensor::Sensor *frequency_sensor) { frequency_sensor_ = frequency_sensor; }
  void reset_energy_sensor();

  void loop() override;

  void update() override;
  void setup() override;
  void dump_config() override;

 protected:
  sensor::Sensor *voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *energy_sensor_;
  sensor::Sensor *frequency_sensor_;

  uint32_t start_time_ = 0;
  uint32_t wait_time_ = 0;
  States state_ = States::NONE;

  // Divide by this to turn into Watt
  float power_reference_ = V9261F_POWER_FACTOR;
  // Divide by this to turn into Volt
  float voltage_reference_ = V9261F_VOLTAGE_FACTOR;
  // Divide by this to turn into Ampere
  float current_reference_ = V9261F_CURRENT_FACTOR;
  // Divide by this to turn into kWh
  float energy_reference_ = V9261F_ENERGY_FACTOR;
  // Divide by this to turn into Herz
  float frequency_reference_ = V9261F_FREQUENCY_FACTOR;

  static void fill_data_packet(DataPacket *frame, uint8_t rac, uint16_t address, uint32_t data);
  static uint8_t calculate_checksum(const DataPacket *frame, uint8_t offset = 0x00);
  static bool validate_checksum(const DataPacket *frame);
  static bool validate_checksum(uint8_t actual, uint8_t expected);

  void write_broadcast_reg(uint16_t address, uint32_t data);
  void write_reg(uint16_t address, uint32_t data);
  uint32_t read_reg(uint16_t address);
  bool read_reg(uint16_t address, uint32_t* buffer, uint8_t length);
};
}  // namespace v9261f
}  // namespace esphome
