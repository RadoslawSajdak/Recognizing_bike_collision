#include <zephyr.h>
#include <logging/log.h>
#include <drivers/uart.h>

#include <pm/pm.h>
#include <pm/state.h>
#include <device.h>
#include <pm/device_runtime.h>

#include "toolkits/defines.h"
#ifdef  COMPONENT_TK_I2C
    #include "toolkits/tk_i2c.h"
#endif
#ifdef  COMPONENT_TK_GPIO
    #include "toolkits/tk_gpio.h"
#endif
#ifdef  COMPONENT_TK_BUZZER
    #include "toolkits/tk_buzzer.h"
#endif
#ifdef  COMPONENT_TK_LED
    #include "toolkits/tk_led.h"
#endif
#ifdef COMPONENT_TK_ACCELEROMETER
    #include "toolkits/tk_accel.h"
#endif
#ifdef  COMPONENT_TK_UART
    #include "toolkits/tk_uart.h"
#endif
#ifdef  COMPONENT_TK_LTE
    #include "toolkits/tk_lte.h"
#endif
#ifdef  COMPONENT_TK_GPS
    #include "toolkits/tk_gps.h"
#endif



LOG_MODULE_REGISTER(main);

void main(void)
{
    int error;
    /*----- I2C -----*/
    #ifdef  COMPONENT_TK_I2C
        if( 0 != tk_i2c_init()) LOG_ERR("Can't initiate I2C");
    #endif

    /*----- GPIO -----*/
    #ifdef  COMPONENT_TK_GPIO
    if( 0 == tk_gpio_init() ) LOG_INF("GPIO Init success");
    #endif

    #ifdef  COMPONENT_TK_UART
    if (0 != tk_uart_init()) LOG_ERR("Couldn't init uart1");
    #endif
    #ifdef  COMPONENT_TK_LTE
    tk_lte_thread_start();
    #endif

    /*----- Buzzer -----*/
    #ifdef  COMPONENT_TK_BUZZER
    if( 0 == tk_buzzer_init()) LOG_INF("Buzzer Init success");
    tk_buzzer_set_volume(tk_buzzer_vol_high);
    if( 0 == tk_buzzer_play_melody(0,34,1)) LOG_INF("Melody Done");
    tk_buzzer_set_volume(tk_buzzer_vol_mute);
    LOG_INF("Buzzer func is done!");
    #endif

    /*----- LEDs -----*/
    #ifdef  COMPONENT_TK_LED
    if( 0 != tk_led_init() ) LOG_ERR("Can't initiate LEDs");
    if( 0 != (error = tk_led_test()) ) LOG_ERR("Led PWM test failed... Code: %d", error);
    if( 0 != (error = tk_led_set_RGB(255,0,128,5000)) ) LOG_ERR("Set RGB error: %d", error);
    if( 0 != (error = tk_led_set_RGB(255,0,0,5000)) ) LOG_ERR("Set RGB error: %d", error);
    LOG_INF("LED Init success");
    #endif

    /*----- Accelerometer -----*/
    #ifdef  COMPONENT_TK_ACCELEROMETER
    if( 0 != (error = tk_accel_init() )) LOG_ERR("Can't initiate Accelerometer. Code: %d", error);
    LOG_INF("Starting self-test for accel");
    if ( 0 != ( error = tk_accel_self_test() )) LOG_ERR("Self test failed");
    tk_acccel_fsm_wake_up_setup();
    #endif

    /*----- GPS -----*/
    #ifdef  COMPONENT_TK_GPS

    tk_gps_get_data();

    #endif

    LOG_INF("Device init finished");

    while (1)
    {
        k_msleep(100U);
    }
}

void pm_init(void)
{
  const struct device *i2c = device_get_binding(DT_LABEL(DT_NODELABEL(i2c1)));
  pm_device_enable(i2c);

  const struct device *pwm = device_get_binding(DT_LABEL(DT_ALIAS(pwm_buzzer)));
  pm_device_enable(pwm);

  const struct device *gpio0 = device_get_binding("GPIO_0");
  pm_device_enable(gpio0);

  const struct device *gpio1 = device_get_binding("GPIO_1");
  pm_device_enable(gpio1);

  struct pm_state_info pminfo = {PM_STATE_STANDBY, 0, 0};
  pm_power_state_set(pminfo);
}
