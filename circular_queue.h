#include<stdio.h>
#define MAX 32


struct queue
{
    int cq[MAX];
    int head=-1;
    int tail=-1;

    int enque(int n)
    {
        int add_item;
        if((head==0 && tail==MAX-1)||(head==tail+1))
        {
            printf("Queue Overflow\n");
            return -1;
        }
        if(head==-1)
        {
            head=0;
            tail=0;
        }
        else
        {
            if(tail==MAX-1)
                tail=0;
            else
                tail=tail+1;
        }

        cq[tail]=n;
        printf("Element inserted.\n");
        return n;
    }

    int deque()
    {
        if(head==-1)
        {
            printf("Queue Underflow\n");
            return -1;
        }
        printf("Element deleted from the Queue is %d\n",cq[head]);
        int to_be_returned = cq[head];

        if(head==tail)
        {
            head=-1;
            tail=-1;
        }
        else
        {
            if(head==MAX-1)
            {
                head=0;
            }
            else
                head=head+1;
        }

        return to_be_returned;
    }

    void display()
    {
        int frontpos= head;
        int rearpos= tail;
        if(head==-1)
        {
            printf("Queue is empty");
            return;
        }
        printf("Queue elements: ");
        if(frontpos<=rearpos)
        {
            while(frontpos<=rearpos)
            {
                printf("%d ",cq[frontpos]);
                frontpos++;
            }
        }
        else
        {
            while(frontpos<=MAX-1)
            {
                printf("%d ",cq[frontpos]);
                frontpos++;
            }
            frontpos=0;
            while(frontpos<=rearpos)
            {
                printf("%d ",cq[frontpos]);
                frontpos++;
            }
        }
        printf("\n");
    }


};


