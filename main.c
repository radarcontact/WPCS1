/*
 
888888b.                     888     888           .d888         888      888               888 
888  "88b                    888     888          d88P"          888      888               888 
888  .88P                    888     888          888            888      888               888 
8888888K.   .d88b.  888  888 888     888 88888b.  888888 .d88b.  888  .d88888  .d88b.   .d88888 
888  "Y88b d88""88b `Y8bd8P' 888     888 888 "88b 888   d88""88b 888 d88" 888 d8P  Y8b d88" 888 
888    888 888  888   X88K   888     888 888  888 888   888  888 888 888  888 88888888 888  888 
888   d88P Y88..88P .d8""8b. Y88b. .d88P 888  888 888   Y88..88P 888 Y88b 888 Y8b.     Y88b 888 
8888888P"   "Y88P"  888  888  "Y88888P"  888  888 888    "Y88P"  888  "Y88888  "Y8888   "Y88888 
File:       main.c
 
Abstract:
 
 
 
 
 
Version:    <0.1>
 
Created: 8/15/13
 
Author: George W. Melcer
 
Copyright ( C ) 2015 BoxUnfolded LLC. All Rights Reserved.
 
 
 
*/



/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf_dfu_settings.h"
#include "nrf_dfu_flash.h"
#include "nrf_dfu_types.h"
#include "nrf_dfu_utils.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "fstorage.h"
#include "fds.h"
#include "peer_manager.h"

#include "bsp.h"
#include "bsp_btn_ble.h"
#include "sensorsim.h"
#include "nrf_gpio.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"

#include "misc.h"
#include "nrf_delay.h"
 #include "ble_lbs.h"
#include "sdk_macros.h"

#include "pins.h"
 #include "nrf_drv_gpiote.h"
#include "application.h"
 #include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "deviceinfo.h"


#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     SHINEPAY_NAME                           /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "BoxUnfolded"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                         /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */


#define FLASH_FLAG_NONE                 (0)
#define FLASH_FLAG_OPER                 (1<<0)
#define FLASH_FLAG_FAILURE_SINCE_LAST   (1<<1)
#define FLASH_FLAG_SD_ENABLED           (1<<2)


#define SCHED_QUEUE_SIZE                10 
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */




extern uint8_t numberOfQuarterPulses;
extern bool enableLED;


static uint32_t m_flags;
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */
ble_lbs_t                                b_ble;                                      /**< Device Service instance. */
volatile uint32_t globalCounter =0;

APP_TIMER_DEF(m_pause_timer_id);
APP_TIMER_DEF(m_pulse_timer_id);
APP_TIMER_DEF(m_global_timer_id);
APP_TIMER_DEF(m_led_timer_id);



static void pulse_timeout_handler(void * p_context);
static void global_timeout_handler(void * p_context);
static void pause_timeout_handler(void * p_context);






#define DELAY_BETWEEN_PULSES                100   /* in ms */
#define PULSE_DURATION                      150    /* in ms */



#define LOW false
#define HIGH true

volatile uint16_t ledPulses = 3;








#ifdef BLE_STACK_SUPPORT_REQD

// Function prototypes
static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result);

FS_REGISTER_CFG(fs_config_t fs_dfu_config) =
{
    .callback       = fs_evt_handler,            // Function for event callbacks.
    .p_start_addr   = (uint32_t*)MBR_SIZE,
    .p_end_addr     = (uint32_t*)BOOTLOADER_SETTINGS_ADDRESS + CODE_PAGE_SIZE
};


static void fs_evt_handler(fs_evt_t const * const evt, fs_ret_t result)
{
    // Clear the operation flag
    m_flags &= ~FLASH_FLAG_OPER;

    if (result == FS_SUCCESS)
    {
        // Clear flag for ongoing operation and failure since last
        m_flags &= ~FLASH_FLAG_FAILURE_SINCE_LAST;
    }
    else
    {
        NRF_LOG_INFO("Generating failure\r\n");
        m_flags |= FLASH_FLAG_FAILURE_SINCE_LAST;
    }

    if (evt->p_context)
    {
        //lint -e611
        ((dfu_flash_callback_t)evt->p_context)(evt, result);
    }
}

#endif





