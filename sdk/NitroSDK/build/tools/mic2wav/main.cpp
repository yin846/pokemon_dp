/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - mic2wav
  File:     main.cpp

  Copyright 2003,2004 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.cpp,v $
  Revision 1.3  2004/08/05 07:27:58  takano_makoto
  fix bug.

  Revision 1.2  2004/06/18 10:52:56  yosizaki
  change to CXX_X86.

  Revision 1.1  2004/06/17 04:41:49  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <direct.h>


// Create WAV file from log file .
class   LogToWav :
    public  WAVEFORMATEX
{
private:
    void    *data;
    int     len, reserve;
    bool    end;

private:

    void    UpdateSetting(void)
    {
        cbSize          = sizeof(WAVEFORMATEX);
        wFormatTag      = WAVE_FORMAT_PCM;
        nBlockAlign     = (WORD)((wBitsPerSample / 8) * nChannels);
        nAvgBytesPerSec = nSamplesPerSec * nBlockAlign * nChannels;
    }

public:

    LogToWav() : data(NULL), len(0), reserve(0), end(false)
    {
        nChannels = 1;
        wBitsPerSample = 16;
        nSamplesPerSec = 11025;
        UpdateSetting();
    }

    ~LogToWav()
    {
        Reset();
    }

    // Discard input data.
    void    Reset(void)
    {
        free(data), data = NULL, reserve = 0;
        len = 0;
        end = false;
    }

    // Determine if sample data group is complete.
    bool    IsEnd(void) const { return  end;    }

    // Analyze input line.
    void    InputLine(const char *s);

    // Save current data to specified file.
    bool    Save(const char *path);

} ;


// Analyze input line.
void    LogToWav::InputLine(const char *s)
{
    switch(*s++) {
    case '#':   // Explicit comment.
        break;
    case '$':   // Set parameters.
        if(!strncmp(s, "bits=", 5)) {
            if(len == 0)
                wBitsPerSample = (WORD)atoi(s + 5);
        }
        else if(!strncmp(s, "rate=", 5)) {
            if(len == 0)
                nSamplesPerSec = (WORD)atoi(s + 5);
        }
        else if(!strncmp(s, "end", 3))
            end = true;
        else
            return;
        UpdateSetting();
        break;
    case '|':   // Input data.
        // Detect as "^|([0-9A-F]{n},)+$" with specified bit width.
        {
            const int   width = wBitsPerSample / 4;
            int nibble = 0, sample = 0;
            for( ; ; ) {
                // Extract sample as hex value.
                char    c = (char)toupper(*s++);
                if((c >= '0') && (c <= '9')) {
                    sample = (sample << 4) + (c +  0 - '0');
                    ++nibble;
                }
                else if((c >= 'A') && (c <= 'F')) {
                    sample = (sample << 4) + (c + 10 - 'A');
                    ++nibble;
                }
                // Add to buffer with delimiter.
                else if(c == ',') {
                    // Take only valid values.
                    if(nibble == width) {
                        // If buffer is too small, expand it.
                        if(len >= reserve) {
                            int new_len = reserve ? reserve * 2 : 16;
                            void    *p = realloc(data, new_len);
                            if(!p) {
                                printf("fatal error : can not allocate memory!\n");
                                exit(1);
                            }
                            data = p, reserve = new_len;
                        }
                        if(wBitsPerSample == 8) {
                            *(BYTE*)((BYTE*)data + len) = (BYTE)sample;
                        }
                        else if(wBitsPerSample == 16) {
                            *(SHORT*)((BYTE*)data + len) = (SHORT)(sample - 0x8000);
                        }
                        len += wBitsPerSample / 8;
                    }
                    nibble = 0, sample = 0;
                }
                // Otherwise, end.
                else
                    break;
            }
        }
        break;
    }

}

// Save current data to specified file.
bool    LogToWav::Save(const char *path)
{
    // Open the output file.
    FILE    *f = fopen(path, "wb");
    if(!f)  return  false;

    // Align with DWORD boundary.
    const int   size = ((len + 3) & ~3);
    while(len < size) {
        if(wBitsPerSample == 8) {
            *(BYTE*)((BYTE*)data + len) = *(BYTE*)((BYTE*)data + len - 1);
        }
        else if(wBitsPerSample == 16) {
            *(SHORT*)((BYTE*)data + len) = *(SHORT*)((BYTE*)data + len - 2);
        }
        len += wBitsPerSample / 8;
    }

    // Output the data in order.
    WORD    w;
    DWORD   d;
    fwrite("RIFF", 1, 4, f);        // 4.
    d = 56 + size - 8;
    fwrite(&d, sizeof(d), 1, f);    // 4.
    fwrite("WAVE", 1, 4, f);        // 4.
    // "fmt" 
    fwrite("fmt ", 1, 4, f);        // 4.
    d = 16;
    fwrite(&d, sizeof(d), 1, f);    // 4.
    w = (WORD)wFormatTag;
    fwrite(&w, sizeof(w), 1, f);    // 2.
    w = (WORD)nChannels;
    fwrite(&w, sizeof(w), 1, f);    // 2.
    d = (DWORD)nSamplesPerSec;
    fwrite(&d, sizeof(d), 1, f);    // 4.
    d = (DWORD)nAvgBytesPerSec;
    fwrite(&d, sizeof(d), 1, f);    // 4.
    w = (WORD)nBlockAlign;
    fwrite(&w, sizeof(w), 1, f);    // 2.
    w = (WORD)wBitsPerSample;
    fwrite(&w, sizeof(w), 1, f);    // 2.
    // 36.
    // "fact" 
    fwrite("fact", 1, 4, f);        // 4.
    d = 4;
    fwrite(&d, sizeof(d), 1, f);    // 4.
    d = (DWORD)(len / nBlockAlign);
    fwrite(&d, sizeof(d), 1, f);    // 4.
    // 48.
    // "data" 
    fwrite("data", 1, 4, f);        // 4.
    d = (DWORD)(len);
    fwrite(&d, sizeof(d), 1, f);    // 4.
    // 56.
    fwrite(data, 1, len, f);
    fclose(f);

    return  true;
}


int main(int argc, char* argv[])
{
    if(argc < 2) {
        printf("no input file.\n");
        return  1;
    }

    // Open the output file.
    FILE    *f = fopen(argv[1], "r");
    if(!f) {
        printf("can not open input file \"%s\".\n", argv[1]);
        return  2;
    }

    // Specify the directory. (Otherwise, the same location as the output file.)
    char    s_dir[MAX_PATH];
    if(argc >= 3) {
        strcpy(s_dir, argv[2]);
        const int   n = strlen(s_dir) - 1;
        if((n >= 0) && ((s_dir[n] != '\\') && (s_dir[n] != '/')))
            s_dir[n + 1] = '/', s_dir[n + 2] = '\0';
    } else {
        strcpy(s_dir, argv[1]);
        int n = strlen(s_dir) - 1;
        while((n >= 0) && (s_dir[n] != '\\') && (s_dir[n] != '/'))
            --n;
        s_dir[n + 1] = '\0';
    }

    {   // Process by line.
        LogToWav    wav;
        int file_index = 0;
        char    line[4096];
        while(fgets(line, sizeof(line), f)) {
            wav.InputLine(line);
            if(wav.IsEnd()) {
                char    path[32];
                sprintf(path, "%s%08d.wav", s_dir, file_index++);
                _mkdir(s_dir);
                if(wav.Save(path))
                    printf("output. \"%s\"\n", path);
                else
                    printf("failed! \"%s\"\n", path);

                wav.Reset();
            }
        }
    }

    fclose(f);
    return  0;
}

