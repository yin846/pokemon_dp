/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - ntrcomp
  File:     ntrcomp_main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ntrcomp_main.c,v $
  Revision 1.19  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.18  01/05/2006 03:52:26  takano_makoto
  Corrected code for debugging.

  Revision 1.17  03/03/2005 11:37:47  takano_makoto
  Revised refactoring and comments

  Revision 1.16  03/03/2005 11:22:01  takano_makoto
  Revised to allow specification of data width during text output

  Revision 1.15  03/03/2005 07:33:23  takano_makoto
  Added 4-byte alignment option

  Revision 1.14  03/02/2005 07:56:22  takano_makoto
  Changed so that output files are 4-byte aligned.

  Revision 1.13  02/28/2005 05:26:27  yosizaki
  do-indent.

  Revision 1.12  01/12/2005 09:37:04  takano_makoto
  fix copyright header.

  Revision 1.11  01/12/2005 08:19:59  takano_makoto
  Added -s option

  Revision 1.10  05/07/2004 02:48:48  takano_makoto
  small modify.

  Revision 1.9  03/05/2004 08:42:13  takano_makoto
  Change output file Header, agb ->nitro.

  Revision 1.8  03/05/2004 03:18:31  takano_makoto
  little modify. One variable change to long from int

  Revision 1.7  03/04/2004 11:25:06  takano_makoto
  little modify

  Revision 1.6  03/04/2004 10:26:53  takano_makoto
  Add function that make output file name automatically.

  Revision 1.5  03/04/2004 09:12:09  takano_makoto
  add CVS Header

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "nitroCompLib.h"

// #pragma comment(lib, "nitroCompLib.lib") /* When using a DLL */

#define uint    unsigned int
#define ushort  unsigned short
#define ulong   unsigned long
#define uchar   unsigned char

#define ROUNDUP(x,n) (((x) + ((n) - 1)) & (~((n)-1)))
#define ROUNDUP4(x) ROUNDUP((x),4)


//---------------------------------------------------------------------------
// Type Definitions
//---------------------------------------------------------------------------
// Compression type
typedef enum
{
    CONV_TYPE_NON,                     // No conversion
    CONV_TYPE_DIFF,                    // Difference filter
    CONV_TYPE_RUNLENGTH,               // Run Length Encoding
    CONV_TYPE_LZ77,                    // LZ77 compression
    CONV_TYPE_HUFFMAN                  // Huffman compression
}
t_conv_type;

// Compression data information structure
typedef struct
{
    t_conv_type type;                  // Compression type
    uint    opt;                       // Compression parameter
    ulong   src_size;                  // Data size before compression
    ulong   dest_size;                 // Data size after compression
    uchar  *src_buf;                   // Data buffer for before compression
    uchar  *dest_buf;                  // Data buffer for after compression
    uchar   textout_width;             // Size and flag indicating whether to output text
    uchar   header_flg:1;              // Flag for whether to append header information
    uchar   silent_flg:1;              // Flag to determine whether to output the message to the standard output
    uchar   round4_flg:1;              // Flag indicating whether to align size of output file to 4 bytes
}
t_conv_data;


//---------------------------------------------------------------------------
// Prototype Declaration
//---------------------------------------------------------------------------
static void usage(const char *cmd_string);
static int parse_opt(int argc, char *argv[], t_conv_data * cnv_dat, char **in_fname,
                     char **out_fname);
static int load_file(const char *fname, t_conv_data * cnv_dat);
static int save_file(const char *fname, const t_conv_data * cnv_dat);
static int convert_data(t_conv_data * cnv_dat);
static int fwrite_text(const uchar * buf, ulong size, uint width, FILE * fp, const char *name);
static char *cut_fname(char *path);
static char *make_output_fname(const char *input_fname, const t_conv_data * cnv_dat);

//---------------------------------------------------------------------------
// Main function
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    t_conv_data cnv_dat;
    char   *in_fname, *out_fname;

    // Initialize data
    memset(&cnv_dat, 0, sizeof(cnv_dat));

    // Parse command options
    if (parse_opt(argc, argv, &cnv_dat, &in_fname, &out_fname) != 0)
    {
        exit(1);
    }

    // Load input file to buffer
    if (load_file(in_fname, &cnv_dat) != 0)
    {
        exit(1);
    }

    // Compress file
    if (convert_data(&cnv_dat) != 0)
    {
        exit(1);
    }

    // Write to output file from buffer
    if (save_file(out_fname, &cnv_dat) != 0)
    {
        exit(1);
    }

    if (!cnv_dat.silent_flg)
    {
        fprintf(stdout, "convert %s(%ldbyte) to %s(%ldbyte)\n", in_fname, cnv_dat.src_size,
                out_fname, cnv_dat.dest_size);
    }

    free(cnv_dat.src_buf);
    if (cnv_dat.dest_buf != cnv_dat.src_buf)
    {
        free(cnv_dat.dest_buf);
    }

    return 0;
}