uint32_t nrf_dfu_flash_init(bool sd_enabled)
{
    fs_fake_init(); // IMPORTANT! ADD THIS! WAS BUG IN THE EXAMPLE!
    m_flags = FLASH_FLAG_SD_ENABLED;
    return NRF_SUCCESS;
}


fs_ret_t nrf_dfu_flash_store(uint32_t const * p_dest, uint32_t const * const p_src, uint32_t len_words, dfu_flash_callback_t callback)
{
    fs_ret_t ret_val = FS_SUCCESS;

    if ((m_flags & FLASH_FLAG_SD_ENABLED) != 0)
    {
        // Check if there is a pending error
        if ((m_flags & FLASH_FLAG_FAILURE_SINCE_LAST) != 0)
        {
            NRF_LOG_INFO("Flash: Failure since last\r\n");
            return FS_ERR_FAILURE_SINCE_LAST;
        }

        // Set the flag to indicate ongoing operation
        m_flags |= FLASH_FLAG_OPER;
        //lint -e611
        ret_val = fs_store(&fs_dfu_config, p_dest, p_src, len_words, (void*)callback);

        if (ret_val != FS_SUCCESS)
        {
            NRF_LOG_INFO("Flash: failed %d\r\n", ret_val);
            return ret_val;
        }

        // Set the flag to indicate ongoing operation
        m_flags |= FLASH_FLAG_OPER;
    }

    return ret_val;
}


/** @brief Internal function to initialize DFU BLE transport
 */
fs_ret_t nrf_dfu_flash_erase(uint32_t const * p_dest, uint32_t num_pages, dfu_flash_callback_t callback)
{
    fs_ret_t ret_val = FS_SUCCESS;
    NRF_LOG_INFO("Erasing: 0x%08x, num: %d\r\n", (uint32_t)p_dest, num_pages);

    if ((m_flags & FLASH_FLAG_SD_ENABLED) != 0)
    {
        // Check if there is a pending error
        if ((m_flags & FLASH_FLAG_FAILURE_SINCE_LAST) != 0)
        {
            NRF_LOG_INFO("Erase: Failure since last\r\n");
            return FS_ERR_FAILURE_SINCE_LAST;
        }

        m_flags |= FLASH_FLAG_OPER;
        ret_val = fs_erase(&fs_dfu_config, p_dest, num_pages, (void*)callback);

        if (ret_val != FS_SUCCESS)
        {
            NRF_LOG_INFO("Erase failed: %d\r\n", ret_val);
            m_flags &= ~FLASH_FLAG_OPER;
            return ret_val;
        }

        // Set the flag to indicate ongoing operation
        m_flags |= FLASH_FLAG_OPER;
    }

    return ret_val;
}







void flash_callback(fs_evt_t const * const evt, fs_ret_t result)
{
    if (result == FS_SUCCESS)
    {

        sd_nvic_SystemReset(); 

    }
}



static void enter_bootloader(void)
{
    
    uint32_t err_code = nrf_dfu_flash_init(true);

    nrf_dfu_settings_init();

    s_dfu_settings.enter_buttonless_dfu = true;
    

   err_code = nrf_dfu_settings_write(flash_callback);

  if (err_code != NRF_SUCCESS)
    {
    }


    /*
    TODO:
     - Save bond data
    */
}




/* YOUR_JOB: Declare all services structure your application is using
   static ble_xx_service_t                     m_xxs;
   static ble_yy_service_t                     m_yys;
 */

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */

static void advertising_start(void);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            advertising_start();
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}


#define LED_ON                      true
#define LED_OFF                      false

bool ledState = false;




void LEDOff(void)
{
        //enableLED = false;
        nrf_gpio_pin_set(LED_PIN);
        ledState = LED_OFF;

}



void LEDOn(void)
{
        //enableLED = false;
        nrf_gpio_pin_clear(LED_PIN);
        ledState = LED_ON;

}

void disableLED(void)
{

    enableLED = false;
}


#define DELAY_ON_STATE                      30
#define DELAY_OFF_STATE                      400

