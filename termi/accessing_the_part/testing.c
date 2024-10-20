#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <stdbool.h>


void nl(int a)
{
    for(int i = 0 ; i < a ; i ++)
    {
        printf("\n");
    }
    
}
void print_partitionname(char * lable_name)
{
    unsigned char byte1 ;
    unsigned char byte2 ;
    unsigned short utf16_char;
     nl(1);
    printf("The Label is :");
    for(int i = 0 ; i < 72 ; i=i+2)
    {
        byte1 = lable_name[i];
        byte2 = lable_name[i+1];
        utf16_char = (byte2 << 8) | byte1;
        
        if (utf16_char == 0)
        {
            break;
        } 
        printf("%c",(char)utf16_char);
    }
}

void MBR_checker(HANDLE  h, int LBA)
{
    printf("\nchecking MBR boot signature...\n");
    LARGE_INTEGER sector_offest;

    // char buffer[2];
    //we cant directly use 2 bytes as we are using the raw data from the physical disk we have to follow some of the rules to access data

    char buffer[512];
    DWORD bytes_read;
    sector_offest.QuadPart = 0;
    SetFilePointerEx(h,sector_offest,NULL,FILE_BEGIN);
    ReadFile(h,buffer,sizeof(buffer),&bytes_read,NULL);

    //tester//printf("\nthis is the value of bytes read %d\n",bytes_read);
    //tester to know if the buffer is actually getting the data nessasary

    //printf("The boot signature of MBR is : %02X %02X\n",(u_char)buffer[510],(u_char)buffer[511]);
    //tester to know which charecters are there in the boot signature area
    if(((int)buffer[510]==85)&&((int)buffer[511]==-86))
    {
        //85 in hex is 55 and -86 is aa
        printf("the boot signature is valid");
        
    }
    else 
    {
        printf("invalid MBR boot signature has been found in the first 512 bytes");
        exit(1);
    }
    nl(1); 
}

unsigned long long int extractor(char * buffer)
{
    unsigned long long int SLBA = 0;
    for(int i = 0 ; i < 8 ; i ++)
    {
        //using to explicitly cutdown the padding 1 bits which are actually due to a padding error 
        unsigned long long int temp = buffer[i];
        temp = temp & (unsigned)255;
        
        //unsigned long long int temp = (unsigned long long int)buffer[i];
        //reason for not using this is that when i tried to use it , the typecasting still wasnt working ans was giving wrong values 
        //example for 0xc7
        //since the binary value is 11000111
        //and the sign bit here is 1 it is assuming the number is negative and is trying to padd the new bits with 1 instead of 0
        //so when printing we are getting the wrong values 

        //tester//printf("\nthe temp is %X\n",temp);
        //using this tester to know what is the value in temp to know if the padding is done correctly or not 
        
        //tester// printf("%X ",(u_char)buffer[i]);
        //using to know if the buffer is correct 

        SLBA = SLBA | temp<<(i*8);
        
        //tester//printf("\nthe starting block is %08X\n",SLBA);
        //using to know if the SLBA is containing the correct values in hex format
    }
    //tester// printf("\nthe starting block is %llu\n",SLBA);
    //using this to know if the value printed is the correct SLBA value in decimal

    return SLBA;
    
}

void ext4_checker(HANDLE h, unsigned long long int * SLBA_arr, int partition_count)
{
    LARGE_INTEGER sector_offset;
    int count = 0;
    for (int i = 0; i < partition_count; i++)
    {
        // Calculate the offset to reach the super block(SLBA + 1024 )
        sector_offset.QuadPart = (SLBA_arr[i] * 512) + 1024;

        char buffer[1024];
        DWORD bytes_read;

        SetFilePointerEx(h, sector_offset, NULL, FILE_BEGIN);
        ReadFile(h, buffer, sizeof(buffer), &bytes_read, NULL);

        //used to print the data in the super block of size 1024
        //tester//printf("printing the super block data of partition %d\n",i+1);
        // for(int j = 0 ; j < 1024 ; j ++)
        // {
        //     printf("%02X ", (u_char)buffer[j]);
            
        //     if((j+1)%16 == 0)
        //     {
        //         printf("\n");
        //     }
        // }

        // Print the magic number in correct order (little-endian)
        //tester printf("Number found partition %d in Magic number offset: %02X%02X\n", i+1, (u_char)buffer[57], (u_char)buffer[56]);
        //used to check the values of the partitions at magic number offset 
        if (((u_char)buffer[57]==239)&&((u_char)buffer[56]==83))
        {
            printf("This partition %d is an Ext4 partition as it has the magic numeber is 0XEF53",i+1);
            nl(1);
        }
    }
}

