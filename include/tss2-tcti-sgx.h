#ifndef TSS2_TCTI_SGX
#define TSS2_TCTI_SGX

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

TSS2_RC tss2_tcti_sgx_init (TSS2_TCTI_CONTEXT *context, size_t *size);

#endif /* TSS2_TCTI_SGX */
