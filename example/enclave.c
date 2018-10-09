#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "enclave_t.h"
/*
 * printf equivalent for use in the enclave
 */
void enc_printf (const char *fmt, ...)
{
    char buf [BUFSIZ] = { '\0' };
    va_list ap;
    va_start (ap, fmt);
    vsnprintf (buf, BUFSIZ, fmt, ap);
    va_end (ap);
    print_string (buf);
}
/*
 * get the manufacturer ID from the TPM using the GetCapability command
 */
uint32_t
getcap_manufacturer(void)
{
    return 666;
}