//---------------------------------------------------------------------------
// USAGE:
//---------------------------------------------------------------------------
static void usage(const char *cmd_string)
{
    fprintf(stderr,
            "Usage: %s <-d|r|l|h [option]> [-s] [-t[width]] [-H] [-A4] [-o outputFile] <inputFile> \n",
            cmd_string);
    fprintf(stderr, "\t-s                   Print no message if conversion has been success.\n");
    fprintf(stderr,
            "\t-t[TypeWidth(1|2|4)] output C format text. We able to specify the type(1=u8,2=u16,4=u32).\n");
    fprintf(stderr, "\t-H                   Raw data header\n");
    fprintf(stderr, "\t-A4                  Align 4byte for filesize\n");
    fprintf(stderr, "\t-d BitSize(8|16)     Differential filter.\n");
    fprintf(stderr, "\t-r                   Runlength encode.\n");
    fprintf(stderr, "\t-l Search            LZ77      encode.\n");
    fprintf(stderr, "\t-h BitSize(4|8)      Huffman   encode.\n");
    fprintf(stderr, "\t-o outputFile        Specify the output file name.\n");
}


//---------------------------------------------------------------------------
// Get a pointer to the file name portion from the path character string. Delete the extension.
// @param path  path
// @return pointer to the file name
//---------------------------------------------------------------------------
static char *cut_fname(char *path)
{
    char   *search_tmp;

    if (path == NULL)
    {
        return NULL;
    }

    if ((search_tmp = strrchr(path, '/')) != NULL)
    {
        path = (search_tmp + 1);
    }
    if ((search_tmp = strrchr(path, '\\')) != NULL)
    {
        path = (search_tmp + 1);
    }
    if ((search_tmp = strrchr(path, '.')) != NULL)
    {
        *search_tmp = '\0';
    }
    return path;
}

//---------------------------------------------------------------------------
// Function that generates the output file name from the input file name.
// @param input_fname Input file name
// @param cnv_dat Conversion information
// @return Pointer to output file name
//---------------------------------------------------------------------------
static char *make_output_fname(const char *input_fname, const t_conv_data * cnv_dat)
{
    char   *out_fname, *edit_tmp;

    if (input_fname == NULL)
    {
        return NULL;
    }

    // Secure memory region
    if ((out_fname = (char *)malloc(strlen(input_fname) + 0x10)) == NULL)
    {
        fprintf(stderr, "internal err: malloc fail!\n");
        exit(1);
    }

    strcpy(out_fname, input_fname);
    edit_tmp = cut_fname(out_fname);

    // Append compression method to file name
    switch (cnv_dat->type)
    {
    case CONV_TYPE_NON:
        strcat(edit_tmp, "_NONE");
        break;
    case CONV_TYPE_DIFF:
        strcat(edit_tmp, "_DIFF");
        break;
    case CONV_TYPE_RUNLENGTH:
        strcat(edit_tmp, "_RL");
        break;
    case CONV_TYPE_LZ77:
        strcat(edit_tmp, "_LZ");
        break;
    case CONV_TYPE_HUFFMAN:
        strcat(edit_tmp, "_HUFF");
        break;
    }
    // Append extension
    if (cnv_dat->textout_width != 0)
    {
        strcat(edit_tmp, ".c");
    }
    else
    {
        strcat(edit_tmp, ".bin");
    }
    return out_fname;
}

//---------------------------------------------------------------------------
// Parse command parameters
// @param argc Number of parameters
// @param argv Parameter array
// @retval cnv_dat Compression information data
// @retval in_fname Input file name
// @retval out_fname Output file name
// @return 0 File write succeeded
//        -1 File write error
//---------------------------------------------------------------------------

