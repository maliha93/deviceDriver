#include<stdio.h>
#include "circular_queue.h"
#define max 32


int main()
{
    int choice, inserted;
    struct queue buffer;

    while(1)
    {
        printf("\nPress 1 to Enqueue\n");
        printf("Press 2 to Dequeue\n");
        printf("Press 3 to Display\n");
        printf("Press 4 to Exit\n");
        printf("Enter your choice:");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1:
            printf("\n Input the element for insertion in queue:");
            scanf("%d", &inserted);
            buffer.enque(inserted);
            break;
        case 2:
            buffer.deque();
            break;
        case 3:
            buffer.display();
            break;
        case 4:
            return 0;
        default:
            printf("\n Invalid Choice:");
            break;
        }
    }
    getchar();
}
