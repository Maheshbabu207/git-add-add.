#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
char magic_string[10];
char*extn;
char secret_extn[40];
unsigned int secret_file_size;

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint src_image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    secret_file_size = get_file_size(encInfo->fptr_secret);


    printf("Enter the Magic String:");
    scanf("%s",magic_string);
    uint magic_string_len = strlen(magic_string);
    if(src_image_size >= (54 + ((magic_string_len + 4 + secret_file_size) * 8)))
     {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint file_size = ftell(fptr);
  
    rewind(fptr);
    return file_size;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char header[54];
    fread(header, 54, 1, fptr_src_image);
    fwrite(header, 54, 1, fptr_dest_image);
    return e_success;
}


/* Store Magic Stringlen */
Status encode_magic_string_len(const char *magic_string, EncodeInfo *encInfo)
{
    uint magic_string_len = strlen(magic_string);
    encode_int_to_lsb(magic_string_len,encInfo);
    return e_success;
}

/* Encode integer data to source file data*/
Status encode_int_to_lsb(uint data, EncodeInfo *encInfo)
{
    unsigned char byte;
    for (int i = 0; i < 32; i++) 
    {
        fread(&byte, 1, 1, encInfo->fptr_src_image);
        byte = (byte & ~1) | ((data >> (31 - i)) & 1);    // Clear the LSB of the byte and set it to the corresponding bit of the data
        fwrite(&byte, 1, 1, encInfo->fptr_stego_image);
       
    }

    return e_success;  // Return success if encoding is completed
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_string_to_lsb(magic_string, encInfo);
    return e_success;
}

/*Encode string data to source file data */
Status encode_string_to_lsb(const char *data, EncodeInfo *encInfo)
{
    unsigned char byte;
    for (int i = 0; i < strlen(data); i++) 
    {
        byte = data[i];
        for (int j = 0; j < 8; j++) 
        {
            if (fread(&byte, 1, 1, encInfo->fptr_src_image) != 1) // Read one byte from the source image
            {
                return e_failure;
            }
            byte = (byte & ~1) | ((data[i] >> (j)) & 1); // Modify the LSB with the current bit of the character
            if (fwrite(&byte, 1, 1, encInfo->fptr_stego_image) != 1)   // Write the modified byte to the stego image
            {
                return e_failure;
            }
        }
    }

    return e_success;
}

/* Encode secret file extenstion*/
Status encode_secret_file_extn_size(long size_t, EncodeInfo *encInfo)
{
    encode_int_to_lsb(size_t, encInfo);
    return e_success;
}

/* Encode secret file extenstion*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_string_to_lsb(file_extn, encInfo);
    return e_success;
}


/* Encode secret file extenstion */
Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
    if (encode_int_to_lsb(secret_file_size, encInfo) == e_success)
    {
        return e_success;
    }

    return e_failure;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char data[secret_file_size+1];
    data[secret_file_size] = '\0';
	fread(data,secret_file_size, 1,encInfo->fptr_secret);

    encode_string_to_lsb(data, encInfo);
	return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Info: Copying Left over Data\n");
    char ch[1];
    while (fread(ch, 1, 1, fptr_src_image))
    {
        fwrite(ch, 1, 1, fptr_dest_image);
    }
    return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("-------------------------------------\n");
    printf("   ## Encoding Procedure Started ##  \n");
    printf("-------------------------------------\n");

    // Step 1: Open files
    if (open_files(encInfo) == e_success)
    {
        printf("Info: Files opened successfully\n");

        // Step 2: Check the capacity of the image
        if (check_capacity(encInfo) == e_success)
        {
            printf("Info: Capacity check passed\n");

            // Step 3: Copy the BMP header (54 bytes)
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Info: BMP header copied\n");

                // Step 4: Encode Magic String Length
                if (encode_magic_string_len(magic_string, encInfo) == e_success)
                {
                    printf("Info: Magic string length encoded\n");

                    // Step 5: Encode Magic String
                    if (encode_magic_string(magic_string, encInfo) == e_success)
                    {
                        printf("Info: Magic string encoded\n");

                        // Step 6: Encode Secret File Extension Size
                        int size_t = strlen(strstr(encInfo-> secret_fname,"."));
                        if (encode_secret_file_extn_size(size_t, encInfo) == e_success)
                        {
                            printf("Info: Secret file extension size encoded\n");

                            // Step 7: Encode Secret File Extension
                            if (encode_secret_file_extn(strstr(encInfo->secret_fname, "."), encInfo) == e_success)
                            {
                                printf("Info: Secret file extension encoded\n");

                                // Step 8: Encode Secret File Size
                                if (encode_secret_file_size(secret_file_size, encInfo) == e_success)
                                {
                                    printf("Info: Secret file size encoded\n");

                                    // Step 9: Encode Secret File Data
                                    if (encode_secret_file_data(encInfo) == e_success)
                                    {
                                        printf("Info: Secret file data encoded\n");

                                        // Step 10: Copy Remaining Image Data
                                        if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                        {
                                            printf("Info: Remaining image data copied\n");
                                            printf("-------------------------------------\n");
                                            printf("-*-*-Encoding Successfully Done-*-*-\n");
                                            printf("-------------------------------------\n");

                                            // Step 11: Close all files
                                            fclose(encInfo->fptr_stego_image);
                                            fclose(encInfo->fptr_src_image);
                                            fclose(encInfo->fptr_secret);

                                            //printf("Info: Encoding procedure completed successfully\n");
                                            return e_success;
                                        }
                                        else
                                        {
                                            fprintf(stderr, "Error: Failed to copy remaining image data\n");
                                        }
                                    }
                                    else
                                    {
                                        fprintf(stderr, "Error: Failed to encode secret file data\n");
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "Error: Failed to encode secret file size\n");
                                }
                            }
                            else
                            {
                                fprintf(stderr, "Error: Failed to encode secret file extension\n");
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: Failed to encode secret file extension size\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error: Failed to encode magic string\n");
                    }
                }
                else
                {
                    fprintf(stderr, "Error: Failed to encode magic string length\n");
                }
            }
            else
            {
                fprintf(stderr, "Error: Failed to copy BMP header\n");
            }
        }
        else
        {
            fprintf(stderr, "Error: Not enough capacity in the image to store the secret file\n");
        }
    }
    else
    {
        fprintf(stderr, "Error: Failed to open files\n");
    }

    return e_failure;  
}


Status read_and_validate_encode_args(int argc ,char *argv[], EncodeInfo *encInfo)
{
    char *ptr;
    if(argc < 4)
    {
        return e_failure;
    }
    if (argc != 4 && argc != 5)
    {
        fprintf(stderr, "ERROR: Invalid number of arguments\n");
        return e_failure;
    }
    if (strcmp(".bmp",(ptr)) == 0)
    {
        fprintf(stderr, "ERROR: Source image file must have .bmp extension\n");
        return e_failure;
    }
    
    char *extn_pos = strchr(argv[3], '.');
    if (extn_pos == NULL || strlen(extn_pos) < 2)  
    {
        fprintf(stderr, "ERROR: Invalid secret file  extension.\n");
        return e_failure;
    }
    if (argc == 5)
    {
        if (strstr(argv[4], ".bmp") == NULL)
        {
            printf("INFO: Invalid stego image filename. Defaulting to 'stego.bmp'.\n");
            encInfo->stego_image_fname = "stego.bmp";
        }
        else
        {
            encInfo->stego_image_fname = argv[4];
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    encInfo->src_image_fname = argv[2];
    encInfo->secret_fname = argv[3];

    return e_success;
}
