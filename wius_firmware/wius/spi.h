/**
 * @file spi.h
 *
 * @brief SPI module for the WIUS firmware
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 05.04.2024
 *
 * @ingroup wius
 *
 */

#ifndef SPI_H_
#define SPI_H_

#include "common.h"

/**
 * @brief SPI instance enumeration
 *
 */
typedef enum wius_spi_inst
{
  WIUS_SPI_INST_0,  /**< Instance 0 on pins [25, 26, 27, 53] (GSPI Master) */
  WIUS_SPI_INST_1,  /**< Instance 1 on pins [8, 9, 10, 11] (SSI Master) */
  WIUS_SPI_MAX_INST /**< Max instance marker (only used internally) */
} wius_spi_inst_t;

/**
 * @brief Initialize SPI module
 *
 * @param instance: SPI instance to initialize
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_INVALID_PARAMETER: Invalid instance
 * @retval other: Error during peripheral initialization
 *
 */
sl_status_t wius_spi_init(wius_spi_inst_t instance);

/**
 * @brief Transfer data over SPI
 *
 * @param instance: SPI instance to use for transfer
 * @param tx_buf: Pointer to the buffer containing the data to be sent
 * @param rx_buf: Pointer to the buffer where the received data will be stored
 * @param len: Number of bytes to transfer
 * @param wait: Wait for transfer to complete
 *
 * @retval SL_STATUS_OK: Success
 * @retval other: Error during transfer or waiting
 *
 */
sl_status_t wius_spi_xfer(wius_spi_inst_t instance, uint8_t *tx_buf, uint8_t *rx_buf, size_t len, bool wait);

/**
 * @brief Transfer data over SPI instance 0 after @ref wius_spi_xfer was called
 *
 * @param tx_buf: Pointer to the buffer containing the data to be sent
 * @param rx_buf: Pointer to the buffer where the received data will be stored
 * @param len: Number of bytes to transfer
 *
 * @retval SL_STATUS_OK: Success
 * @retval other: Error during transfer or waiting
 *
 * @note This function should take less time for setting up DMA
 * @warning This function works only if DMA for SPI is enabled
 *
 */
sl_status_t wius_spi0_xfer_cont(uint8_t *tx_buf, uint8_t *rx_buf, size_t len);

/**
 * @brief Await SPI transfer completion
 *
 * @param instance: SPI instance to await
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_TIMEOUT: Timeout occured (See @ref WIUS_SPI_RX_TIMEOUT)
 *
 */
sl_status_t wius_spi_await(wius_spi_inst_t instance);

#endif /* SPI_H_ */
