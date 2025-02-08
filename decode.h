#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h"

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    uint magic_str_len;
    char magic_string[30];
    /* Decoded file Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
  //  uint ms_size;
    uint sec_extn_size;
    uint sec_file_size;
    char file_extn[MAX_FILE_SUFFIX];

    /* Stego Image Info */
    char *output_file_name;
    FILE *fptr_output_file;
    uint extn_size;
    uint size_sec_file;
    char image_data[1000];
    char file_name[30];
    char extension_decode[10];

}DecodeInfo;

/* Encoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decode operation*/
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status openfiles(DecodeInfo *decInfo);

/* Skip bmp image header */
Status Skip_bmp_header(FILE *fptr_stego_image_fname);

/* Store Magic String */
Status decode_magic_string_len(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);

Status check_decode_password(DecodeInfo *dncInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

Status copy_data_to_file(DecodeInfo *dncInfo);

/* Decode a byte into LSB of image data array */
int decode_int_from_lsb(DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
Status decode_string_from_lsb(uint data, DecodeInfo *decInfo);

#endif


