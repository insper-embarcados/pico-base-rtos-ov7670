/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/clocks.h"




volatile bool vsync_cam_flag;
volatile bool href_cam_flag;
volatile bool pclk_cam_flag;



struct cam_ov7670 {
    const int VSYNC;
    const int HREF;
    const int PCLK;
};

// defining camera pins struct
struct cam_ov7670 camera = {16, 17, 18};

void camera_callback(uint gpio, uint32_t events) {
  // entra 
  if (gpio == camera.VSYNC && events == 0x8){
    vsync_cam_flag = true;
  }
  if (gpio == camera.VSYNC && events == 0x4){
    vsync_cam_flag = false;
  }

  if (gpio == camera.HREF && events == 0x8){
    href_cam_flag = true;
  }
  if (gpio == camera.HREF && events == 0x4){
    href_cam_flag = false;
  }
}

void sync(void *p){
    struct cam_ov7670 *pins = (struct cam_ov7670 *)p;
    gpio_init(pins->VSYNC);
    gpio_init(pins->HREF);
    gpio_init(pins->PCLK);
    gpio_set_dir(pins->VSYNC, GPIO_IN);
    gpio_set_dir(pins->HREF, GPIO_IN);
    gpio_set_dir(pins->PCLK, GPIO_IN);
    while (true){
        // starting a frame
        while (!vsync_cam_flag);
        while (vsync_cam_flag);
        // preparing camera -> zering FIFO
        // camera.preparing()
        while (!href_cam_flag);
        while(!pclk_cam_flag);
        // starting capture to buffer
        // exemplo qqvgaframe1[i] = LPC_GPIO2->FIOPIN; //Lê o barramento de dados do Ov7670
        while (pclk_cam_flag); //Aguarda o sinal de PCLK ir a nível baixo


        while (!vsync_cam_flag);
        // stop capturing

        }
}

int main() {
    stdio_init_all();

    // Initializes clock on GP21 for OV7670
    clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10);

    

    // set callback for the end of the package
    gpio_set_irq_enabled_with_callback(camera.VSYNC, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &camera_callback);

    
    xTaskCreate(sync, "Sync Task", 4096, &camera, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
