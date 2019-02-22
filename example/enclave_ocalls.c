#include <stdio.h>
#include <sgx_error.h>

sgx_status_t
print_string (const char *str)
{
    if (printf ("%s", str) >= 0)
        return SGX_SUCCESS;
    else
        return SGX_ERROR_UNEXPECTED;
}
