#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;

    OperationType option = check_operation_type(argv);
    if (option == e_encode)
    {
        printf("User selected encode operation\n");

        if (read_and_validate_encode_args(argc, argv, &encInfo) == e_failure)
        {
            printf("Encoding: ./a.out -e <.bmp file> <.txt file> <.bmp file> [output file]\n");
            return e_failure;
        }
        else
        {
            if (do_encoding(&encInfo) == e_failure)
            {
                printf("Encoding failed.\n");
                return e_failure;
            }
        }
    }
     else if (option == e_decode)
     {
         printf("User selected decode operation\n");
 
         if (read_and_validate_decode_args(argv, &decInfo) == e_failure)
         {
             printf("Decoding: ./a.out -d <.bmp file> <.txt> [output file]\n");
             return e_failure;
         }
         else
         {
             if (do_decoding(&decInfo) == e_failure)
             {
                printf("-------------------------------------\n");
                printf("-*-*-     Decoding failed       -*-*-\n");
                printf("-------------------------------------\n");
                 return e_failure;
             }
             else
             {
                 printf("-------------------------------------\n");
                printf("-*-*-Decoding Done successfully-*-*- \n");
                printf("-------------------------------------\n");
                 return e_success;
             }
         }
     }
     else
     {
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> <.bmp file> [output file]\n");
        printf("Decoding: ./a.out -d <.bmp file> <.txt file> [output file]\n");
         return e_failure;
     }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
    {
        return e_unsupported;
    }
   
    if(!strcmp(argv[1], "-e"))
        return e_encode;


    else if (!strcmp(argv[1], "-d"))
        return e_decode;

     
}
