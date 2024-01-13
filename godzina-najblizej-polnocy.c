#include <stdio.h>
#include <stdint.h>

uint32_t n_iterations, min_time = 12 * 60;

int main(void)
{
	scanf("%u", &n_iterations);
	for (int i = 0; i < n_iterations; i++) {
		uint32_t hours = 0, minutes = 0;
		scanf("%u %u", &hours, &minutes);
		if (hours == 12) hours = 0;
		uint32_t total_time = hours * 60 + minutes;
		if (total_time < min_time) min_time = total_time;
	}
	printf("%u %u\n", min_time / 60 == 0 ? 12 : min_time / 60, min_time % 60);
	return 0;
}
