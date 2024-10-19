#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int  main ()
{
    char str1[] = "fuck you";
    char str2[] = "fuck you";    
    if(memcmp(str1,str2,9)== 0)
    {
        printf("bruh");
    }
    else
    {
        printf("wtf");
    }
}