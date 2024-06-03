/**
 * @file config.h
 *
 * @brief Configurations for the WIUS and TinyProbe firmware
 *
 * This file contains macros that configure various functionalities of the WIUS and
 * TinyProbe firmware like power management, SPI, WiFi settings for WIUS and buffer sizes,
 * UDP port, GPIO numbers for TinyProbe, etc.
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup common
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/**
 * @defgroup config_wius WiUS firmware configurations
 * This module documents the firmware configurations specific to WiUS.
 * @ingroup wius
 * @{
 */

/** @name WiUS SPI configurations
 * @{
 */
#define WIUS_SPI_RX_TIMEOUT 100 /**< Timeout for reception (ticks) */
#define WIUS_SPI_EXT_CS0    53  /**< Use seperate CS0 pin (set to 0 if unused) */
#define WIUS_SPI_EXT_CS1    0   /**< Use seperate CS1 pin (set to 0 if unused) */
/** @}
 */

/** @name WiUS WiFi configurations
 * @{
 */
#define WIUS_WIFI_FILTER_BROADCAST 0 /**< Whether to filter the WiFi broadcast */
/** @}
 */

/**
 * @name WiUS logging configurations
 * @{
 */
#define WIUS_LOG_LEVEL LOG_LEVEL_DEBUG /**< Log level for WiUS */
#define WIUS_LOG_GPIO_LED_RED 7        /**< Red LED gpio number */
#define WIUS_LOG_GPIO_LED_GREEN 6      /**< Green LED gpio number */
/** @}
 */

/** @} End of config_wius group */

/** @defgroup config_tinyprobe TinyProbe firmware configurations
 *  This module documents the firmware configurations specific to TinyProbe.
 *  @ingroup tinyprobe
 *  @{
 */

/** @name TinyProbe general configurations
 * @{
 */
#define TP_PROBE_ID 1               /**< ID of the probe */
#define TP_WIFI_RX_BUFFER_SIZE 1472 /**< Size of the WiFi RX buffer */
#define TP_UDP_PACKET_SIZE 1000     /**< Size of one UDP packet (without header) */
#define TP_UDP_PORT 50007           /**< Port on which UDP transfers happen */
#define TP_GPIO_INT 2               /**< FPGA Interrupt UULP gpio number */
#define TP_GPIO_RESET 10            /**< FPGA Reset ULP gpio number */
#define TP_THREAD_STACK_MAIN 4096   /**< Stack of main thread */
#define TP_THREAD_STACK_WIFI 4096   /**< Stack of WiFi thread */
/** @}
 */

/** @name TinyProbe AFE control configurations
 * @{
 */
#define TP_AFE_SPI_DELAY_NS 500000 /**< Delay after SPI transfers in ns */
/** @}
 */

/** @name TinyProbe buffering configurations
 * @{
 */
#define TP_BUFFER_NUM 2     /**< Number of buffers available */
#define TP_BUFFER_SIZE 2048 /**< Size of one buffer in bytes */
/** @}
 */

/** @name TinyProbe commands configurations
 * @{
 */
#define TP_COMMAND_MAX 2048 /**< Maximum number of commands per WiFi package */
/** @}
 */

/** @name TinyProbe FPGA control configurations
 * @{
 */
#define TP_FPGA_SPI_DELAY_NS 50 /**< Delay after SPI transfers in ns */
/** @}
 */

/** @name TinyProbe MUX control configurations
 * @{
 */
#define TP_MUX_GPIO_EXTINT 3 /**< EXT INT MUX UULP gpio number */
#define TP_MUX_GPIO_AFETX 1  /**< AFE TX MUX ULP gpio number */
/** @}
 */

/** @name TinyProbe power control configurations
 * @{
 */
#define TP_POWER_GPIO_NEG_5V 2      /**< -5V ULP gpio number */
#define TP_POWER_GPIO_NEG_HV 52     /**< -HV gpio number */
#define TP_POWER_GPIO_POS_HV 56     /**< +HV gpio number */
#define TP_POWER_GPIO_LVDS_PWR_SW 8 /**< LVDS power switch ULP gpio number */
/** @}
 */

/** @name TinyProbe event flags
 * @{
 */
#define FLAG_CMD_RECEIVED (1 << 0)    /**< Command received flag */
#define FLAG_CMD_EXECUTED (1 << 1)    /**< Command done executing flag */
#define FLAG_SPI_TF0_DONE (1 << 2)    /**< SPI instance 0 transfer done flag */
#define FLAG_SPI_TF1_DONE (1 << 3)    /**< SPI instance 1 transfer done flag */
#define FLAG_FIFO_DATA_READY (1 << 4) /**< FIFO data ready flag */
/** @}
 */

/** @} End of config_tinyprobe group */

#endif /* CONFIG_H_ */
