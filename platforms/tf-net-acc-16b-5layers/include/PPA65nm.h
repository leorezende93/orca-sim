// Units
#define POWER_UNIT "mW"
#define PERIOD_UNIT "ns"
#define TECH_NODE "65nm"

// Performance values
#define CONV_CYCLES 14
#define PERIOD_VALUE 1
int RUN_TIME = PERIOD_VALUE * CONV_CYCLES;
float THROUGHPUT = ((32*1000000000.0)/RUN_TIME)/1000000000.0;

// Power values
#define POWER_1TF 31
#define POWER_555 42
#define POWER_AAA 45
#define POWER_FFF 38
#define POWER_FT1 33
#define POWER_STD 28
float AVG_POWER = ((POWER_1TF + POWER_555 + POWER_AAA + POWER_FFF + POWER_FT1 + POWER_STD)/6);

// area values
#define AREA 97890