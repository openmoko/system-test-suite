#include "dm_init.h"

#define BATTERY_VOLTAGE "/sys/bus/i2c/drivers/pcf50633/0-0073/battvolt"

int get_voltage(const char* device ,int type)
{
	FILE *fp = NULL;
	char battery_voltage_value[8];
	int i, total = 0;
	int average = 0;

	/* Confirm device initialized */
	if (access(device, R_OK) != 0) {
		goto err;
	}

	for (i = 0; i< 10; i++) {
                if ((fp = fopen(device, "r"))) {
                        if (fread(battery_voltage_value, sizeof(char), 5, fp))
                        {
                                total += atoi(battery_voltage_value);
			}
			else {
				fclose(fp);
				break;
			}
		} else
			goto err;
	}

	fclose(fp);

	if (i == 10) {
		average = total / 10;
		if(total!=0)
		{
			if( average != 65536 ){
				printf("[BATTERY] %4d mV\n", average);
			}
			else{   	
				printf("[BATTERY] 65536 V : Fail");	
				return 0;
			}
		}
		else
			printf("[BATTERY] 0 V : Fail");

		return 1;

	} else {
err:		
	        printf("[BATTERY] Fail ");
		return 0;
	}
}

void battery_test(void)
{
        get_voltage(BATTERY_VOLTAGE, 0);
}

