
default:
	g++ -o mcp2221_test -g -I./hidapi/hidapi mcp2221.cpp test.cpp ./hidapi/lib/lib/libhidapi-hidraw.a ./hidapi/lib/lib/libhidapi-libusb.a -ludev
	g++ -o mcp2221_cmd -g -I./hidapi/hidapi mcp2221_cmd.cpp ./hidapi/lib/lib/libhidapi-hidraw.a ./hidapi/lib/lib/libhidapi-libusb.a -ludev

