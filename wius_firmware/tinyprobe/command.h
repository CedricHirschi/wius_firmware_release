/**
 * @file command.h
 *
 * @brief Command handling for the TinyProbe
 *
 * @author Cédric Hirschi, ETH Zürich
 * @date 08.04.2024
 *
 * @ingroup tinyprobe
 *
 */

#ifndef TP_COMMAND_H_
#define TP_COMMAND_H_

#include "common.h"

/**
 * @brief Command IDs enumeration
 *
 */
typedef enum tp_cmd_id
{
	TP_CMD_PING = 0,
	TP_CMD_EN_REPLIES,
	TP_CMD_SW_MUX,
	TP_CMD_WRITE_SPI,
	TP_CMD_WRITE_FPGA,
	TP_CMD_WRITE_AFE,
	TP_CMD_WRITE_TX,
	TP_CMD_DELAY_NS,
	TP_CMD_SLEEP_MS,
	TP_CMD_CTRL_PWR,
	TP_CMD_TRIGGER_SHOT,
	TP_CMD_ID_MAX
} tp_command_id_t;

/**
 * @brief Command structure
 *
 * <table class="tg">
 * <tbody>
 *   <tr>
 *     <th class="tg-1wig">Byte</th>
 *     <th class="tg-0lax">0</th>
 *     <th class="tg-0lax">1</th>
 *     <th class="tg-0lax">2</th>
 *     <th class="tg-0lax">3</th>
 *     <th class="tg-0lax">4</th>
 *     <th class="tg-0lax">5 ... (4 + ARG_LEN_0)</th>
 *     <th class="tg-0lax">(5 + ARG_LEN_0)</th>
 *     <th class="tg-0lax">...</th>
 *   </tr>
 *   <tr>
 *     <td class="tg-1wig">Description</td>
 *     <td class="tg-0lax" colspan="2">number of commands<br><span style="font-style:italic">[uint16_t]</span></td>
 *     <td class="tg-0lax">Command 0 ID<br><span style="font-style:italic">[uint8_t]</span></td>
 *     <td class="tg-0lax" colspan="2">CMD arguments length<br><span style="font-style:italic">[uint16_t]</span></td>
 *     <td class="tg-0lax">Argument 0</td>
 *     <td class="tg-0lax">Argument 1</td>
 *     <td class="tg-0lax">...</td>
 *   </tr>
 * </tbody>
 * </table>
 *
 */
typedef struct tp_cmd
{
	tp_command_id_t id;
	uint8_t *args;
	size_t args_length;
} tp_command_t;

/**
 * @brief Parse a command from a buffer
 *
 * @param buffer The buffer containing the command
 * @param buffer_length The length of the buffer
 *
 * @return The parsed command
 *
 */
tp_command_t *tp_command_parse(uint8_t *buffer, size_t buffer_length);

/**
 * @brief Execute a command
 *
 * @param command The command to execute
 *
 * @return The status of the command execution
 *
 */
sl_status_t tp_command_execute(tp_command_t command);

/**
 * @brief Parse and execute a command from a buffer
 *
 * @param buffer The buffer containing the command
 * @param buffer_length The length of the buffer
 *
 * @return The status of the command execution
 *
 * @note This function is a wrapper around @ref tp_parse_command and @ref tp_execute_command
 *
 */
sl_status_t tp_command_parse_and_execute(uint8_t *buffer, size_t buffer_length);

#endif /* TP_COMMAND_H_ */