static int parse_opt(int argc, char *argv[], t_conv_data * cnv_dat,
                     char **in_fname, char **out_fname)
{
    int     i = 1;
    t_conv_type type;
    int     opt_num;
    char   *cmd_string;
#define SYNTAX_CHECK(exp) do { if (!(exp)) { usage(cmd_string); return -1; } } while(0)

    // Get command name
    cmd_string = cut_fname(argv[0]);

    // Initialize status
    cnv_dat->type = CONV_TYPE_NON;
    cnv_dat->header_flg = 0;
    cnv_dat->textout_width = 0;
    cnv_dat->silent_flg = 0;
    cnv_dat->round4_flg = 0;
    *out_fname = NULL;
    *in_fname = NULL;

    // Analyze options
    while (i < argc)
    {
        if (argv[i][0] != '-')
        {
            SYNTAX_CHECK(*in_fname == NULL);    // Double read check

            // Acquire input file name
            *in_fname = argv[i];
            i++;
            continue;
        }

        // Analyze hyphenated options
        switch (argv[i][1])
        {
            //--------------------
            // No standard output option 's' (argument 0)
        case 's':
            SYNTAX_CHECK(strlen(argv[i]) == 2); // Check no. of chars
            SYNTAX_CHECK(!cnv_dat->silent_flg); // Double read check

            cnv_dat->silent_flg = 1;
            break;

            //--------------------
            // C language text output option 't' (argument 0)
        case 't':
            SYNTAX_CHECK(cnv_dat->textout_width == 0);  // Double read check

            if (strlen(argv[i]) == 2)  // If no parameters, write in byte-sized units
            {
                cnv_dat->textout_width = 1;
            }
            else
            {
                cnv_dat->textout_width = atoi(&argv[i][2]);
                SYNTAX_CHECK(cnv_dat->textout_width == 1 || cnv_dat->textout_width == 2
                             || cnv_dat->textout_width == 4);
            }
            break;

            //--------------------
            // Output file size align specification 'A4' (argument 0)
        case 'A':
            SYNTAX_CHECK(strlen(argv[i]) == 3); // Check no. of chars
            SYNTAX_CHECK(argv[i][2] == '4');    // Check for option chars
            SYNTAX_CHECK(!cnv_dat->round4_flg); // Double read check

            cnv_dat->round4_flg = 1;
            break;
            //--------------------
            // Option 'H' to prepend data size at the head of data decoded data (argument 0)
        case 'H':
            SYNTAX_CHECK(strlen(argv[i]) == 2); // Check no. of chars
            SYNTAX_CHECK(!cnv_dat->header_flg); // Double read check

            cnv_dat->header_flg = 1;
            break;

            //--------------------
            // Output file specification option 'o' (argument 1)
        case 'o':
            SYNTAX_CHECK(strlen(argv[i]) == 2); // Check no. of chars
            SYNTAX_CHECK(*out_fname == NULL);   // Double read check
            SYNTAX_CHECK(i + 1 < argc); // Check the parameter

            *out_fname = argv[++i];
            break;

            //--------------------
            // Difference filter specification 'd' (argument 1)
        case 'd':
            type = CONV_TYPE_DIFF;
            goto common_opt1;
            //--------------------
            // Run length compression specification 'r' (argument 0)
        case 'r':
            SYNTAX_CHECK(strlen(argv[i]) == 2); // Check no. of chars

            type = CONV_TYPE_RUNLENGTH;
            opt_num = 0;
            goto common;
            //--------------------
            // LZ compression specification 'd' (argument 1)
        case 'l':
            type = CONV_TYPE_LZ77;
            goto common_opt1;
            //--------------------
            // Huffman compression specification 'h' (argument 1)
        case 'h':
            type = CONV_TYPE_HUFFMAN;
            goto common_opt1;

            //--------------------
            // Common process for specific part of compression method
          common_opt1:
            if (strlen(argv[i]) == 2)
                // If the character string is finished, next argument will be a parameter.
            {
                opt_num = 1;
            }
            else
                // If the character string continues, that value will become a parameter.
            {
                cnv_dat->opt = atoi(&argv[i][2]);
                SYNTAX_CHECK(cnv_dat->opt != 0);        // Checks whether the parameter is a number greater than or equal to 1.
                opt_num = 0;
            }
            // don't break;
          common:
            SYNTAX_CHECK(cnv_dat->type == CONV_TYPE_NON);       // Double read check

            cnv_dat->type = type;

            if (opt_num == 0)
            {
                break;
            }

            // If there are arguments
            SYNTAX_CHECK(i + 1 < argc); // Check the parameter
            i++;
            SYNTAX_CHECK(argv[i][0] != '-');    // Checks parameter to see whether starts with Huffman

            cnv_dat->opt = atoi(argv[i]);

            SYNTAX_CHECK(cnv_dat->opt != 0);    // Checks whether the parameter is a number greater than or equal to 1.

            break;
            //--------------------
        default:
            usage(cmd_string);
            return -1;
        }
        i++;
    }

    SYNTAX_CHECK(*in_fname != NULL);   // Checks whether an input file is specified.

    // Auto generate output file name
    if (*out_fname == NULL)
    {
        *out_fname = make_output_fname(*in_fname, cnv_dat);
    }

    if (strcmp(*in_fname, *out_fname) == 0)
    {
        fprintf(stderr, "ERR: output file %s is same as input file\n", *out_fname);
        return -1;
    }

    return 0;
#undef SYNTAX_CHECK
}



