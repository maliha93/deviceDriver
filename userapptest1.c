#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define DEVICE "/dev/chardev1"
#define DEVICE_write "/dev/chardev"

int main(){
	char ch,dummy;
	char read_buf[100], write_buf[100];
	int temp;
	int fd = open(DEVICE, O_RDWR);
	if(fd == -1){
		printf("file either does not exist or has been locked by others");
		exit(-1);
	}
	int fd2 = open(DEVICE_write, O_RDWR);
	printf("r = read from device, w= write to device, enter command: ");
	scanf("%c" , &ch);
	getchar();
	switch (ch)
	{
		case 'w':
		printf("enter data: ");
		scanf("%[^\n]", write_buf);
		write(fd2, write_buf, sizeof(write_buf));
		break;
		case 'r':
		printf("How many chars to read?\n");
		scanf("%d%c",&temp,&dummy);
		read(fd, read_buf, temp);
		printf("device: %s\n", read_buf);
		break;

		default:
		printf("command not recognized\n");
		break;
	}
	return 0;
}