
#include <stdio.h>

#define CHECK_EQ(val, exp) { int v = (val); int e = (exp); if (v == e) { printf("."); } else { printf("[TEST FAIL] %s@%d\n", __FUNCTION__, __LINE__); return 1; } }

