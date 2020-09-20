
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hidapi.h"

#define READ_TIMEOUT_MS (1000)

int mcp2221_send(hid_device *dev, uint8_t *data, int length) {
	if (length < 0 || 64 < length) {
		return -1;
	}
	if (data == NULL) {
		return -1;
	}

	const int send_size = 64 + 1;
	uint8_t send_data[send_size] = {0};
	memcpy(send_data + 1, data, length);

	return hid_write(dev, send_data, send_size);
}

int mcp2221_recv(hid_device *dev, uint8_t *data, int length) {
	int ret;

	if (length < 0 || 64 < length) {
		return -1;
	}
	if (data == NULL) {
		return -1;
	}

	const int recv_size = 64;
	uint8_t recv_data[recv_size] = {0};

	ret = hid_read_timeout(dev, recv_data, recv_size, READ_TIMEOUT_MS);

	memcpy(data, recv_data, length);

	return ret;
}

int mcp2221_init(hid_device **dev) {
	int ret;

	*dev = NULL;

	ret = hid_init();
	if (ret != 0) {
		printf("hid_init error.\n");
		return 1;
	}

	*dev = hid_open(0x04d8, 0x00dd, NULL);
	if (dev == NULL) {
		printf("hid_open error.\n");
		return 1;
	}

	return 0;
}

int mcp2221_destroy(hid_device *dev) {
	int ret;

	hid_close(dev);

	ret = hid_exit();
	if (ret != 0) {
		printf("hid_exit error.\n");
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	int ret;
	hid_device *dev;

	if (argc < 2) {
		printf("usage : ./main <param1> <param2> ...\n");
		return 1;
	}

	ret = mcp2221_init(&dev);
	if (ret < 0) {
		printf("mcp2221_init error.\n");
		return 1;
	}

	const int pnum = argc - 1;
	uint8_t send_data[64] = {0};

	for (int i = 0 ; i < pnum ; i++) {
		uint8_t val = strtol(argv[1 + i], NULL, 0);
		send_data[i] = val;
	}

	ret = mcp2221_send(dev, send_data, pnum);
	if (ret < 0) {
		printf("mcp2221_send error.\n");
		return 1;
	}
	printf("send %d bytes\n", ret);

	uint8_t recv_data[64] = {0};
	ret = mcp2221_recv(dev, recv_data, sizeof(recv_data) / sizeof(uint8_t));
	if (ret < 0) {
		printf("mcp2221_recv error.\n");
		return 1;
	}
	printf("recv %d bytes\n", ret);

	for (int i = 0 ; i < sizeof(recv_data) / sizeof(uint8_t) ; i++) {
		printf("0x%02x ", recv_data[i]);
		if (i % 16 == 15) {
			printf("\n");
		}
	}

	ret = mcp2221_destroy(dev);
	if (ret < 0) {
		printf("mcp2221_destroy error.\n");
		return 1;
	}
}