void pulseLEDs(uint8_t pulses)
{
              uint32_t err_code;
        uint32_t pulse_time;
        ledPulses = pulses;
            pulse_time = APP_TIMER_TICKS(DELAY_ON_STATE, APP_TIMER_PRESCALER);  //1ms ticks
        err_code = app_timer_start(m_led_timer_id, (uint32_t)pulse_time, NULL);
        APP_ERROR_CHECK(err_code);
}




static void led_timeout_handler(void * p_context)
{

    if(ledState == LED_OFF)
    {
        
        LEDOn();

    }
    else
    {
        LEDOff();
        ledPulses--;

        
    }







    if(ledPulses > 0)
    {
        //pulse again
        uint32_t err_code;
        uint32_t pulse_time;

        if(ledState == LEDOff)
        {
            pulse_time = APP_TIMER_TICKS(DELAY_OFF_STATE, APP_TIMER_PRESCALER);  //1ms ticks
            err_code = app_timer_start(m_led_timer_id, (uint32_t)pulse_time, NULL);
            APP_ERROR_CHECK(err_code);

        }
        else
        {
            pulse_time = APP_TIMER_TICKS(DELAY_ON_STATE, APP_TIMER_PRESCALER);  //1ms ticks
            err_code = app_timer_start(m_led_timer_id, (uint32_t)pulse_time, NULL);
            APP_ERROR_CHECK(err_code);

        }
;
    
    }

    else
    {
        //turn led off
        ledPulses = 5;
        LEDOff();
    }





}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{

    ret_code_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.
    err_code = app_timer_create(&m_pulse_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                pulse_timeout_handler);

    APP_ERROR_CHECK(err_code);


    err_code = app_timer_create(&m_pause_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                pause_timeout_handler);

    APP_ERROR_CHECK(err_code);


        err_code = app_timer_create(&m_led_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                led_timeout_handler);

    APP_ERROR_CHECK(err_code);


    err_code = app_timer_create(&m_global_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                global_timeout_handler);


    APP_ERROR_CHECK(err_code);



    // Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}




/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void led_write_handler(ble_lbs_t * p_lbs, uint8_t* led_state)
{
uint8_t i =0;
uint32_t err_code;
for(i=0; i<5; i++)
    NRF_LOG_INFO("RX'ed BLE Data: %x\r\n", led_state[i]);


if(led_state[0] == 0xFA)
{
    NRF_LOG_INFO("ENTERINGING BOOTLOADER");
    // err_code = sd_power_gpregret_set(BOOTLOADER_DFU_START);

    // APP_ERROR_CHECK(err_code);

    // sd_nvic_SystemReset();   

    enter_bootloader();

}
else
{
    decode_packet(led_state, 0);
}


    //decode_packet(led_state, 0);
    
}



/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
   static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                       ble_yy_service_evt_t * p_evt)
   {
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. \r\n", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
   }*/

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t       err_code;

    ble_lbs_init_t init;

    init.led_write_handler = led_write_handler;

     err_code = ble_lbs_init(&b_ble, &init);
     APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
static void application_timers_start(void)
{
     //YOUR_JOB: Start your timers. below is an example of how to start a timer.
       uint32_t err_code;
       uint32_t ticks = APP_TIMER_TICKS(1, APP_TIMER_PRESCALER);

       err_code = app_timer_start(m_global_timer_id, ticks, NULL);
       APP_ERROR_CHECK(err_code); 

}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);

    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            //sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.\r\n");


            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    /** The Connection state module has to be fed BLE events in order to function correctly
     * Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions. */
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    /*YOUR_JOB add calls to _on_ble_evt functions from each service your application is using
       ble_xxs_on_ble_evt(&m_xxs, p_ble_evt);
       ble_yys_on_ble_evt(&m_yys, p_ble_evt);
     */

           on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_lbs_on_ble_evt(&b_ble, p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist();
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break; // BSP_EVENT_KEY_0

        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

ble_uuid_t adv_uuids[] = {{LBS_UUID_SERVICE, b_ble.uuid_type}};

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
     bsp_event_t startup_event;

    // uint32_t err_code;

    // err_code = bsp_btn_ble_init(NULL, &startup_event);
    // APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);

    APP_ERROR_CHECK(err_code);
}









