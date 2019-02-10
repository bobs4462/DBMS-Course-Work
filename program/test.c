#include <stdio.h>

int main(void)
{
	int hello; 
	scanf("%d", &hello);
	int array[hello];
	printf("sizof array : %u\n", sizeof(array));
	array[0] = 1000;
	printf("%d", array[0]);
}
