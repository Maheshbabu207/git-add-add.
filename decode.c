#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "encode.h"
#include "types.h"
#include <stdlib.h>

char password[40];
char str[500];

Status openfiles(DecodeInfo *decInfo)
{
    decInfo->stego_image_fname = "stego.bmp";
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    else
    {
        printf("INFO : Opened %s\n", decInfo->stego_image_fname);
        return e_success;
    }
    return e_success;
}
/* Skip bmp image header */
Status Skip_bmp_header(FILE *fptr_stego_image_fname)
{

    fseek(fptr_stego_image_fname, 54, SEEK_SET);
    return e_success;
}

/* Decode a byte into LSB of image data array */
int decode_int_from_lsb(DecodeInfo *decInfo)
{
    unsigned char byte[32];
    unsigned int bit = 0;
    fread(byte, 32, 1, decInfo->fptr_stego_image);

    for (int i = 0; i < 32; i++) 
    {
       
        bit = ((byte[i] & 1) << (31 - i)) | (bit);

    }
    return bit;
}


/* Encode a byte into LSB of image data array */
Status decode_string_from_lsb(uint size, DecodeInfo *decInfo)
{
    unsigned char byte[9];
    char ch = 0;
    int i;

    for(i = 0;i < size;i++)
	{
		ch = 0;
		fread(byte, 8, 1, decInfo->fptr_stego_image);
		for(int j = 7;j >= 0;j--)
		{
            if (byte[j] & 1)
            {
                ch = (1 | (ch<<1)) ;
            }else{
                ch = ch << 1;
            }
            
			// ch = (((byte[j] & 1) << j) | (ch));
		}
		str[i] = ch;
	}

	str[i] = '\0';
    return e_success;
}

/* Store Magic String */
Status decode_magic_string_len(DecodeInfo *decInfo)
{
    
    decInfo->magic_str_len = decode_int_from_lsb(decInfo);

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    decode_string_from_lsb(decInfo->magic_str_len, decInfo);
    return e_success;
}

/* Decode secret file extenstion */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    decInfo->sec_extn_size = decode_int_from_lsb(decInfo);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decode_string_from_lsb(decInfo->sec_extn_size, decInfo);
    strcpy(decInfo->extension_decode, str);
    return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->sec_file_size = decode_int_from_lsb(decInfo);
    
    return e_success;
}

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decode_string_from_lsb(decInfo->sec_file_size, decInfo);
    strcpy(decInfo->image_data, str);
    return e_success;
}

Status copy_data_to_file(DecodeInfo *decInfo)
{
   strcpy(decInfo->file_name,strcat(decInfo->file_name, decInfo->extension_decode) );
    
    //opening ouput file aftter concatinating the decoded extension.
    decInfo->fptr_output_file = fopen(decInfo->file_name, "w");
    if (decInfo->fptr_output_file == NULL)
    {
        perror("fopen");
        printf("ERROR: output file failed.\n");
        return e_failure;
    }
    //printf("INFO : Opened %s\n", decInfo->file_name);
   
    fwrite(decInfo->image_data, sizeof(char), decInfo->sec_file_size, decInfo->fptr_output_file);
    fclose(decInfo->fptr_output_file);
    return e_success;
}

Status check_decode_password(DecodeInfo *decInfo)
{
  printf("Enter the PASSWORD :");
  scanf("%19s", password);
 
  if (strcmp(password, str) == 0)
  {
    return e_success;
  }
  else
  {
    return e_failure;
  }
}

/* Perform the decode operation*/
Status do_decoding(DecodeInfo *decInfo)
{
    printf("\n--------------------------------"); 
    printf("\n## Decoding Procedure Started ##\n");
    printf("----------------------------------\n"); 

    // Step 1: Open Files
    if (openfiles(decInfo) == e_success) 
    {
        printf("Info: Files opened successfully\n");

            // Step 2: Skip BMP header
            if (Skip_bmp_header(decInfo->fptr_stego_image) == e_success) 
            {
                printf("Info: Decoding magic string length\n");
                // Step 3: Decode Magic String Length
                if (decode_magic_string_len(decInfo) == e_success) 
                {
                     //printf("Info: Decoding magic string length\n");
                    // Step 4: Decode Magic String
                    printf("Info: Decoding magic string\n");
                    if (decode_magic_string(decInfo) == e_success) 
                    {
                        //printf("Info: Magic string decoded\n");

                        // Step 5: Check Decode Password
                        if (check_decode_password(decInfo) == e_success) 
                        {
                             printf("Info: Decoding secret file extension length\n");
                            // Step 6: Decode Secret File Extension Size
                            if (decode_secret_file_extn_size(decInfo) == e_success) 
                            {
                                printf("Info: Decoding secret file extension\n");
                                // Step 7: Decode Secret File Extension
                                if (decode_secret_file_extn(decInfo) == e_success) 
                                {
                                    //printf("Successfull extention ");
                                     printf("Info: Decoding secret file size\n");
                                    // Step 8: Decode Secret File Size
                                    if (decode_secret_file_size(decInfo) == e_success) 
                                    {
                                     printf("Info: Decoding secret file data\n");
                                        // Step 9: Decode Secret File Data
                                        if (decode_secret_file_data(decInfo) == e_success) 
                                        { 
                                                printf("Info: Data successfully copied to file\n");
                                            // Step 10: Copy Data to File
                                            if (copy_data_to_file(decInfo) == e_success) 
                                            {
                                                //printf("\nInfo: Decoding procedure completed successfully\n");
                                                printf("\n");
                                                return e_success;
                                            } 
                                            else 
                                            {
                                                printf("Error: Failed to copy data to file\n");
                                            }
                                        } 
                                        else 
                                        {
                                            printf("Error: Failed to decode secret file data\n");
                                        }
                                    } 
                                    else 
                                    {
                                        printf("Error: Failed to decode secret file size\n");
                                    }
                                } 
                                else 
                                {
                                    printf("Error: Failed to decode secret file extension\n");
                                }
                            } 
                            else 
                            {
                                printf("Error: Failed to decode secret file extension size\n");
                            }
                        } 
                        else 
                        {
                            printf("Error: Password check failed\n");
                        }
                    } 
                    else 
                    {
                        printf("Error: Failed to decode magic string\n");
                    }
                } 
                else 
                {
                    printf("Error: Failed to decode magic string length\n");
                }
            } 
            else 
            {
                printf("Error: Failed to skip BMP header\n");
            }
    
    } 
    else 
    {
        printf("Error: Failed to open files\n");
    }

    return e_failure; 
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[3] != NULL)
  {
    strcpy(decInfo->file_name, argv[3]);
  }
  else
  {
    
    strcpy(decInfo->file_name, "output_file");
  }


if (argv[2] != NULL)
{
  char *str;
  str = strstr(argv[2], ".");
  if (strcmp(str, ".bmp") == 0)
  {
    decInfo->stego_image_fname = argv[2];
  }
  else
  {
    printf("Give correct stego or bmp file\n");
  }
}
else
{
  return e_failure;
}
 return e_success;
}