void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
         uint8_t pinState = nrf_gpio_pin_read(PIN_IN);
         static uint32_t lastTickCount = 0;


   if(1 == pinState)
    {
        // if(lastTickCount <  (globalCounter + 300))
        // { 
            //
            nrf_gpio_pin_clear(LED_PIN);
            nrf_gpio_pin_set(PIN_OUT);
        if((lastTickCount+1500) <  globalCounter)
        { 

            increment_quarter_count_only();
            lastTickCount = globalCounter;
            //increment_quarter_count();
        }

    
    }
    else if(0 == pinState)
    {
    nrf_gpio_pin_set(LED_PIN);
     nrf_gpio_pin_clear(PIN_OUT);


    }
        
    
    
}





/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output, 
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
  

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    //in_config.pull = NRF_GPIO_PIN_PULLDOWN;

    err_code = nrf_drv_gpiote_in_init(PIN_IN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(err_code);



    nrf_drv_gpiote_in_event_enable(PIN_IN, true);

 }



/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}





static void init_output_pin(void)
{

    //nrf_gpio_cfg_output(PIN_OUT);
        nrf_gpio_cfg(
        PIN_OUT,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_PULLUP,
        GPIO_PIN_CNF_DRIVE_H0H1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_cfg_output(LED_PIN);
    nrf_gpio_pin_set(LED_PIN);
    setPinOut(LOW);

    

}






static void global_timeout_handler(void * p_context)
{


    globalCounter++;
    if(enableLED)
    {
        if(globalCounter % 100 == 0)
        {
            nrf_gpio_pin_toggle(LED_PIN);
        }
    }

    if(globalCounter == 0XFFFFFFFF) //overflow protection
    {
        globalCounter = 0;
    }

}





static void pause_timeout_handler(void * p_context)
{


        //nrf_gpio_pin_set(PIN_OUT);
        setPinOut(HIGH);
    uint32_t err_code;
    uint32_t pulse_time;
    
    pulse_time = APP_TIMER_TICKS(PULSE_DURATION, APP_TIMER_PRESCALER);  //1ms ticks
    err_code = app_timer_start(m_pulse_timer_id, (uint32_t)pulse_time, NULL);
    APP_ERROR_CHECK(err_code);


       // pulse_pin_out();


}


void spoofQuarter(void)
{

        uint32_t err_code;
        uint32_t pulse_time;

        pulse_time = APP_TIMER_TICKS(DELAY_BETWEEN_PULSES, APP_TIMER_PRESCALER);  //1ms ticks
        err_code = app_timer_start(m_pause_timer_id, (uint32_t)pulse_time, NULL);
        APP_ERROR_CHECK(err_code);

}





void pulse_pin_out(void)
{

    uint32_t err_code;
    uint32_t pulse_time;
    //numberOfQuarterPulses= 6;
    pulse_time = APP_TIMER_TICKS(PULSE_DURATION, APP_TIMER_PRESCALER);  //1ms ticks
    err_code = app_timer_start(m_pulse_timer_id, (uint32_t)pulse_time, NULL);
    APP_ERROR_CHECK(err_code);
    


       
}

void setPinOut(bool state)   //function allow easy chanability for active High/Low
{
    if(state == LOW)
    {
        nrf_gpio_pin_clear(PIN_OUT); 
    }
    else if(state == HIGH)
    {
        nrf_gpio_pin_set(PIN_OUT); 
    }



}




/**@brief Function for handling the Temperature measurement timer timeout.
 *
 * @details This function will be called each time the temperature measurement timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void pulse_timeout_handler(void * p_context)
{



    setPinOut(LOW);
    
    if(0!=numberOfQuarterPulses)
    {


        spoofQuarter();


    }
    else
    {

        LEDOff();

    }
numberOfQuarterPulses--;


}











/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    bool     erase_bonds;

    // Initialize.



    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("ShinePay Init Starting...\r\n");

    timers_init();
    gpio_init();
    init_output_pin();
    


    
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    peer_manager_init(erase_bonds);
    if (erase_bonds == true)
    {
        NRF_LOG_INFO("Bonds erased!\r\n");
    }
    gap_params_init();
    advertising_init();
    services_init();
    conn_params_init();
    
    // Start execution.
    

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
    application_timers_start();

NRF_LOG_INFO("ShinePay Init Ended\r\n");
    // Enter main loop.
    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}


/**
 * @}
 */
