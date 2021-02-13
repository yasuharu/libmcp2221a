
#include "mcp2221.h"
#include "test.h"

int test_sram_setting() {
	MCP2221Handle handle;

	CHECK_EQ(mcp2221_init(&handle), STATUS_OK);

	SRAMSetting setting;

	setting.enable_gpio_config = 1;

	setting.gpn_func[0]           = 0;
	setting.gpn_gpio_direction[0] = 1;
	setting.gpn_gpio_value[0]     = 1;

	setting.gpn_func[1]           = 1;
	setting.gpn_gpio_direction[1] = 1;
	setting.gpn_gpio_value[1]     = 0;

	setting.gpn_func[2]           = 2;
	setting.gpn_gpio_direction[2] = 1;
	setting.gpn_gpio_value[2]     = 1;

	setting.gpn_func[3]           = 3;
	setting.gpn_gpio_direction[3] = 1;
	setting.gpn_gpio_value[3]     = 0;

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

int test_gpio_direction() {
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

	GPIODirection dirs[4];

	CHECK_EQ(mcp2221_set_gpio_direction(&handle, 0, GPIO_DIR_IN) , STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_direction(&handle, 1, GPIO_DIR_OUT), STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_direction(&handle, 2, GPIO_DIR_IN) , STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_direction(&handle, 3, GPIO_DIR_OUT), STATUS_OK);

	CHECK_EQ(mcp2221_get_gpio_direction(&handle, 0, &dirs[0]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_direction(&handle, 1, &dirs[1]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_direction(&handle, 2, &dirs[2]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_direction(&handle, 3, &dirs[3]), STATUS_OK);

	CHECK_EQ(dirs[0], GPIO_DIR_IN);
	CHECK_EQ(dirs[1], GPIO_DIR_OUT);
	CHECK_EQ(dirs[2], GPIO_DIR_IN);
	CHECK_EQ(dirs[3], GPIO_DIR_OUT);

	CHECK_EQ(mcp2221_destroy(&handle), STATUS_OK);
}

int test_gpio_value() {
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

	GPIOValue values[4];

	CHECK_EQ(mcp2221_set_gpio_value(&handle, 0, GPIO_VALUE_L), STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_value(&handle, 1, GPIO_VALUE_H), STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_value(&handle, 2, GPIO_VALUE_L), STATUS_OK);
	CHECK_EQ(mcp2221_set_gpio_value(&handle, 3, GPIO_VALUE_H), STATUS_OK);

	CHECK_EQ(mcp2221_get_gpio_value(&handle, 0, &values[0]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_value(&handle, 1, &values[1]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_value(&handle, 2, &values[2]), STATUS_OK);
	CHECK_EQ(mcp2221_get_gpio_value(&handle, 3, &values[3]), STATUS_OK);

	CHECK_EQ(values[0], GPIO_VALUE_L);
	CHECK_EQ(values[1], GPIO_VALUE_H);
	CHECK_EQ(values[2], GPIO_VALUE_L);
	CHECK_EQ(values[3], GPIO_VALUE_H);

	CHECK_EQ(mcp2221_destroy(&handle), STATUS_OK);
}

int main(int argc, char* argv[]) {
	test_sram_setting();
	test_gpio_direction();

	printf("test success.\n");
}

