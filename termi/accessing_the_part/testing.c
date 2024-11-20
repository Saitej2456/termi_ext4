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

unsigned int extractor_s4(char * buffer)
{
    unsigned int value = 0 ;
    
    for(int i = 0 ; i < 4 ;  i++)
    {
        unsigned int temp = (unsigned int)buffer[i];
        temp = temp & (unsigned)255;
        value = value | (temp << i * 8);
    }

    return value;
}

unsigned short extractor_s2(char*buffer)
{
    unsigned short value = 0;

    for(int i = 0 ; i < 2; i++)
    {
        unsigned short temp = (unsigned short)buffer[i];
        temp = temp & (unsigned)255;
        value = value | (temp << i * 8);
    }

    return value; 
}


void superblock(HANDLE h,int partition, unsigned long long int *SLBA_arr)
{
    LARGE_INTEGER set_offset;
    char buffer_sb[1024];
    DWORD bytes_read;
    
    /*
    1. Block Size Calculation

    Field: s_log_block_size
    Offset: 24 bytes into the superblock (absolute offset: 1024 + 24 = 1048 bytes)
    Size: 4 bytes
    Purpose: Determines the block size as:
    block_size=1024<<s_log_block_size

    2. Blocks per Group

    Field: s_blocks_per_group
    Offset: 32 bytes into the superblock (absolute offset: 1024 + 32 = 1056 bytes)
    Size: 4 bytes
    Purpose: Defines the number of blocks in each block group.

    3. Inodes per Group

    Field: s_inodes_per_group
    Offset: 40 bytes into the superblock (absolute offset: 1024 + 40 = 1064 bytes)
    Size: 4 bytes
    Purpose: Specifies the number of inodes in each block group, which helps in locating inodes and inode tables.

    4. First Data Block

    Field: s_first_data_block
    Offset: 20 bytes into the superblock (absolute offset: 1024 + 20 = 1044 bytes)
    Size: 4 bytes
    Purpose: Indicates the ID of the first block that contains file system data, which helps determine if block group descriptors start right after the superblock or if they’re offset by one or more blocks.
    
    5. total number of blocks

    Field: s_blocks_count
    Offset: 4 bytes into the superblock (absolute offset: 1024 + 4 = 1028 bytes from the partition start)
    Size: 4 bytes
    purpose: You can find the total number of blocks in the filesystem from the superblock using the s_blocks_count
    */
      
    
    //offsets

   

    //gives the ofset of the superblock with respect to the ext4 partition start
    int super_block_off = 1024;

    //gives the offset of the s_blocks_count from superblock
    int s_blocks_count_off = 4;

    //gives the offset of the from superblock
    int s_first_data_block_off = 20;
    
    //gives the offset of the s_log_block_size from superblock
    int s_log_block_size_off = 24;

    //gives the offset of the s_blocks_per_group from superblock
    int s_blocks_per_group_off = 32;

    //gives the offset of the s_inodes_per_group from superblock 
    int s_inodes_per_group_off = 40;

    //gives the offset of s_desc_size from the start of superblock
    int s_desc_size_off = 254;



    //storage unit realted to superblock
    char s_blocks_count_str[4];
    char s_first_data_block_str[4];
    char s_log_block_size_str[4];
    char s_blocks_per_group_str[4];
    char s_inodes_per_group_str[4];
    char s_desc_size_str[2];

    set_offset.QuadPart = (SLBA_arr[partition-1]*512)+super_block_off;
    SetFilePointerEx(h,set_offset,NULL,FILE_BEGIN);
    ReadFile(h,buffer_sb,sizeof(buffer_sb),&bytes_read,NULL);

    //tester// prints number of bytes read
    if(0)
    {
        printf("bytes read is : %d\n",bytes_read);
    }    
    //using to store bytes inside the storage buffers 
    int j = 0 ;

    //printf("The s_blocks_count is :\n"); using for the tester
    for(int i = s_blocks_count_off ; i < s_blocks_count_off+4 ; i++)
    {
        s_blocks_count_str[j] = buffer_sb[i];
        //tester to check the data getting into the varibles
        if(0)
        {
            printf("%02X ",(u_char)s_blocks_count_str[j]);
        }
        
        j++;
    }
    nl(1);

    j = 0;
    //printf("The s_first_data_block is :\n"); using for the tester
    for(int i = s_first_data_block_off ; i < s_first_data_block_off+4 ; i++)
    {
        s_first_data_block_str[j] = buffer_sb[i];
        //tester to check the data getting into the varibles
        if(0)
        {
            printf("%02X ",(u_char)s_first_data_block_str[j]);
        }
        
        j++;
    }
    nl(1);

    j = 0;
    // printf("The s_log_block_size is :\n"); using for the tester
    for(int i = s_log_block_size_off ; i < s_log_block_size_off+4 ; i++)
    {
        s_log_block_size_str[j] = buffer_sb[i];
        //tester to check the data getting into the varibles
        if(0)
        {
            printf("%02X ",(u_char)s_log_block_size_str[j]);
        }
        
        j++;
    }
    nl(1);
    
    j = 0;
    // printf("The s_blocks_per_group is :\n"); using for the tester
    for(int i = s_blocks_per_group_off ; i < s_blocks_per_group_off+4 ; i++)
    {
        s_blocks_per_group_str[j] = buffer_sb[i];
        //tester to check the data getting into the varibles
        if(0)
        {
            printf("%02X ",(u_char)s_blocks_per_group_str[j]);
        }
        
        j++;
    }
    nl(1);

    j = 0;
    // printf("The s_inodes_per_group is :\n"); using for the tester
    for(int i = s_inodes_per_group_off ; i < s_inodes_per_group_off+4 ; i++)
    {
        s_inodes_per_group_str[j] = buffer_sb[i];
        //tester to check the data getting into the varibles
        if(0)
        {
            printf("%02X ",(u_char)s_inodes_per_group_str[j]);
        }
        
        j++;
    }
    nl(1);
    j = 0;

    for(int i = s_desc_size_off ; i < s_desc_size_off+2 ; i++)
    {
        s_desc_size_str[j] = buffer_sb[i];
        j++;
    }
    j = 0;

    //values of superblock contents
    u_int s_blocks_count_val = extractor_s4(s_blocks_count_str);
    u_int s_first_data_block_val = extractor_s4(s_first_data_block_str);
    u_int s_log_block_size_val = extractor_s4(s_log_block_size_str);
    u_int s_blocks_per_group_val = extractor_s4(s_blocks_per_group_str);
    u_int s_inodes_per_group_val = extractor_s4(s_inodes_per_group_str);
    u_short s_desc_size_val = extractor(s_desc_size_str);
    

    nl(1);

    //tester to test if the data is getting into the varibales correctly
    if(1)
    {
        printf("%08X\n",s_blocks_count_val);
        printf("%08X\n",s_first_data_block_val);
        printf("%08X\n",s_log_block_size_val);
        printf("%08X\n",s_blocks_per_group_val);
        printf("%08X\n",s_inodes_per_group_val);
        printf("%08X\n",s_desc_size_val);
    }

    //The s_log_block_size field determines the block size with the formula 1024 << s_log_block_size
    u_int block_size = 1024 << s_log_block_size_val;

    //tester to check the block size of the partition
    if (1)
    {
        printf("block_size is : %u\n",block_size);
    }
    

    //handling_group descriptors


    //group descriptors start from second block each block is of size <blocksize> so blocksize is the offset of the group descriptors 
    /*
    1. Offset for Inode Bitmap (bg_inode_bitmap):

    Offset in Group Descriptor: 4 bytes into the group descriptor.
    Size: 4 bytes.
    Meaning: Specifies the starting block of the inode bitmap for this block group.

    2. Offset for Inode Table (bg_inode_table)

    Offset in Group Descriptor: 8 bytes into the group descriptor.
    Size: 4 bytes.
    Meaning: Specifies the starting block of the inode table for this block group.
    */

    

   //offsets

    //gives the offset of the group_descriptor table
    int group_descriptors_off = block_size;

    //gives the offset of the inode bitmap from the group descriptors
    int bg_inode_bitmap_off = 4;

    //gives the offset of the inode table from the group descriptors
    int bg_inode_table_off = 8;
   
    //storage units of the group_descriptors 
    char bg_inode_bitmap_str[4];
    char bg_inode_table_str[4];

    j = 0;

    short size_of_descriptor = s_desc_size_val;

    //tester 
    //to know know many bytes are used for the group descriptors
    if(0)
    {
        printf("\n%d bytes",size_of_descriptor);
    }

    //gives the values of the group_descriptors
    u_int bg_inode_bitmap_val = extractor_s4(bg_inode_bitmap_str);
    u_int bg_inode_table_val = extractor_s4(bg_inode_table_str);

    //gives values to calculate the numebr of blocks the group descriptor table is spanning to 
    u_int number_of_groups = (s_blocks_count_val + s_blocks_per_group_val - 1)/s_blocks_per_group_val;
    u_int size_of_group_descriptor_table = number_of_groups * 32;

    //gives the numebr of blocks the group descriptor table is spanning to 
    int evaluator = (size_of_group_descriptor_table + block_size -1) / block_size;

    if (evaluator > 1)
    {
        printf("more than 1, valueof evaluator is %d",evaluator);
    }
    else 
    {
        printf("1 or less, valueof evaluator is %d",evaluator);
    }
    
    //buffer which stores the group descriptor(teporary change the block_size as soon as possible based on evaluator value)
    char buffer_gdt[block_size];
    set_offset.QuadPart = (SLBA_arr[partition-1]*512)+group_descriptors_off;
    SetFilePointerEx(h,set_offset,NULL,FILE_BEGIN);
    if (!SetFilePointerEx(h, set_offset, NULL, FILE_BEGIN)) 
    {
        printf("Error: Failed to set file pointer. Error code: %d\n", GetLastError());
        return;
    }
    else 
    {
        printf("File pointer successfully set to offset: %lld\n", set_offset.QuadPart);
    }

    ReadFile(h,buffer_gdt,sizeof(buffer_gdt),&bytes_read,NULL);
    
    char buffer_gd[64];

    //this is the structure of a group descriptor 
    // struct ext4_group_desc 
    // {
    // __le32  bg_block_bitmap_lo;     /* Blocks bitmap block */
    // __le32  bg_inode_bitmap_lo;     /* Inodes bitmap block */
    // __le32  bg_inode_table_lo;      /* Inodes table block */
    // __le16  bg_free_blocks_count_lo;/* Free blocks count */
    // __le16  bg_free_inodes_count_lo;/* Free inodes count */
    // __le16  bg_used_dirs_count_lo;  /* Directories count */
    // __le16  bg_flags;               /* EXT4_BG_flags (INODE_UNINIT, etc) */
    // __le32  bg_exclude_bitmap_lo;   /* Exclude bitmap for snapshots */
    // __le16  bg_block_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+bbitmap) LE */
    // __le16  bg_inode_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+ibitmap) LE */
    // __le16  bg_itable_unused_lo;    /* Unused inodes count */
    // __le16  bg_checksum;            /* crc16(sb_uuid+group+desc) */
    // ...
    // };

    nl(1);
    for(int j = 0 ; j<3 ; j++)
    {
        printf("\ngd :%d\n",j+1);
        for(int i = 0 ; i < 64 ; i++)
        {
            buffer_gd[i] = buffer_gdt[i+(j*64)];
            printf("%02X ",(u_char)buffer_gd[i]);
        }
    }

    
    //tester to k now the number of bytes read
    if(1)
    {
        printf("\nbytes read is %d",bytes_read);
    }



    
}