void partition_data_printer(HANDLE h,LARGE_INTEGER sector_offest,int LBA)
{
    // sector_offest.QuadPart =0 0th LBA(MBR);
    // sector_offest.QuadPart =512 1st LBA(GPT header);
    char buffer[512];
    DWORD bytes_read;
    
    //helps to keep track of whether the pointer is pointing to lable byte or not
    int lable_count =0;
    
    //helps to keep track of the partitions 
    int count =0;

    //helps to keep track of whether the pointer is pointing to starting LBA containing byte or not
    int start_LBA_count = 0;


    //using to store the SLBAS of the partition entries
    unsigned long long int SLBA_arr[128];
    for(int j = 2; j < 34; j++)
    {
        //helps with the storage of the lable 
        char lable_buffer[73] ;

        //used to store the starting LBA of the partitions 
        unsigned long long int SLBA;

        //flag_for_lable helps with identifying the lable starting byte for each partition 
        BOOL flag_lable = true;

        //helps to find the first LBA of the partition
        BOOL flag_partiti_startLBA = true;

        LBA = j;
        sector_offest.QuadPart =512*LBA;
        //partition entries in 2nd LBA to 33LBA

        //SetFilePointerEx(HANDLE,sector_ofset,new position storer,start from)
        //sector_offset can be given with making a long integer variable and setting the .Quadpart to be the dsired byte value 
        SetFilePointerEx(h,sector_offest,NULL,FILE_BEGIN);
        ReadFile(h,buffer,sizeof(buffer),&bytes_read,NULL);
        printf("\nbytes read are : %d",bytes_read);
        nl(1);
        for (size_t i = 0; i < 512 ; i++)
        {
            if (flag_partiti_startLBA == true)
            {
                if(start_LBA_count == 32)
                {
                    char start_LBA[9] = {0};
                    int l = i;
                    for(int k = 0 ; k < 8 ; k++)
                    {
                        start_LBA[k] = buffer[l];
                        l++;
                    }
                    start_LBA[8] ='\0';
                    // nl(1);
                    // printf("the starting LBA bytes start from here :");
                    // for(int k = 0 ; k < 8 ; k++)
                    // {
                    //     printf("%02X ",(u_char)start_LBA[k]);
                    // }
                    SLBA = extractor(start_LBA);
                    // nl(1);
                    flag_partiti_startLBA = false;
                    start_LBA_count = 0;
                }
                else
                {
                    start_LBA_count++;
                }
            }
            if (flag_lable == true)
            {
                
                if(lable_count == 56)
                {
                    
                    //tester to know which is the 56th byte 
                    //printf("\nit is 56th byte");
                    int l = i;
                    for(int k = 0 ; k < 72 ; k++)
                    {
                        lable_buffer[k] = buffer[l++];
                    }
                    lable_buffer[72] = '\0';
                    //tester for knowing what is in the lable_buffer
                    // for(int k = 0 ; k < 72 ; k++)
                    // {
                    //     printf("%02X ",(unsigned char)lable_buffer[k]);
                    // }
                    // nl();
                    
                    lable_count = 0;
                    flag_lable = false;
                }
                else
                {
                    lable_count++;
                }
            }
            printf("%02X ", (u_char)buffer[i]);
            if((i+1)%16 == 0)
            {
                printf("\n");
            }
            if((i+1)%128 == 0)
            {
                SLBA_arr[count]=SLBA;
                count++;
                flag_lable = true;
                flag_partiti_startLBA = true;
                printf("Above is partition %d data:",count);
                print_partitionname(lable_buffer);
                printf("\nThe starting block is %llu",SLBA);
                nl(2);

            }
        }
        
    }

    nl(1);

    //gives the number of partitions which are there in system
    int part_num_count=0;
    for(int i = 0 ; i < 128 ; i ++)
    {
        if (SLBA_arr[i]==0)
        {
            break;
        }
        part_num_count++;
        //tester printf("%llu ",SLBA_arr[i]);
        //tester to know if the stored SLBA values are correct 
    }
    nl(1);
    //tester// printf("%d",part_num_count);
    //using to test if its capturing the correct number of partition entries with non zero data
    
    ext4_checker(h,SLBA_arr,part_num_count);
    
}


