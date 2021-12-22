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
#ifdef  COMPONENT_TK_BUTTON
    #include "toolkits/tk_button.h"
#endif
#ifdef COMPONENT_PB_SERVICE
    #include "../pebblebee-found11-fw/src/app/app_pb_ble_service.h"
    #include "../pebblebee-found11-fw/src/app/app_smp_ota.h"
#endif

#define ALARM_STOP_TIMEOUT_SEC              10U
#define ALARM_RESEND_PERIOD_MINUTES         5U

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static void accel_timer_callback(struct k_timer *timer_id);
static void long_timer_callback(struct k_timer *timer_id);
K_TIMER_DEFINE(g_accel_timer, accel_timer_callback, NULL);
K_TIMER_DEFINE(g_gps_timeout_timer, accel_timer_callback, NULL);
K_TIMER_DEFINE(g_periodically_timer, long_timer_callback, NULL);

static bool                     g_event_interrupted = false;
static bool                     g_send_periodically = false;
static bool                     g_device_running = false; // false = stopped, true = running
static bool                     g_timer_started = false;
static bool                     g_long_timer_up = false;

void clear_flags(void);

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

    #ifdef COMPONENT_TK_GASGAUGE
    if( tk_success != tk_gas_initialize() ) LOG_ERR("Can't initate gasgauge");
    #endif
    #ifdef COMPONENT_PB_SERVICE
    if (app_pb_ble_service_init() == 0)
    {
        app_smp_ota_init();
        app_pb_ble_service_adv_start();
        LOG_INF("WORKS 👌 🐝 !!!!!!!");
    }
    #endif
    /*----- Power management -----*/
    pm_init();

    #ifdef  COMPONENT_TK_LTE
    tk_lte_init_setup();
    #endif

    /*----- Buzzer -----*/
    #ifdef  COMPONENT_TK_BUZZER
    if( 0 == tk_buzzer_init()) LOG_INF("Buzzer Init success");
    tk_buzzer_set_volume(tk_buzzer_vol_high);
    if( 0 == tk_buzzer_play_melody(0,34,1)) LOG_INF("Melody Done");
    tk_buzzer_set_volume(tk_buzzer_vol_mute);
    LOG_INF("Buzzer func is done!");
    #endif

    /*----- GPS -----*/
    #ifdef  COMPONENT_TK_GPS
    tk_gps_power(false);    // Just to be sure if GPS is off
    #endif

    #ifdef  COMPONENT_TK_BUTTON
    tk_button_init();
    #endif

    /*----- Accelerometer -----*/
    #ifdef  COMPONENT_TK_ACCELEROMETER
    if( 0 != (error = tk_accel_init() )) LOG_ERR("Can't initiate Accelerometer. Code: %d", error);
    #endif

    LOG_INF("Device init finished");
    g_device_running = true;
    while (1)
    {
        if(g_button_events.long_press) //Switch device state
        {
            LOG_DBG("Long press");
            if(g_device_running) // Turn everything off
            {
                LOG_DBG("Turning off");
                // TODO: Buzzer off
                clear_flags();
                tk_gps_power(false);
                tk_lte_reset();
                tk_lte_power_off();
                tk_accel_power(false);
                g_device_running = false;
            }
            else
            {
                LOG_DBG("Turning on");
                tk_accel_init();
                clear_flags();
                g_device_running = true;
            }
        }
        else if(g_button_events.short_press)
        {
            LOG_DBG("Short press");
            if(g_device_running)
            {
                clear_flags();
                // TODO: Buzzer off
                tk_gps_power(false);
                tk_lte_reset();
                tk_lte_power_off();
                // Leave accel On, device is running
            }
            else
            {
                ;// Ignore, because the device is off    
            }
        }
        else if(g_device_running)
        {
            if( tk_accel_get_event_status() &&  !g_timer_started)
            {
                LOG_DBG("Accel event. Setting timer up");
                tk_accel_clear_event_status();
                g_timer_started = true;
                k_timer_start(&g_accel_timer, K_SECONDS(ALARM_STOP_TIMEOUT_SEC), K_NO_WAIT);
                // TODO: Buzzer on
            }
            else if( g_event_interrupted || (g_send_periodically && g_long_timer_up))
            {
                tk_gps_get_data(true); // Leave GPS on to hold fix
                while( !tk_gps_get_data_ready()  && !g_button_events.long_press && !g_button_events.short_press) 
                {
                    k_msleep(100);
                }
                if(!g_button_events.short_press && !g_button_events.long_press) 
                {
                    tk_lte_send_sms_location();
                    // Resetup device for auto wakeup periodically.
                    g_send_periodically = true;
                    g_event_interrupted = false;
                    g_long_timer_up = false;
                    k_timer_start(&g_periodically_timer, K_SECONDS(60 * ALARM_RESEND_PERIOD_MINUTES), K_NO_WAIT);
                }
            }
        }
        k_msleep(1000U);
    }
}

void clear_flags(void)
{
    g_button_events.short_press = false;
    g_button_events.long_press = false;
    g_send_periodically = false;
    g_event_interrupted = false;
    g_timer_started = false;
    k_timer_stop(&g_periodically_timer);
    k_timer_stop(&g_accel_timer);
    tk_accel_clear_event_status();
}
static void accel_timer_callback(struct k_timer *timer_id)
{
    g_event_interrupted = true;
    g_timer_started = false;
    k_timer_stop(timer_id);

    return;    
}

static void long_timer_callback(struct k_timer *timer_id)
{
    g_event_interrupted = true;
    g_timer_started = false;
    g_long_timer_up = true;
    k_timer_stop(timer_id);

    return;    
}


void pm_init(void)
{
  const struct device *i2c = device_get_binding(DT_LABEL(DT_NODELABEL(i2c1)));
  pm_device_enable(i2c);

  const struct device *uart0 = device_get_binding(DT_LABEL(DT_NODELABEL(uart0)));
  pm_device_enable(uart0);

  const struct device *uart1 = device_get_binding(DT_LABEL(DT_NODELABEL(uart1)));
  pm_device_enable(uart1);

  const struct device *pwm = device_get_binding(DT_LABEL(DT_ALIAS(pwm_buzzer)));
  pm_device_enable(pwm);

  const struct device *gpio0 = device_get_binding("GPIO_0");
  pm_device_enable(gpio0);

  const struct device *gpio1 = device_get_binding("GPIO_1");
  pm_device_enable(gpio1);
}