void ext4_checker(HANDLE h, unsigned long long int * SLBA_arr, int partition_count)
{

    //using to store the partition numbers which are of ext4 
    int ext4_partitions[partition_count];
    LARGE_INTEGER sector_offset;

    //using to place the ext4 partition numbers correctly 
    int ext4_arr_count = 0;
    for (int i = 0; i < partition_count; i++)
    {
        // Calculate the offset to reach the super block(SLBA + 1024 )
        sector_offset.QuadPart = (SLBA_arr[i] * 512) + 1024;

        char buffer[1024];
        DWORD bytes_read;

        SetFilePointerEx(h, sector_offset, NULL, FILE_BEGIN);
        ReadFile(h, buffer, sizeof(buffer), &bytes_read, NULL);

        // used to print the data in the super block of size 1024
        // printf("printing the super block data of partition %d\n",i+1);
        // for(int j = 0 ; j < 1024 ; j ++)
        // {
        //     printf("%02X ", (u_char)buffer[j]);
            
        //     if((j+1)%16 == 0)
        //     {
        //         printf("\n");
        //     }
        // }

        // Print the magic number in correct order (little-endian)
        // tester printf("Number found partition %d in Magic number offset: %02X%02X\n", i+1, (u_char)buffer[57], (u_char)buffer[56]);
        // used to check the values of the partitions at magic number offset 

        if (((u_char)buffer[57]==239)&&((u_char)buffer[56]==83))
        {
            // printf("This partition %d is an Ext4 partition as it has the magic numeber is 0XEF53",i+1);
            ext4_partitions[ext4_arr_count] = i+1;
            ext4_arr_count++;
            printf("printing the super block data of partition %d\n",i+1);
        for(int j = 0 ; j < 1024 ; j ++)
        {
            printf("%02X ", (u_char)buffer[j]);
            
            if((j+1)%16 == 0)
            {
                printf("\n");
            }
        }
        }
        else 
        {
            ext4_partitions[ext4_arr_count] = -1;
            ext4_arr_count++;
        }
    }

    for(int i = 0 ; i < sizeof(ext4_partitions)/sizeof(int);i++)
    {
       if (ext4_partitions[i] == -1)
       {
            continue;
       }
       else 
       {
            printf("This partition %d is an Ext4 partition ",ext4_partitions[i]);
       }

        nl(1);
    }

    superblock(h,4, SLBA_arr);
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





