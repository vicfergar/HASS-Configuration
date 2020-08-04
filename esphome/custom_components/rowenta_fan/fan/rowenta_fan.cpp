#include "rowenta_fan.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rowenta_fan {

static const char *TAG = "rowfan";
    
    RowentaLedStore::RowentaLedStore(const std::string &name, int pinNumber, bool inverted)
    {
        this->pinGPIO = new GPIOPin(pinNumber, INPUT, inverted);

        auto led_deltafilter = new sensor::DeltaFilter(10.0);
        this->led_sensor = new duty_cycle::DutyCycleSensor();
        this->led_sensor->set_internal(true);
        this->led_sensor->set_update_interval(100);
        this->led_sensor->set_name(name);
        this->led_sensor->set_pin(this->pinGPIO);
        this->led_sensor->set_accuracy_decimals(0);
        this->led_sensor->set_force_update(false);
        this->led_sensor->add_filters({led_deltafilter});
        App.register_component(this->led_sensor);
        App.register_sensor(this->led_sensor);
    }

    bool RowentaLedStore::IsLedOn()
    {
        return this->led_sensor->get_state() > ROWFAN_LED_ON_PERCENT;
    }

    void RowentaLedStore::PressButton()
    {
        this->pinGPIO->pin_mode(OUTPUT);
        this->pinGPIO->digital_write(0);
    }

    void RowentaLedStore::ReleaseButton()
    {
        this->pinGPIO->pin_mode(INPUT);
    }

    bool RowentaFan::rowfan_refresh_state()
    {
        auto last_state = this->rowfan_state;
        if (this->led_mosquito->IsLedOn())
        {
            this->rowfan_state = RowFanStates::MOSQUITO;
        }
        else if (this->led_low->IsLedOn())
        {
            this->rowfan_state = RowFanStates::MINIMUM;
        }
        else if (this->led_medium->IsLedOn())
        {
            this->rowfan_state = RowFanStates::MEDIUM;
        }
        else if (this->led_high->IsLedOn())
        {
            this->rowfan_state = RowFanStates::FAST;
        }
        else if (this->led_boost->IsLedOn())
        {
            this->rowfan_state = RowFanStates::BOOST;
        }
        else
        {
            this->rowfan_state = RowFanStates::OFF;
        }

        return this->rowfan_desired_state == RowFanStates::UNDEFINED &&
               this->rowfan_state != last_state;
    }

    void RowentaFan::rowfan_btn_press(RowentaLedStore* buttonLed)
    {
        if (!this->rowfan_active_btn)
        {
            this->rowfan_active_btn = buttonLed;
            this->rowfan_active_btn->PressButton();
        }
    }

    void RowentaFan::rowfan_control_state()
    {
        if (this->rowfan_desired_state == this->rowfan_state || this->rowfan_desired_state == RowFanStates::UNDEFINED)
        {
            this->rowfan_desired_state = RowFanStates::UNDEFINED;
        }
        else if (this->rowfan_desired_state == RowFanStates::OFF && this->rowfan_state != RowFanStates::OFF)
        {
            ESP_LOGD(TAG, "Pressing power button");
            rowfan_btn_press(ROWFAN_BTN_POWER);
        }
        else
        {
            if (this->rowfan_state == RowFanStates::OFF)
            {
                ESP_LOGD(TAG, "Pressing power button");
                this->rowfan_btn_press(ROWFAN_BTN_POWER);
            }
            else if (this->rowfan_desired_state == RowFanStates::MOSQUITO)
            {
                ESP_LOGD(TAG, "Pressing mosquito button");
                rowfan_btn_press(ROWFAN_BTN_MOSQUITO);
            }
            else if (this->rowfan_desired_state == RowFanStates::BOOST)
            {
                ESP_LOGD(TAG, "Pressing boost button");
                this->rowfan_btn_press(ROWFAN_BTN_BOOST);
            }
            else
            {
                ESP_LOGD(TAG, "Pressing speed button");
                this->rowfan_btn_press(ROWFAN_BTN_SPEED);
            }
        }
    }

    void RowentaFan::rowfan_set_state(RowFanStates state)
    {
        if(!this->fan_ || state == RowFanStates::UNDEFINED)
        {
            return;
        }
        
        this->discard_write = true; /* Prevent sending the value back to the hardware */
        auto call = state == RowFanStates::OFF ? this->fan_->turn_off() : this->fan_->turn_on();
        switch (state)
        {
            case RowFanStates::MINIMUM:
                call.set_speed(fan::FAN_SPEED_LOW);
                break;
            case RowFanStates::MEDIUM:
                call.set_speed(fan::FAN_SPEED_MEDIUM);
                break;
            case RowFanStates::FAST:
            case RowFanStates::BOOST:
            case RowFanStates::MOSQUITO:
                call.set_speed(fan::FAN_SPEED_HIGH);
                break;
            default:
                break;
        }
        call.perform();
    }

    void RowentaFan::updateDesiredState()
    {
        if (this->discard_write)
        {
            this->discard_write = false;
            return;
        }

        if (this->fan_->state) {
            if (this->fan_->speed == fan::FAN_SPEED_LOW)
                this->rowfan_desired_state = RowFanStates::MINIMUM;
            else if (this->fan_->speed == fan::FAN_SPEED_MEDIUM)
                this->rowfan_desired_state = RowFanStates::MEDIUM;
            else if (this->fan_->speed == fan::FAN_SPEED_HIGH)
                this->rowfan_desired_state = RowFanStates::FAST;
        }
        else
        {
            this->rowfan_desired_state = RowFanStates::OFF;
        }
        
        ESP_LOGD(TAG, "Setting speed: %.2f", this->rowfan_desired_state);
    }

    void RowentaFan::dump_config() {
        ESP_LOGCONFIG(TAG, "Fan '%s':", this->fan_->get_name().c_str());
    }

    void RowentaFan::setup()
    {
        auto traits = fan::FanTraits();
        traits.set_speed(true);
        this->fan_->set_traits(traits);
        this->fan_->add_on_state_callback([this]() { this->updateDesiredState(); });

        this->led_low = new RowentaLedStore("LED_LOW", ROWFAN_LED_LOW, false);
        this->led_medium = new RowentaLedStore("LED_MED", ROWFAN_LED_MEDIUM, false);
        this->led_high = new RowentaLedStore("LED_HIGH", ROWFAN_LED_HIGH, false);
        this->led_boost = new RowentaLedStore("LED_BOOST", ROWFAN_LED_BOOST, false);
        this->led_mosquito = new RowentaLedStore("LED_MOSQ", ROWFAN_LED_MOSQUITO, true);
    }

    void RowentaFan::loop()
    {
        if (millis() - this->lastUpdate < ROWFAN_UPDATE_RATE_MS)
        {
            return;
        }

        this->lastUpdate = millis();

        if (this->rowfan_active_btn)
        {
            this->rowfan_active_btn->ReleaseButton();
            this->rowfan_active_btn = nullptr;
            return;
        }

        if (rowfan_refresh_state())
        {
            ESP_LOGD(TAG, "State externally changed to %d", this->rowfan_state);
            this->rowfan_set_state(this->rowfan_state);
        }

        this->rowfan_control_state();

        if (millis() - this->lastStatePrint < 1000)
        {
            return;
        }
        this->lastStatePrint = millis();

        ESP_LOGD(TAG, "Low: %u  Medium: %u  High: %u  Boost: %u  Mosquito: %u",
                 led_low->IsLedOn(),
                 led_medium->IsLedOn(),
                 led_high->IsLedOn(),
                 led_boost->IsLedOn(),
                 led_mosquito->IsLedOn());
    }

    float RowentaFan::get_setup_priority() const { return setup_priority::DATA; }
}  // namespace rowenta_fan
}  // namespace esphome