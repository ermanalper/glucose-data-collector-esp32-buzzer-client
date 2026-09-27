#pragma once
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class OutPinService {
private:
    gpio_num_t pin1;
    gpio_num_t pin2;
    bool isRunningTestProtocol = false;

public:
    OutPinService(gpio_num_t pinNumber1, gpio_num_t pinNumber2)
        : pin1(pinNumber1), pin2(pinNumber2) {

        gpio_reset_pin(pin1);
        gpio_reset_pin(pin2);

        gpio_set_direction(pin1, GPIO_MODE_OUTPUT);
        gpio_set_direction(pin2, GPIO_MODE_OUTPUT);

    
        gpio_set_level(pin1, 0);
        gpio_set_level(pin2, 0);

        isRunningTestProtocol = false;
    }

    void turnAllOn() {
        gpio_set_level(pin1, 1);
        gpio_set_level(pin2, 1);
    }

    void turnAllOff() {
        gpio_set_level(pin1, 0);
        gpio_set_level(pin2, 0);
    }

    void runTestProtocol() {
        if (isRunningTestProtocol) {
            return; 
        }
        isRunningTestProtocol = true;
        // save initial states
        int initialPin1State = gpio_get_level(pin1);
        int initialPin2State = gpio_get_level(pin2);
        turnAllOff();
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(pin1, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin1, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin1, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin1, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin2, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin2, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin2, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(pin2, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
        turnAllOn();
        vTaskDelay(pdMS_TO_TICKS(100));
        turnAllOff();
        vTaskDelay(pdMS_TO_TICKS(100));
        turnAllOn();
        vTaskDelay(pdMS_TO_TICKS(100));

        // restore initial states
        gpio_set_level(pin1, initialPin1State);
        gpio_set_level(pin2, initialPin2State);
        




        isRunningTestProtocol = false;

    }
};