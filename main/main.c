#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/clocks.h"

const int I2C_CHIP_ADDRESS_W = 0x42;
const int I2C_CHIP_ADDRESS_R = 0x43;
const int I2C_SDA_GPIO = 16;
const int I2C_SCL_GPIO = 17;

void i2c_task(void *p) {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO);

    uint8_t buffer[6];

    // read whoami
    uint8_t reg_address = 0x76;
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS_W, &reg_address, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, I2C_CHIP_ADDRESS_R, buffer, 1, false);
    printf("WHOAMI: 0x%X \n", buffer[0]);

    // TODO
    // Leia o INT_ENABLE e imprima o valor
    printf("INT_ENABLE: 0x%X \n", buffer[0]);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");
    clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10);

    xTaskCreate(i2c_task, "i2c task", 4095, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {
    }
}