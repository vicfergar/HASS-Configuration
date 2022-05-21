#include "v9261f.h"
#include "esphome/core/log.h"

namespace esphome {
namespace v9261f {

static const char *const TAG = "v9261f";

static DataPacket request;
static DataPacket response;

void V9261F::write_broadcast_reg(uint16_t address, uint32_t data) {
  fill_data_packet(&request, RacBroadcast, address, data);
  write_array(request.raw, sizeof(request.raw));
}

void V9261F::write_reg(uint16_t address, uint32_t data) {
  fill_data_packet(&request, RacWrite, address, data);
  write_array(request.raw, sizeof(request.raw));

  response = {};
  if (read_array(response.raw, 4)) {
    response.checksum = response.data0;
    response.data0 = 0;
    if (validate_checksum(&response)) {
      if(response.head != request.head) {
        ESP_LOGW(TAG, "Invalid response. Header mismatch: %d", response.head);
      } else if(response.control != request.control) {
        ESP_LOGW(TAG, "Invalid response. Control mismatch: 0x%02X != 0x%02X", response.control, request.control);
      } else if(response.address != request.address) {
        ESP_LOGW(TAG, "Invalid response. Address mismatch: 0x%02X != 0x%02X", response.address, request.address);
      }
    }
  } else {
    ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
    while (read() >= 0)
      ;
  }
}

uint32_t V9261F::read_reg(uint16_t address) {
  uint32_t result;
  if (!read_reg(address, &result, 1))
  {
    return 0;
  }

  return result;
}

bool V9261F::read_reg(uint16_t address, uint32_t* buffer, uint8_t length)
{
  fill_data_packet(&request, RacRead, address, length);
  write_array(request.raw, sizeof(request.raw));

  uint8_t* byteBuffer = (uint8_t *)buffer;
  response = {};
  if (read_array(response.raw, 3) &&
      read_array(byteBuffer, 4 * length) &&
      read_byte(&response.checksum)) {
    uint8_t checksum = 0x00;
    for (uint32_t i = 0; i < 4 * length; i++) {
      checksum += byteBuffer[i];
    }

    checksum = calculate_checksum(&response, checksum);
    if (validate_checksum(checksum, response.checksum)) {
      if(response.head != request.head) {
        ESP_LOGW(TAG, "Invalid response. Header mismatch: %d", response.head);
      } else if(response.control != request.control) {
        ESP_LOGW(TAG, "Invalid response. Control mismatch: 0x%02X != 0x%02X", response.control, request.control);
      } else if(response.address != length) {
        ESP_LOGW(TAG, "Invalid response. Length mismatch: 0x%02X != 0x%02X", response.address, length);
      }
      else
      {
         return true;
      }
    }
  } else {
    ESP_LOGW(TAG, "Junk on wire. Throwing away partial message");
    while (read() >= 0)
      ;
  }

  return false;
}

void V9261F::fill_data_packet(DataPacket *frame, uint8_t rac, uint16_t address, uint32_t data) {
  frame->head = HeadValue;
  frame->control = (uint8_t)((address & 0x0F00)>>4) + rac;
  frame->address = (uint8_t)address;
  frame->data0 = (uint8_t)(data);
  frame->data1 = (uint8_t)(data>>8);
  frame->data2 = (uint8_t)(data>>16);
  frame->data3 = (uint8_t)(data>>24);
  frame->checksum = calculate_checksum(frame);
}

uint8_t V9261F::calculate_checksum(const DataPacket *frame, uint8_t offset) {
  uint8_t checksum = offset;
  // Whole package but checksum
  for (uint32_t i = 0; i < sizeof(frame->raw) - 1; i++) {
    checksum += frame->raw[i];
  }

  checksum = (~checksum) + 0x33;

  return checksum;
}

bool V9261F::validate_checksum(const DataPacket *frame) {
  uint8_t checksum = calculate_checksum(frame);
  return validate_checksum(checksum, frame->checksum);
}

bool V9261F::validate_checksum(uint8_t actual, uint8_t expected) {
  if (actual != expected) {
    ESP_LOGW(TAG, "Invalid checksum! 0x%02X != 0x%02X", actual, expected);
  }

  return actual == expected;
}

void V9261F::loop() {
  if (this->state_ >= States::READY ||
      millis() - this->start_time_ < this->wait_time_)
    return;

  if (this->state_ == States::BROADCAST_1) {
    this->write_broadcast_reg(MTPARA0, 0xE0038002);
    this->wait_time_ = 20;
  }
  else if (this->state_ == States::BROADCAST_2) {
    this->write_broadcast_reg(SysCtrl, 0x01FF0008);
    this->wait_time_ = 20;
  }
  else if (this->state_ == States::SETUP) {
    this->write_reg(ANCtrl0, 0x1F0004D3);
    this->write_reg(ANCtrl1, 0x20000000);
    this->write_reg(ANCtrl2, 0x0000000F);
    this->write_reg(SysCtrl, 0x00000000);
    this->write_reg(BPFPARA, 0x811D2BA7);
    this->write_reg(EGYTH,   0x067215D8);
    this->write_reg(CTH,     0x00000000);
    this->write_reg(MTPARA0, 0xE0000000);
    this->write_reg(MTPARA1, 0x0A0B0900);
    this->write_reg(WARTU,   0x00000000);
    this->write_reg(WARTI,   0x00000000);
    this->write_reg(WARTM,   0x00000000);
    this->write_reg(WAPT,    0x00000000);
    this->write_reg(WWAPT,   0x00000000);
    this->wait_time_ = 20;
  }
  else if (this->state_ == States::READ_TOTALS) {
    uint32_t aartu = this->read_reg(AARTU);
    uint32_t aarti = this->read_reg(AARTI);
    uint32_t aap = this->read_reg(AAP);
    this->wait_time_ = 1500;
    ESP_LOGV(TAG, "AARTU %d AARTI %d AAP %d", aartu, aarti, aap);
  }
  else if (this->state_ == States::INIT) {
    uint32_t mtpara0 = this->read_reg(MTPARA0);
    if (mtpara0 != 0xE0000000) {
      ESP_LOGE(TAG, "Invalid MTPARA0! 0x%02X != 0x%02X", mtpara0, 0xE0000000);
    }
    else
    {
      this->write_reg(MTPARA0, 0xE0000080);
    }

    this->wait_time_ = 10;
  }

  this->state_ = (States)((uint8_t)this->state_ + 1);
  this->start_time_ = millis();
}

void V9261F::update() {
  if (this->state_ != States::READY)
    return;

  int32_t values[6];
  uint32_t cf_cnt;
  if (this->read_reg(AAP, (uint32_t*)values, 6) &&
      this->read_reg(PCFCNT, &cf_cnt, 1))
  {
    float v_rms = values[2] / voltage_reference_;
    float i_rms = values[3] / current_reference_;
    float watt = values[0] / power_reference_;
    float total_energy_consumption = (float)cf_cnt / energy_reference_;
    float frequency = (float)values[5] / frequency_reference_;

    if (voltage_sensor_ != nullptr) {
      voltage_sensor_->publish_state(v_rms);
    }
    if (current_sensor_ != nullptr) {
      current_sensor_->publish_state(i_rms);
    }
    if (power_sensor_ != nullptr) {
      power_sensor_->publish_state(watt);
    }
    if (energy_sensor_ != nullptr) {
      energy_sensor_->publish_state(total_energy_consumption);
    }
    if (frequency_sensor_ != nullptr) {
      frequency_sensor_->publish_state(frequency);
    }

    ESP_LOGV(TAG, "U %fV, I %fA, P %fW, Cnt %d, ∫P %fkWh, F %fHz", v_rms, i_rms, watt, cf_cnt,
            total_energy_consumption, frequency);

    ESP_LOGV(TAG, "AAP %d, AAQ %d, AARTU %d, AARTI %d, SAFREQ %d, AFREQ %d, PCFCNT %d",
            values[0], values[1], values[2], values[3], values[4], values[5], cf_cnt);
  }
}

void V9261F::reset_energy_sensor() {
  this->write_reg(PEGY,   0);
  this->write_reg(NEGY,   0);
  this->write_reg(PCFCNT, 0);
  this->write_reg(NCFCNT, 0);
}

void V9261F::setup() {
  state_ = States::BROADCAST_1;
}

void V9261F::dump_config() {  // NOLINT(readability-function-cognitive-complexity)
  ESP_LOGCONFIG(TAG, "V9261F:");
  LOG_SENSOR("", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Energy", this->energy_sensor_);
  LOG_SENSOR("", "Frequency", this->frequency_sensor_);
  check_uart_settings(4800, 1, esphome::uart::UART_CONFIG_PARITY_ODD, 8);
}

}  // namespace v9261f
}  // namespace esphome