int main()
{

    HANDLE h;
    DWORD bytes_read;
    /*
    HANDLE CreateFile(    
         LPCWSTR               lpFileName,   
         DWORD                 dwDesiredAccess,    
         DWORD                 dwShareMode,    
         LPSECURITY_ATTRIBUTES lpSecurityAttributes,    
         DWORD                 dwCreationDisposition,    
         DWORD                 dwFlagsAndAttributes,    
         HANDLE                hTemplateFile  );  
         */
    char PATH[MAX_PATH] = "\\\\.\\PHYSICALDRIVE0" ;
    h = CreateFile(PATH,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if (h == INVALID_HANDLE_VALUE)
    {
        printf("error msg : %lu",GetLastError());
        CloseHandle(h);
        return 1;
    }

    // char lable_str[73];
    //check into this later 
    //issue : when declared getting an output which is random for each execution when the partition data printer was in main 

    char buffer[512];

    //for  setting the offset with .Quadpart
    LARGE_INTEGER sector_offest;
    // each LBA is of size 512
    int LBA;
    LBA = 0;

    //will check if there is a valid MBR boot signature
    MBR_checker(h,LBA);
    
    //will print the lables of the partitions present and also the data in the group descriptors 
    partition_data_printer(h,sector_offest,LBA);
    
    
    CloseHandle(h);
    
    
}

//backup_partition_printer
// {
    // sector_offest.QuadPart =0 0th LBA(MBR);
    // sector_offest.QuadPart =512 1st LBA(GPT);
    // for(int j = 2; j < 34; j++)
    // {
    //     char lable_buffer[73] ;
    //     BOOL flag = true;
    //     LBA = j;
    //     sector_offest.QuadPart =512*LBA;//partition entries in 2nd LBA to 33LBA

    //     //SetFilePointerEx(HANDLE,sector_ofset,new position storer,start from)
    //     //sector_offset can be given with making a long integer variable and setting the .Quadpart to be the dsired byte value 
    //     SetFilePointerEx(h,sector_offest,NULL,FILE_BEGIN);
    //     ReadFile(h,buffer,sizeof(buffer),&bytes_read,NULL);
    //     printf("\nbytes read are : %d",bytes_read);
    //     nl(1);
    //     for (size_t i = 0; i < 512 ; i++)
    //     {
    //         if (flag == true)
    //         {
                
    //             if(lable_count == 56)
    //             {
                    
    //                 //tester to know which is the 56th byte 
    //                 //printf("\nit is 56th byte");
    //                 int l = i;
    //                 for(int k = 0 ; k < 72 ; k++)
    //                 {
    //                     lable_buffer[k] = buffer[l++];
    //                 }
    //                 lable_buffer[72] = '\0';
    //                 //tester for knowing what is in the lable_buffer
    //                 // for(int k = 0 ; k < 72 ; k++)
    //                 // {
    //                 //     printf("%02X ",(unsigned char)lable_buffer[k]);
    //                 // }
    //                 // nl();
                    
    //                 lable_count = 0;
    //                 flag = false;
    //             }
    //             else
    //             {
    //                 lable_count++;
    //             }
    //         }
    //         printf("%02X ", (u_char)buffer[i]);
    //         if((i+1)%16 == 0)
    //         {
    //             printf("\n");
    //         }
    //         if((i+1)%128 == 0)
    //         {
    //             count++;
    //             flag = true;
    //             printf("Above is partition %d data:",count);
    //             print_partitionname(lable_buffer);
    //             nl(2);

    //         }
    //     }
        
    // }
// }





