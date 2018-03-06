#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

static unsigned int
str_hash(const char *p)
{
    const unsigned char *s = (const unsigned char *)p;
    unsigned int g, h = 0;

    while (*s != '\0') {
        h = (h << 4) + *s;
        if ((g = h & 0xf0000000UL)) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
        s++;
    }

    return h;
}

static char *
hash_mod(const char *s, unsigned int offset, unsigned int width)
{
	unsigned int value = str_hash(s);
    char *hash;
    char *fmt;

    if (width != 0)
        value %= width;

    asprintf(&fmt, "%%%02dx", offset);
    hash = malloc(20);
    snprintf(hash, 20, fmt, value);
    free(fmt);

    return hash;
}

my_bool
dovecot_hash_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 1 || args->arg_count > 3) {
        strcpy(message, "Wrong number of arguments given to dovecot_hash");
        return 1;
    }
    if (args->arg_type[0] != STRING_RESULT) {
        strcpy(message, "Wrong argument type given to dovecot_hash");
        return 1;
    }
    initid->max_length = args->lengths[0];
    initid->maybe_null = 0;
    return 0;
}

void
dovecot_hash_deinit(UDF_INIT *initid __attribute__((unused)))
{
}

char *
dovecot_hash(UDF_INIT *initid __attribute__((unused)),
             UDF_ARGS *args, char *result, unsigned long *length,
             char *is_null, char *error __attribute__((unused)))
{
    char *str = strndup(args->args[0], args->lengths[0]);
    char *hash;
    unsigned int offset, width;
    size_t argc = args->arg_count;
    size_t len;

    offset = argc > 1 && args->args[1] ? *((long long *)args->args[1]) : 0;
    width  = argc > 2 && args->args[2] ? *((long long *)args->args[2]) : 0;

    hash = hash_mod(str, offset, width);
    len = strlen(hash);
    memcpy(result, hash, len);
    result[len] = '\0';
    *length = len;

    free(str);
    free(hash);

    return result;
}