//---------------------------------------------------------------------------
// Function to load file name
// 
// @param fname Input file name
// @retval Pointer to the compression information structure
// @return 0 File read succeeded
//        -1 File read error
//---------------------------------------------------------------------------

static int load_file(const char *fname, t_conv_data * cnv_dat)
{
    FILE   *in_file;

    // Open the file
    if ((in_file = fopen(fname, "rb")) == NULL)
    {
        fprintf(stderr, "ERR: could not open file: %s\n", fname);
        return -1;
    }

    // Get file size
    fseek(in_file, 0, SEEK_END);
    if ((cnv_dat->src_size = ftell(in_file)) < 0)
    {
        fprintf(stderr, "ERR: file error\n");
        return -1;
    }

    // Load input file to buffer
    if ((cnv_dat->src_buf = (uchar *) malloc(ROUNDUP4(cnv_dat->src_size))) == NULL)
    {
        fprintf(stderr, "ERR: memory exhausted\n");
        return -1;
    }

    fseek(in_file, 0, SEEK_SET);
    if (fread(cnv_dat->src_buf, 1, cnv_dat->src_size, in_file) != cnv_dat->src_size)
    {
        fprintf(stderr, "ERR: read error\n");
        return -1;
    }

    fclose(in_file);

    // Pack with 0 until 4-byte aligned.
    {
        ulong   i;
        for (i = 0; (cnv_dat->src_size + i) & 3; i++)
        {
            cnv_dat->src_buf[cnv_dat->src_size + i] = 0;
        }
    }

    return 0;
}


//---------------------------------------------------------------------------
// Function to write to output file
// 
// @param fname Output file name
// @retval Pointer to the compression information structure
// @return 0 File write succeeded
//        -1 File write error
//---------------------------------------------------------------------------

static int save_file(const char *fname, const t_conv_data * cnv_dat)
{
    FILE   *out_file;

    // Check write data
    if (cnv_dat->dest_buf == NULL || cnv_dat->dest_size <= 0)
    {
        fprintf(stderr, "ERR: convert error\n");
        return -1;
    }

    // Open the file
    if ((out_file = fopen(fname, "wb")) == NULL)
    {
        fprintf(stderr, "ERR: could not open file: %s\n", fname);
        return -1;
    }

    // Write the buffer to the output file.
    if (cnv_dat->textout_width != 0)
    {
        // Text output
        if (fwrite_text
            (cnv_dat->dest_buf, cnv_dat->dest_size, cnv_dat->textout_width, out_file,
             fname) != cnv_dat->dest_size)
        {
            fprintf(stderr, "ERR: write error\n");
            return -1;
        }
    }
    else
    {
        // Binary output
        if (fwrite(cnv_dat->dest_buf, 1, cnv_dat->dest_size, out_file) != cnv_dat->dest_size)
        {
            fprintf(stderr, "ERR: write error\n");
            return -1;
        }
    }

    fclose(out_file);

    return 0;
}

//---------------------------------------------------------------------------
// C language format text output
// @param buf Data buffer
// @param size Data size
// @param fp Output file pointer
// @param name Constant label name
// @return 0 File write succeeded
//        -1 File write error
//---------------------------------------------------------------------------

static int fwrite_text(const uchar * buf, ulong size, uint width, FILE * fp, const char *name)
{
    char   *const_name;
    ulong   i;
    ulong   line_num;

    if (fp == NULL)
    {
        return -1;
    }
    if (buf == NULL)
    {
        return -1;
    }
    if (name == NULL)
    {
        return -1;
    }

    // Decide constant name
    if ((const_name = (char *)malloc(strlen(name))) == NULL)
    {
        fprintf(stderr, "internal err: malloc fail!\n");
        return -1;
    }

    strcpy(const_name, name);
    const_name = cut_fname(const_name);
    size = (size + width - 1) / width;

    // Write the data
    if (fprintf(fp, "#include <nitro.h>\n\n") < 0)
    {
        return -1;
    }
    if (fprintf(fp, "const u%d %s[0x%lX] = {", width * 8, const_name, size) < 0)
    {
        return -1;
    }

    line_num = (width >= 2) ? 2 : 1;

    for (i = 0; i < size; i++)
    {
        if ((i * line_num) % 0x10 == 0)
        {
            if (fprintf(fp, "\n") < 0)
            {
                return -1;
            }
        }
        switch (width)
        {
        case 1:
            if (fprintf(fp, "0x%02X,", *buf) < 0)
            {
                return -1;
            }
            buf++;
            break;
        case 2:
            if (fprintf(fp, "0x%04X,", *(ushort *) buf) < 0)
            {
                return -1;
            }
            buf += 2;
            break;
        case 4:
            if (fprintf(fp, "0x%08lX,", *(ulong *) buf) < 0)
            {
                return -1;
            }
            buf += 4;
            break;
        default:
            return -1;
        }
    }

    if (fprintf(fp, "\n};\n") < 0)
    {
        return -1;
    }

    free(const_name);

    return i * width;
}


