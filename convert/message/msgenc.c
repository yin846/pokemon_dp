#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define KEY_START 596947
#define KEY_INC 18749

struct string {
    uint32_t offset;
    uint32_t length;
};

struct header {
    uint16_t count;
    uint16_t seed;
    struct string entries[];
};

void crypto_string(void *string, unsigned length, unsigned index)
{
    uint16_t *c = string;
    uint32_t val = KEY_START * (index + 1);
    while (length--) {
        *c++ ^= val;
        val += KEY_INC;
    }
}

void crypto_entry(struct string *data, unsigned index, unsigned seed)
{
    uint32_t val = (uint16_t)(765 * seed * (index + 1));
    val |= val << 16;
    data->offset ^= val;
    data->length ^= val;
}

void encrypt(void *data)
{
    struct header *header = data;

    for (int i = 0; i < header->count; i++ )
    {
        struct string *cur = &header->entries[i];
        crypto_string((char *)data + cur->offset, cur->length, i);
        crypto_entry(cur, i, header->seed);
    }
}

void decrypt(void *data)
{
    struct header *header = data;

    for (int i = 0; i < header->count; i++) {
        struct string *cur = &header->entries[i];
        crypto_entry(cur, i, header->seed);
        crypto_string((char *)data + cur->offset, cur->length, i);
    }
}

void *load_file(const char *path, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    void *data = malloc(*size);
    if (!data) return NULL;
    fread(data, *size, 1, f);
    return data;
}

int write_file(void *data, const char *path, size_t size)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fwrite(data, size, 1, f);
    fclose(f);
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) {
        printf("usage:>msgenc [-d] <datfile>\n");
        printf("[-d] つけると復号モード\n");
        printf("<datfile> 暗号化したいメッセージデータファイル\n");
        return 1;
    }

    char *fname;
    if (argc == 2) {
        fname = argv[1];
    } else {
        fname = argv[2];
    }

    size_t data_len;
    void *data = load_file(fname, &data_len);
    if (!data) {
        printf("%s を読み込めない\n", fname);
        return 1;
    }

    if (argc == 2) {
        encrypt(data);
    } else {
        decrypt(data);
    }

    if (!write_file(data, fname, data_len)) {
        printf("%s に書き込めない\n", fname);
        return 1;
    }

    return 0;
}
