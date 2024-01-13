#include <stdio.h>
#include <stdint.h>

int main() 
{
	uint64_t x = 0; 
	/*Very secure code, DO NOT USE IN PRODUCTION*/
	scanf("%lu", &x);
	printf("%lu", ++x);
	return 0;
}
