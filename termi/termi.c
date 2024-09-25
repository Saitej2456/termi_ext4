#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

void list_contents(char * path)
{
    DIR * dirp = opendir(path);
   if(dirp == NULL)
   {
        printf("couldnt open or the directory is empty");
   }
   struct dirent * direntp;
    while((direntp = readdir(dirp))!= NULL)
    {
        if(direntp->d_name[0] == '.')
        {

        }
        else 
        {
            printf(" %s\n",direntp->d_name);
        }
    }
    closedir(dirp);
    sleep(1);
}

void making_dir(char * path)
{
    if (mkdir(path) == 0)
    {
        printf("the folder has been created");
    }
    else 
    {
        perror("error occured while making a folder");
    }
}

void read_file(char * path)
{
    FILE * filep = fopen(path,"r");
    if (filep == NULL)
    {
        perror("error opening the file");
        return ;
    }
    else 
    {
        char buffer[512];
        while(fgets(buffer,sizeof(buffer),filep))
        {
            printf("%s",buffer);
        }
        fclose(filep);
    
    }
}

void write_file(char * path)
{
    
    if(access(path,F_OK)==0)
    {
            printf("the contents before writing:\n");
            read_file(path);
    }
    else if (access(path,F_OK)==-1)
    {
        printf("creating a new file");
    }
    FILE * fp = fopen(path,"w");
    if(fp == NULL)
    {
        perror("file did not open");
    }
    else 
    {
        printf("\nenter the new data in here :");
        char file_data[256];
        fgets(file_data,sizeof(file_data),stdin);
        fprintf(fp,"%s",file_data);
        printf("\nfinished entering data \nThe current data is:\n");
        fclose(fp);
        read_file(path);
    }
}

void remove_file(char * path)
{
    if(remove(path)==0)
    {
        printf("the file hass been succesfully removed");
    }
    else
    {
        perror("the file is not removed");
    }
}

void screen_clearer(int n)
{
    for(int i = 0 ; i < n ; i++)
    {
        printf("\n");
    }
}
void printf_logo1()
{
    printf("\033[35m");
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______          \n"
            "|\\  ___ \\         \n"
            "\\ \\   __/|        \n"
            " \\ \\  \\_|/__       \n"
            "  \\ \\  \\_|\\ \\      \n"
            "   \\ \\_______\\       \n"
            "    \\|_______|        \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________ \n"
            "|\\  ___ \\         |\\   ___  \\\n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\\n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\\n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\\n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___\n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\\n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\\n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\\n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\\n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________\n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\\n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|\n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\ ___\n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\\n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|        \\|_______|\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______\n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\\n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\\n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\\n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\\n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\       \\ \\__\\    \\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|        \\|_______|        \\|__|     \\|__|\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\       \\ \\  \\ \\  \\ \n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\       \\ \\__\\    \\ \\__\\       \\ \\__\\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|        \\|_______|        \\|__|     \\|__|        \\|__|\\|__|\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf("\033[0m");
}

void printf_logo2()
{
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\       \\ \\  \\ \\  \\ \n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\       \\ \\  \\ \\  \\ \n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\       \\ \\__\\    \\ \\__\\       \\ \\__\\ \\__\\\n"
            );
    screen_clearer(20);
    sleep(1);
    screen_clearer(50);
    printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\       \\ \\  \\ \\  \\ \n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\       \\ \\__\\    \\ \\__\\       \\ \\__\\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|        \\|_______|        \\|__|     \\|__|        \\|__|\\|__|\n"
            );
    screen_clearer(20);
    sleep(1);
    printf("\033[0m");
    screen_clearer(50);
}
/*
printf("\033[43m\033[1;30mThis text is white on a purple background.\033[0m\n");for yellow background and black text
printf(
            " _______           ________           ___          ________          _____ ______           ________     \n"
            "|\\  ___ \\         |\\   ___  \\        |\\  \\        |\\   ____\\        |\\   _ \\  _   \\        |\\   __  \\    \n"
            "\\ \\   __/|        \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\___|        \\ \\  \\ \\__\\ \\  \\       \\ \\  \\__\\ \\   \n"
            " \\ \\  \\_|/__       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\  ___       \\ \\  \\\\|__| \\  \\       \\ \\   __  \\  \n"
            "  \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\       \\ \\  \\       \\ \\  \\|   \\       \\ \\  \\    \\ \\  \\       \\ \\  \\ \\  \\ \n"
            "   \\ \\_______\\       \\ \\__\\\\ \\__\\       \\ \\__\\       \\ \\_______\\       \\ \\__\\    \\ \\__\\       \\ \\__\\ \\__\\\n"
            "    \\|_______|        \\|__| \\|__|        \\|__|        \\|_______|        \\|__|     \\|__|        \\|__|\\|__|\n"
            );
    printf("\033[0m");
*/
int main()
{
    
    //printf_logo1();
    printf_logo2();
    printf("\033[1;37m");
    printf("Hello! welcome to the terminal file editor for ext4 systems from windows\033[0m\n");
    // printf("operations available are :\n1.list the conetnts in a directory\n2.make a directory\n3.read the contnets of a file\n4.write contents into a file\n5.remove a file\n6.exit");
    printf("\033[4;33m");
    printf("\nCommands :\033[0m\n1.listing = list\n2.making a directory = mkdir\n3.reading a file = read\n4.writing a file = write\n5.removing a file = rm\n6.exiting = exit");
    printf("\n\nsyntax: command <path>");
    char command[128];
    char path[128];
    while(1)
    {
        printf("\n\033[1;31m-->\033[0m");
        fgets(command,sizeof(command),stdin);
        if(sscanf(command,"list %s",path) == 1)
        {
            list_contents(path);
        }
        else if(sscanf(command,"mkdir %s",path)==1)
        {
            making_dir(path);
        }
        else if(sscanf(command,"read %s",path)==1)
        {
            read_file(path);
        }
        else if (sscanf(command,"write %s",path)==1)
        {
            write_file(path);
        }
        else if (sscanf(command,"rm %s",path))
        {
            remove_file(path);
        }
        else if(strcmp(command,"exit\n")==0)
        {
            printf("thank you for using our services!");
            break;
        }
        else 
        {
            printf("no such command doesnt exist");
        }
    }
}
