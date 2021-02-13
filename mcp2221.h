
#ifndef __MCP2221_H__
#define __MCP2221_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hidapi.h"

enum GPIOValue {
	GPIO_VALUE_L = 0,
	GPIO_VALUE_H,
	GPIO_VALUE_MAX,
};

enum GPIODirection {
	GPIO_DIR_OUT = 0,
	GPIO_DIR_IN,
	GPIO_DIR_MAX,
};

enum GP0Function {
	GP0_FUNC_GPIO = 0,
	GP0_FUNC_SSPND,
	GP0_FUNC_LED_UART_RX,
	GP0_FUNC_MAX,
};

enum GP1Function {
	GP1_FUNC_GPIO = 0,
	GP1_FUNC_CLK_OUTPUT,
	GP1_FUNC_ADC1,
	GP1_FUNC_LED_UART_TX,
	GP1_FUNC_INT,
	GP1_FUNC_MAX,
};

enum GP2Function {
	GP2_FUNC_GPIO = 0,
	GP2_FUNC_USB_CFG,
	GP2_FUNC_ADC2,
	GP2_FUNC_DAC2,
	GP2_FUNC_MAX,
};

enum GP3Function {
	GP3_FUNC_GPIO = 0,
	GP3_FUNC_LED_I2C,
	GP3_FUNC_ADC3,
	GP3_FUNC_DAC3,
	GP3_FUNC_MAX,
};

typedef struct _SRAMSetting {
	// byte 2 : Clock output divider value
	// not impl

	// byte 3 : DAC voltage reference
	// not impl

	// byte 4 : Set DAC output value
	// not impl

	// byte 5 : ADC voltage reference
	// not impl

	// byte 6 : Setup the interrupt
	// not impl

	// byte 7 : Alter GPIO configuration
	int enable_gpio_config;

	// byte 8 : GP0 setting
	// byte 9 : GP1 setting
	// byte 10 : GP2 setting
	// byte 11 : GP3 setting
	int gpn_func[4];
	int gpn_gpio_value[4];
	int gpn_gpio_direction[4];
} SRAMSetting;

typedef struct _GPIOSetting {
	int           enable_value;
	GPIOValue     value;
	int           enable_direction;
	GPIODirection direction;
} GPIOSetting;

typedef struct _MCP2221Handle {
	hid_device *dev;

	SRAMSetting setting;
} MCP2221Handle;

#define STATUS_OK (0)
#define STATUS_IO_ERROR (1)
#define STATUS_ARGUMENT_ERROR (2)

int mcp2221_write_sram_setting(MCP2221Handle *handle, SRAMSetting *setting);
int mcp2221_read_sram_setting(MCP2221Handle *handle, SRAMSetting *setting);
int mcp2221_set_gpio_direction(MCP2221Handle *handle, int port, GPIODirection dir);
int mcp2221_get_gpio_direction(MCP2221Handle *handle, int port , GPIODirection *dir);
int mcp2221_set_gpio_value(MCP2221Handle *handle, int port, GPIOValue value);
int mcp2221_get_gpio_value(MCP2221Handle *handle, int port, GPIOValue *value);
int mcp2221_init(MCP2221Handle *handle);
int mcp2221_destroy(MCP2221Handle *handle);

#ifdef __cplusplus
}
#endif

#endif

