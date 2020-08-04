#pragma once

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/components/duty_cycle/duty_cycle_sensor.h"
#include "esphome/components/fan/fan_state.h"

namespace esphome {
namespace rowenta_fan {

#define ROWFAN_LED_LOW 4
#define ROWFAN_LED_MEDIUM 14
#define ROWFAN_LED_HIGH 12
#define ROWFAN_LED_BOOST 13
#define ROWFAN_LED_MOSQUITO 5

#define ROWFAN_BTN_POWER this->led_boost
#define ROWFAN_BTN_SPEED this->led_low
#define ROWFAN_BTN_BOOST this->led_high
#define ROWFAN_BTN_MOSQUITO this->led_medium

#define ROWFAN_UPDATE_RATE_MS 200

#define ROWFAN_LED_ON_PERCENT 30.0f

class RowentaLedStore
{
    private:
    GPIOPin *pinGPIO;
    duty_cycle::DutyCycleSensor *led_sensor;

    public:
    RowentaLedStore(const std::string &name, int pinNumber, bool inverted);
    bool IsLedOn();
    void PressButton();
    void ReleaseButton();
};

class RowentaFan : public Component
{
    protected:

        typedef enum
        {
            UNDEFINED = 0,
            OFF,
            MINIMUM,
            MEDIUM,
            FAST,
            BOOST,
            MOSQUITO
        } RowFanStates;

        fan::FanState *fan_;
        bool discard_write = false;

        RowFanStates rowfan_state = RowFanStates::UNDEFINED;
        RowFanStates rowfan_desired_state = RowFanStates::UNDEFINED;
        RowentaLedStore *rowfan_active_btn = nullptr;
        ulong lastUpdate = 0;
        ulong lastStatePrint = 0;

        RowentaLedStore *led_low;
        RowentaLedStore *led_medium;
        RowentaLedStore *led_high;
        RowentaLedStore *led_boost;
        RowentaLedStore *led_mosquito;

        bool rowfan_refresh_state();
        void rowfan_btn_press(RowentaLedStore* buttonLed);
        void rowfan_control_state();
        void rowfan_set_state(RowFanStates state);
        void updateDesiredState();

    public:
        RowentaFan(fan::FanState *fan) : fan_(fan) {}
        void setup() override;
        void loop() override;
        void dump_config() override;
        float get_setup_priority() const override;
};

}  // namespace rowenta_fan
}  // namespace esphome