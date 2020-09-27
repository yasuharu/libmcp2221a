
#include <stdio.h>
#include <string.h>

#include "hidapi.h"
#include "mcp2221.h"

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define PRINT_DEBUG(x, ...) printf(x, __VA_ARGS__) 
#else
#define PRINT_DEBUG(x, ...) {}
#endif

#define READ_TIMEOUT_MS (1000)

// ----- helper api -----
static int mcp2221_validate_gpio_setting(GPIOSetting *setting) {
	if (setting->enable_value < 0 || 1 < setting->enable_value) {
		return STATUS_ARGUMENT_ERROR;
	}
	if (setting->value < 0 || GPIO_VALUE_MAX <= setting->value) {
		return STATUS_ARGUMENT_ERROR;
	}
	if (setting->enable_direction < 0 || 1 < setting->enable_direction) {
		return STATUS_ARGUMENT_ERROR;
	}
	if (setting->direction < 0 || GPIO_DIR_MAX <= setting->direction) {
		return STATUS_ARGUMENT_ERROR;
	}

	return STATUS_OK;
}

// ----- low level api -----
int mcp2221_lowlevel_send(hid_device *dev, uint8_t *data, int length) {
	if (length < 0 || 64 < length) {
		return STATUS_ARGUMENT_ERROR;
	}
	if (data == NULL) {
		return STATUS_ARGUMENT_ERROR;
	}

#ifdef ENABLE_DEBUG
	printf("----- send begin -----\n");
	for (int i = 0 ; i < length ; i++) {
		printf("0x%02x ", data[i]);
		if (i % 16 == 15) {
			printf("\n");
		}
	}
	printf("----- send end -----\n");
#endif

	const int send_size = 64 + 1;
	uint8_t send_data[send_size] = {0};
	memcpy(send_data + 1, data, length);

	if (hid_write(dev, send_data, send_size) != send_size) {
		printf("[ERROR] hid_write error.\n");
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}

int mcp2221_lowlevel_recv(hid_device *dev, uint8_t *data, int length) {
	if (length < 0 || 64 < length) {
		return STATUS_ARGUMENT_ERROR;
	}
	if (data == NULL) {
		return STATUS_ARGUMENT_ERROR;
	}

	const int recv_size = 64;
	uint8_t recv_data[recv_size] = {0};

	if (hid_read_timeout(dev, recv_data, recv_size, READ_TIMEOUT_MS) != recv_size) {
		printf("[ERROR] hid_read_timeout error.\n");
		return STATUS_IO_ERROR;
	}

	memcpy(data, recv_data, length);

#ifdef ENABLE_DEBUG
	printf("----- recv begin -----\n");
	for (int i = 0 ; i < length ; i++) {
		printf("0x%02x ", data[i]);
		if (i % 16 == 15) {
			printf("\n");
		}
	}
	printf("----- recv end -----\n");
#endif

	return STATUS_OK;
}

int mcp2221_lowlevel_init(hid_device **dev) {
	int ret;

	*dev = NULL;

	ret = hid_init();
	if (ret != 0) {
		printf("[ERROR] hid_init error.\n");
		return STATUS_IO_ERROR;
	}

	*dev = hid_open(0x04d8, 0x00dd, NULL);
	if (dev == NULL) {
		printf("[ERROR] hid_open error.\n");
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}

int mcp2221_lowlevel_destroy(hid_device *dev) {
	int ret;

	hid_close(dev);

	ret = hid_exit();
	if (ret != 0) {
		printf("hid_exit error.\n");
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}

// ----- command api -----

// Set GPIO Output Values
// @param cmd cmd needed to allocate 64byte
void mcp2221_command_set_gpio_output(uint8_t cmd[64], GPIOSetting *gp0, GPIOSetting *gp1, GPIOSetting *gp2, GPIOSetting *gp3) {
	memset(cmd, 0, 64);

	cmd[0] = 0x50;

	if (gp0 != NULL) {
		cmd[2] = gp0->enable_value;
		cmd[3] = gp0->value;
		cmd[4] = gp0->enable_direction;
		cmd[5] = gp0->direction;
	}

	if (gp1 != NULL) {
		cmd[6] = gp1->enable_value;
		cmd[7] = gp1->value;
		cmd[8] = gp1->enable_direction;
		cmd[9] = gp1->direction;
	}

	if (gp2 != NULL) {
		cmd[10] = gp2->enable_value;
		cmd[11] = gp2->value;
		cmd[12] = gp2->enable_direction;
		cmd[13] = gp2->direction;
	}

	if (gp3 != NULL) {
		cmd[14] = gp3->enable_value;
		cmd[15] = gp3->value;
		cmd[16] = gp3->enable_direction;
		cmd[17] = gp3->direction;
	}
}

// Get GPIO Values
void mcp2221_command_get_gpio_input(uint8_t cmd[64]) {
	memset(cmd, 0, 64);

	cmd[0] = 0x51;
}

// Set SRAM settings
void mcp2221_command_set_sram_settings(uint8_t cmd[64], SRAMSetting *setting) {
	memset(cmd, 0, 64);

	cmd[0]  = 0x60;

	cmd[1]  = 0;
	cmd[2]  = 0;	// won't be altered
	cmd[3]  = 0;	// won't be altered
	cmd[4]  = 0;	// won't be altered
	cmd[5]  = 0;	// won't be altered
	cmd[6]  = 0;	// won't be altered
	cmd[7]  = (setting->enable_gpio_config & 0x1) << 7;
	cmd[8]  = (setting->gpn_func[0] & 0x7) | ((setting->gpn_gpio_direction[0] & 0x1) << 3) | ((setting->gpn_gpio_value[0] & 0x1) << 4);
	cmd[9]  = (setting->gpn_func[1] & 0x7) | ((setting->gpn_gpio_direction[1] & 0x1) << 3) | ((setting->gpn_gpio_value[1] & 0x1) << 4);
	cmd[10] = (setting->gpn_func[2] & 0x7) | ((setting->gpn_gpio_direction[2] & 0x1) << 3) | ((setting->gpn_gpio_value[2] & 0x1) << 4);
	cmd[11] = (setting->gpn_func[3] & 0x7) | ((setting->gpn_gpio_direction[3] & 0x1) << 3) | ((setting->gpn_gpio_value[3] & 0x1) << 4);
}

// Get SRAM settings
void mcp2221_command_get_sram_setting(uint8_t cmd[64]) {
	memset(cmd, 0, 64);

	cmd[0]  = 0x61;
}

// ----- high layaer api -----
int mcp2221_issue_command(MCP2221Handle *handle, uint8_t send_cmd[64], uint8_t recv_buf[64]) {
	int ret;

	ret = mcp2221_lowlevel_send(handle->dev, send_cmd, 64);
	if (ret != STATUS_OK) return STATUS_IO_ERROR;

	ret = mcp2221_lowlevel_recv(handle->dev, recv_buf, 64);
	if (ret != STATUS_OK) return STATUS_IO_ERROR;

	return STATUS_OK;
}

/*
int mcp2221_setup_port_func(MCP2221Handle *handle, int port, int func) {
	if (port < 0 || 4 < port) {
		return STATUS_ARGUMENT_ERROR;
	}

	if (port == 0 && (func < 0 || GP0_FUNC_MAX <= func)) {
		return STATUS_ARGUMENT_ERROR;
	}

	if (port == 1 && (func < 0 || GP1_FUNC_MAX <= func)) {
		return STATUS_ARGUMENT_ERROR;
	}

	if (port == 2 && (func < 0 || GP2_FUNC_MAX <= func)) {
		return STATUS_ARGUMENT_ERROR;
	}

	if (port == 3 && (func < 0 || GP3_FUNC_MAX <= func)) {
		return STATUS_ARGUMENT_ERROR;
	}

	handle->setting.gpn_func[port] = func;

	return STATUS_OK;
}

int mcp2221_setup_initial_gpio_setting(MCP2221Handle *handle, int port, GPIOSetting *setting) {
	if (port < 0 || 4 < port) {
		return STATUS_ARGUMENT_ERROR;
	}

	if (mcp2221_validate_gpio_setting(setting) != STATUS_OK) {
		return STATUS_ARGUMENT_ERROR;
	}

	handle->setting.gpn_gpio[port].enable_value     = setting->enable_value;
	handle->setting.gpn_gpio[port].value            = setting->value;
	handle->setting.gpn_gpio[port].enable_direction = setting->enable_direction;
	handle->setting.gpn_gpio[port].direction        = setting->direction;

	return STATUS_OK;
}
*/

int mcp2221_write_flash_setting(MCP2221Handle *handle) {

	return STATUS_OK;
}

int mcp2221_read_flash_setting(MCP2221Handle *handle) {

	return STATUS_OK;
}

int mcp2221_write_sram_setting(MCP2221Handle *handle, SRAMSetting *setting) {
	int ret;
	uint8_t cmd[64];
	uint8_t buf[64];

	mcp2221_command_set_sram_settings(cmd, setting);

	if (mcp2221_issue_command(handle, cmd, buf) != STATUS_OK) {
		return STATUS_IO_ERROR;
	}

	if (buf[0] != 0x60 || buf[1] != 0) {
		PRINT_DEBUG("[ERROR] ret = 0x%x\n", buf[0]);
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}

int mcp2221_read_sram_setting(MCP2221Handle *handle, SRAMSetting *setting) {
	int ret;
	uint8_t cmd[64];
	uint8_t buf[64];

	mcp2221_command_get_sram_setting(cmd);

	if (mcp2221_issue_command(handle, cmd, buf) != STATUS_OK) {
		return STATUS_IO_ERROR;
	}

	if (buf[0] != 0x61 || buf[1] != 0) {
		PRINT_DEBUG("[ERROR] ret = 0x%x\n", buf[0]);
		return STATUS_IO_ERROR;
	}

	setting->gpn_func[0] = buf[22] & 0x7;
	setting->gpn_func[1] = buf[23] & 0x7;
	setting->gpn_func[2] = buf[24] & 0x7;
	setting->gpn_func[3] = buf[25] & 0x7;

	setting->gpn_gpio_direction[0] = (buf[22] >> 3) & 0x1;
	setting->gpn_gpio_direction[1] = (buf[23] >> 3) & 0x1;
	setting->gpn_gpio_direction[2] = (buf[24] >> 3) & 0x1;
	setting->gpn_gpio_direction[3] = (buf[25] >> 3) & 0x1;

	setting->gpn_gpio_value[0] = (buf[22] >> 4) & 0x1;
	setting->gpn_gpio_value[1] = (buf[23] >> 4) & 0x1;
	setting->gpn_gpio_value[2] = (buf[24] >> 4) & 0x1;
	setting->gpn_gpio_value[3] = (buf[25] >> 4) & 0x1;

	return STATUS_OK;
}

int mcp2221_set_gpio_direction(MCP2221Handle *handle, int port, GPIODirection direction) {
	int ret;
	uint8_t cmd[64];
	uint8_t buf[64];
	GPIOSetting gpio;

	gpio.enable_value     = 0;
	gpio.value            = GPIO_VALUE_L;
	gpio.enable_direction = 1;
	gpio.direction        = direction;

	if (mcp2221_validate_gpio_setting(&gpio) != STATUS_OK) {
		return STATUS_ARGUMENT_ERROR;
	}

	switch (port) {
		case 0:
			mcp2221_command_set_gpio_output(cmd, &gpio, NULL, NULL, NULL);
			break;
		case 1:
			mcp2221_command_set_gpio_output(cmd, NULL, &gpio, NULL, NULL);
			break;
		case 2:
			mcp2221_command_set_gpio_output(cmd, NULL, NULL, &gpio, NULL);
			break;
		case 3:
			mcp2221_command_set_gpio_output(cmd, NULL, NULL, NULL, &gpio);
			break;
		default:
			return STATUS_ARGUMENT_ERROR;
	}

	if (mcp2221_issue_command(handle, cmd, buf) != STATUS_OK) {
		return STATUS_IO_ERROR;
	}

	// check return value
	if (buf[0] != 0x50) {
		return STATUS_IO_ERROR;
	}

	for (int i = 0 ; i < 4 ; i++) {
		if (i == port) {
			if (
					buf[4 * i + 4] != cmd[4 * i + 4] ||
					buf[4 * i + 5] != cmd[4 * i + 5]
			) {
				return STATUS_IO_ERROR;
			}
		} else {
			if (
					buf[4 * i + 4] != 0xEE ||
					buf[4 * i + 5] != 0xEE
			) {
				return STATUS_IO_ERROR;
			}
		}
	}

	return STATUS_OK;
}

int mcp2221_set_gpio_value(MCP2221Handle *handle, int port, GPIOValue value) {
	int ret;
	uint8_t cmd[64];
	uint8_t buf[64];
	GPIOSetting gpio;

	if (mcp2221_validate_gpio_setting(&gpio) != STATUS_OK) {
		return STATUS_ARGUMENT_ERROR;
	}

	gpio.enable_value     = 1;
	gpio.value            = value;
	gpio.enable_direction = 0;
	gpio.direction        = GPIO_DIR_IN;

	switch (port) {
		case 0:
			mcp2221_command_set_gpio_output(cmd, &gpio, NULL, NULL, NULL);
			break;
		case 1:
			mcp2221_command_set_gpio_output(cmd, NULL, &gpio, NULL, NULL);
			break;
		case 2:
			mcp2221_command_set_gpio_output(cmd, NULL, NULL, &gpio, NULL);
			break;
		case 3:
			mcp2221_command_set_gpio_output(cmd, NULL, NULL, NULL, &gpio);
			break;
		default:
			return STATUS_ARGUMENT_ERROR;
	}

	if (mcp2221_issue_command(handle, cmd, buf) != STATUS_OK) {
		return STATUS_IO_ERROR;
	}

	// check return value
	if (buf[0] != 0x50) {
		return STATUS_IO_ERROR;
	}

	for (int i = 0 ; i < 4 ; i++) {
		if (i == port) {
			if (
					buf[4 * i + 2] != cmd[4 * i + 2] ||
					buf[4 * i + 3] != cmd[4 * i + 3]
			) {
				return STATUS_IO_ERROR;
			}
		} else {
			if (
					buf[4 * i + 2] != 0xEE ||
					buf[4 * i + 3] != 0xEE
			) {
				return STATUS_IO_ERROR;
			}
		}
	}

	return STATUS_OK;
}

int mcp2221_get_gpio_value(MCP2221Handle *handle, GPIOValue *gp0, GPIOValue *gp1, GPIOValue *gp2, GPIOValue *gp3) {
	int ret;
	uint8_t cmd[64];
	uint8_t buf[64];

	mcp2221_command_get_gpio_input(cmd);

	if (mcp2221_issue_command(handle, cmd, buf) != STATUS_OK) {
		return STATUS_IO_ERROR;
	}

	if (buf[0] != 0x51) {
		return STATUS_IO_ERROR;
	}

	*gp0 = (GPIOValue)buf[3];
	*gp1 = (GPIOValue)buf[5];
	*gp2 = (GPIOValue)buf[7];
	*gp3 = (GPIOValue)buf[9];

	return STATUS_OK;
}

int mcp2221_init(MCP2221Handle *handle) {
	int ret;
	ret = mcp2221_lowlevel_init(&handle->dev);

	if (ret < 0) {
		printf("mcp2221_init error.\n");
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}

int mcp2221_destroy(MCP2221Handle *handle) {
	int ret;
	ret = mcp2221_lowlevel_destroy(handle->dev);

	if (ret < 0) {
		printf("mcp2221_destroy error.\n");
		return STATUS_IO_ERROR;
	}

	return STATUS_OK;
}
