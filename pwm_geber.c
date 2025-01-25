// pwm_geber 
#include <stdio.h>
#include "pico/stdlib.h"


int main() {
	stdio_init_all();
	while (true) {
		printf("Hallo Welt\n");
		sleep_ms(1000);
	}
}

