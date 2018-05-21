/* MIT License
 *
 * Copyright (c) 2017 spino.tech Guillaume Chereau & Raphael Seghier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "spino.h"
#include "uthash.h"

typedef struct {
    char *key;
    char *value;
    UT_hash_handle hh;
} entry_t;

static entry_t *entries;

bool storage_remove(const char *key)
{
    entry_t *e;
    HASH_FIND_STR(entries, key, e);
    if (e == NULL)
        return false;
    HASH_DEL(entries, e);
    free(e->key);
    free(e->value);
    free(e);
    return true;
}

void storage_init(void)
{
    FILE *file;
    char line[256];
    char *key, *value;
    const char *path = get_data_path("storage.txt");
    entry_t *e;
    entries = NULL;
    LOG_D("Read storage from file %s", path);
    file = fopen(path, "r");
    if (!file) {
        storage_set_int("storage-version", SPINO_VERSION_CODE);
        return;
    }
    while(fgets(line, 256, file)) {
        key = strtok(line, "=\n");
        value = strtok(NULL, "=\n");
        e = malloc(sizeof(entry_t));
        e->key = strdup(key);
        e->value = strdup(value);
        HASH_ADD_KEYPTR(hh, entries, e->key, strlen(e->key), e);
    }
    fclose(file);
}

static void storage_save()
{
    FILE *file;
    entry_t *e, *tmp;
    const char *path = get_data_path("storage.txt");
    file = fopen(path, "w");
    HASH_ITER(hh, entries, e, tmp)
        fprintf(file, "%s=%s\n", e->key, e->value);
    fclose(file);
}

void storage_set(const char *key, const char *value)
{
    entry_t *e;
    LOG_D("Storage set %s=%s", key, value);
    e = malloc(sizeof(entry_t));
    e->key = strdup(key);
    e->value = strdup(value);
    storage_remove(key);
    HASH_ADD_KEYPTR(hh, entries, e->key, strlen(e->key), e);
    storage_save();
}

const char *storage_get(const char *key, const char *default_value)
{
    entry_t *e;
    HASH_FIND_STR(entries, key, e);
    return e ? e->value : default_value;
}

void storage_set_int(const char *key, int value)
{
    char *value_str;
    asprintf(&value_str, "%d", value);
    storage_set(key, value_str);
    free(value_str);
}

int storage_get_int(const char *key, int default_value)
{
    entry_t *e;
    HASH_FIND_STR(entries, key, e);
    return e ? atoi(e->value) : default_value;
}

