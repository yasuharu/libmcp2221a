
#include "mcp2221.h"
#include "test.h"

int main(int argc, char* argv[]) {
	MCP2221Handle handle;

	CHECK_EQ(mcp2221_init(&handle), STATUS_OK);

	SRAMSetting setting;

	setting.enable_gpio_config = 1;

	setting.gpn_func[0]           = 0;
	setting.gpn_gpio_direction[0] = 1;
	setting.gpn_gpio_value[0]     = 1;

	setting.gpn_func[1]           = 0;
	setting.gpn_gpio_direction[1] = 1;
	setting.gpn_gpio_value[1]     = 1;

	setting.gpn_func[2]           = 0;
	setting.gpn_gpio_direction[2] = 1;
	setting.gpn_gpio_value[2]     = 1;

	setting.gpn_func[3]           = 0;
	setting.gpn_gpio_direction[3] = 1;
	setting.gpn_gpio_value[3]     = 1;

	CHECK_EQ(mcp2221_write_sram_setting(&handle, &setting), STATUS_OK);

	SRAMSetting read_setting;

	CHECK_EQ(mcp2221_read_sram_setting(&handle, &read_setting), STATUS_OK);

	CHECK_EQ(setting.gpn_func[0], read_setting.gpn_func[0]);
	CHECK_EQ(setting.gpn_func[1], read_setting.gpn_func[1]);
	CHECK_EQ(setting.gpn_func[2], read_setting.gpn_func[2]);
	CHECK_EQ(setting.gpn_func[3], read_setting.gpn_func[3]);

	CHECK_EQ(setting.gpn_gpio_value[0], read_setting.gpn_gpio_value[0]);
	CHECK_EQ(setting.gpn_gpio_value[1], read_setting.gpn_gpio_value[1]);
	CHECK_EQ(setting.gpn_gpio_value[2], read_setting.gpn_gpio_value[2]);
	CHECK_EQ(setting.gpn_gpio_value[3], read_setting.gpn_gpio_value[3]);

	CHECK_EQ(setting.gpn_gpio_direction[0], read_setting.gpn_gpio_direction[0]);
	CHECK_EQ(setting.gpn_gpio_direction[1], read_setting.gpn_gpio_direction[1]);
	CHECK_EQ(setting.gpn_gpio_direction[2], read_setting.gpn_gpio_direction[2]);
	CHECK_EQ(setting.gpn_gpio_direction[3], read_setting.gpn_gpio_direction[3]);

	CHECK_EQ(mcp2221_destroy(&handle), STATUS_OK);
}