//---------------------------------------------------------------------------
// Data conversion
// @retval cnv_dat Conversion information data
// @return 0 Conversion succeeded
//        -1 Conversion failed
//---------------------------------------------------------------------------

static int convert_data(t_conv_data * cnv_dat)
{
    char    cmd_str[16];

    // Generate the command string
    switch (cnv_dat->type)
    {
        //---------------------
        // Difference filter
    case CONV_TYPE_DIFF:
        if (cnv_dat->opt != 8 && cnv_dat->opt != 16)
        {
            fprintf(stderr, "ERR: invalid DIFF parameter %d\n", cnv_dat->opt);
            return -1;
        };
        sprintf(cmd_str, "d%d", cnv_dat->opt);
        break;

        //---------------------
        // Run length compression
    case CONV_TYPE_RUNLENGTH:
        // Run Length Encoding
        strcpy(cmd_str, "r");
        break;

        //---------------------
        // LZ77 compression
    case CONV_TYPE_LZ77:
        if (cnv_dat->opt < 2 || cnv_dat->opt > 255)
        {
            fprintf(stderr, "ERR: invalid LZ77 parameter %d\n", cnv_dat->opt);
            return -1;
        };
        sprintf(cmd_str, "l%d", cnv_dat->opt);
        break;

        //---------------------
        // Huffman compression
    case CONV_TYPE_HUFFMAN:
        if (cnv_dat->opt != 4 && cnv_dat->opt != 8)
        {
            fprintf(stderr, "ERR: invalid HUFFMAN parameter %d\n", cnv_dat->opt);
            return -1;
        };
        sprintf(cmd_str, "h%d", cnv_dat->opt);
        break;

        //---------------------
        // No compression
    case CONV_TYPE_NON:
        {
            cnv_dat->dest_size = cnv_dat->src_size;
            cnv_dat->dest_buf = cnv_dat->src_buf;

            // 4 byte align check
            if (cnv_dat->round4_flg)
            {
                cnv_dat->dest_size = ROUNDUP4(cnv_dat->src_size);
            }
            else if (cnv_dat->textout_width >= 2)
            {
                cnv_dat->dest_size = ROUNDUP(cnv_dat->src_size, cnv_dat->textout_width);
            }
        }
        return 0;

    default:
        fprintf(stderr, "ERR: invalid convert type\n");
        return -1;
    }

    // Secure buffer region for file after compression
    cnv_dat->dest_buf = nitroCompMalloc(cnv_dat->src_size);

    // Data compression
    cnv_dat->dest_size = nitroCompress(cnv_dat->src_buf,
                                       cnv_dat->src_size,
                                       cnv_dat->dest_buf, cmd_str, cnv_dat->header_flg);
    // 4 byte align check
    if (cnv_dat->round4_flg)
    {
        cnv_dat->dest_size = ROUNDUP4(cnv_dat->dest_size);
    }
    // Check alignment according to text format's output type.
    if (cnv_dat->textout_width >= 2)
    {
        cnv_dat->dest_size = ROUNDUP(cnv_dat->dest_size, cnv_dat->textout_width);
    }

#if 0
    // Check expansion of compressed data
    {
        char   *decomp_buf;
        ulong   decomp_size;

        decomp_buf = nitroCompMalloc(cnv_dat->src_size);
        decomp_size = nitroDecompress(cnv_dat->dest_buf, cnv_dat->dest_size, decomp_buf, 1);

        if (matchingCheck(cnv_dat->src_buf, cnv_dat->src_size, decomp_buf, decomp_size) == 0)
        {
            fprintf(stderr, "ERR: data convert error\n");
            return -1;
        }
    }
#endif

    return 0;
}
