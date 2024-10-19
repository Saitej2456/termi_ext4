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
    int count = 0;
    int lable_count = 0;
    // char lable_str[73];
    //check into this later 
    //issue : when declared getting an output which is randomw for each execution
    char buffer[512];
    LARGE_INTEGER sector_offest;
    int LBA;
    //eahc LBA is of size 512
    // sector_offest.QuadPart =0 0th LBA(MBR);
    // sector_offest.QuadPart =512 1st LBA(GPT);
    for(int j = 2; j < 34; j++)
    {
        char lable_buffer[73] ;
        BOOL flag = true;
        LBA = j;
        sector_offest.QuadPart =512*LBA;//partition entries in 2nd LBA to 33LBA
        SetFilePointerEx(h,sector_offest,NULL,FILE_BEGIN);
        ReadFile(h,buffer,sizeof(buffer),&bytes_read,NULL);
        for (size_t i = 0; i < 512 ; i++)
        {
            if (flag == true)
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
                    flag = false;
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
                count++;
                flag = true;
                printf("Above is partition %d data:",count);
                print_partitionname(lable_buffer);
                nl(2);

            }
        }
        
    }
    CloseHandle(h);
    
    
}


// #include <stdio.h>
// #include <unistd.h>
// #include <windows.h>
// #include <stdbool.h>

// int main()
// {
//     HANDLE h;
//     DWORD bytes_read;
//     /*
//     HANDLE CreateFile(    
//          LPCWSTR               lpFileName,   
//          DWORD                 dwDesiredAccess,    
//          DWORD                 dwShareMode,    
//          LPSECURITY_ATTRIBUTES lpSecurityAttributes,    
//          DWORD                 dwCreationDisposition,    
//          DWORD                 dwFlagsAndAttributes,    
//          HANDLE                hTemplateFile  );  
//          */
//     char PATH[MAX_PATH] = "\\\\.\\PHYSICALDRIVE0" ;
//     h = CreateFile(PATH,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
//     if (h == INVALID_HANDLE_VALUE)
//     {
//         printf("error msg : %lu",GetLastError());
//         CloseHandle(h);
//         return 1;
//     }
//     int count = 0;
//     char buffer[512];
//     LARGE_INTEGER sector_offest;
//     int LBA;
//     //eahc LBA is of size 512
//     // sector_offest.QuadPart =0 0th LBA(MBR);
//     // sector_offest.QuadPart =512 1st LBA(GPT);
//     for(int j = 2; j < 34; j++)
//     {
//         LBA = j;
//         sector_offest.QuadPart =512*LBA;//partition entries in 2nd LBA to 33LBA
//         SetFilePointerEx(h,sector_offest,NULL,FILE_BEGIN);
//         ReadFile(h,buffer,sizeof(buffer),&bytes_read,NULL);
//         for (size_t i = 0; i < 512 ; i++)
//         {
//             printf("%02X ", (u_char)buffer[i]);
//             if((i+1)%16 == 0)
//             {
//                 printf("\n");
//             }
//             if((i+1)%128 == 0)
//             {
//                 count++;
//                 printf("partition %d data:\n",count);
//             }
//         }
        
//     }
//     CloseHandle(h);
    
    
// }

// #include <stdio.h>
// #include <unistd.h>
// #include <windows.h>
// #include <stdbool.h>

// void nl()
// {
//     printf("\n");
// }

// int main()
// {
//     HANDLE h;
//     DWORD bytes_read;
    
//     char PATH[MAX_PATH] = "\\\\.\\PHYSICALDRIVE0";
//     h = CreateFile(PATH, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
//     if (h == INVALID_HANDLE_VALUE)
//     {
//         printf("Error message: %lu", GetLastError());
//         CloseHandle(h);
//         return 1;
//     }
    
//     char buffer[512];
//     LARGE_INTEGER sector_offset;
//     int LBA;
    
//     for (int j = 2; j < 34; j++) // Loop through LBAs 2 to 33 (Partition Entries)
//     {
//         int partition_count = 0;
//         LBA = j;
//         sector_offset.QuadPart = 512 * LBA; // Set the offset to the current LBA
//         SetFilePointerEx(h, sector_offset, NULL, FILE_BEGIN);
//         ReadFile(h, buffer, sizeof(buffer), &bytes_read, NULL);
        
//         for (size_t i = 0; i < 512; i += 128) // Process each 128-byte partition entry
//         {
//             printf("Partition %d data:\n", ++partition_count);

//             // Print entire partition entry for debugging (Optional)
//             for (size_t k = i; k < i + 128; k++)
//             {
//                 printf("%02X ", (unsigned char)buffer[k]);
//                 if ((k + 1) % 16 == 0) printf("\n");
//             }
//             nl();

//             // Start reading label from the 56th byte
//             printf("Label: ");
//             for (int k = 56; k < 128; k += 2)
//             {
//                 // Combine two bytes into a UTF-16 character
//                 unsigned char byte1 = buffer[i + k];
//                 unsigned char byte2 = buffer[i + k + 1];
//                 unsigned short utf16_char = (byte2 << 8) | byte1;
                
//                 // Stop if null terminator
//                 if (utf16_char == 0) break;

//                 // Print character (assuming basic ASCII for simplicity)
//                 printf("%c", (char)utf16_char);
//             }
//             nl();
//         }
//     }
    
//     CloseHandle(h);
//     return 0;
// }
