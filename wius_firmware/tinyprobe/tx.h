/**
 * @file tx.h
 *
 * @brief Commands for the TX of the TinyProbe
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup tinyprobe
 *
 */

#ifndef TP_TX_H_
#define TP_TX_H_

#include "common.h"

/**
 * @brief TX initialization
 *
 * @retval SL_STATUS_OK: Success
 * @retval other: Error during writing to registers
 *
 * @note This function must be called before any other TX function
 *
 */
sl_status_t tp_tx_init(void);

/**
 * @brief Write to a register of the TX
 *
 * @param address: Address of the register to write to
 * @param value: Value to write to the register
 *
 * @retval SL_STATUS_OK: Success
 * @retval other: Error during writing to registers
 *
 */
sl_status_t tp_tx_write_reg(uint16_t address, uint32_t value);

/**
 * @brief Write to a register of the TX and check if the write was successful
 *
 * @param address: Address of the register to write to
 * @param value: Value to write to the register
 *
 * @retval SL_STATUS_OK: Success
 * @retval SL_STATUS_FAIL: Write was not successful
 * @retval other: Error during writing to registers
 *
 */
sl_status_t tp_tx_write_reg_safe(uint16_t address, uint32_t value);

/**
 * @brief Read from a register of the TX
 *
 * @param address: Address of the register to read from
 * @param value: Pointer to the value read from the register
 *
 * @retval SL_STATUS_OK: Success
 * @retval other: Error during reading from registers
 *
 */
sl_status_t tp_tx_read_reg(uint16_t address, uint32_t *value);

#endif /* TP_TX_H_ */