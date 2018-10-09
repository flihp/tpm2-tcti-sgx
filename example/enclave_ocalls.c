#include <stdio.h>
#include <sgx_error.h>

sgx_status_t
print_string (const char *str)
{
    printf ("%s", str);
}
