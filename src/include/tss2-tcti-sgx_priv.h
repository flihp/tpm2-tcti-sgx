#ifndef TSS2_TCTI_SGX_PRIV_H
#define TSS2_TCTI_SGX_PRIV_H

/* generate your own:
 * cat /dev/random | tr -dc 'a-f0-9' | fold -w 16 | head -n 1
 */
#define TSS2_TCTI_SGX_MAGIC 0x4e50bc1dcdb7623c

/* This is our private TCTI structure. We're required by the spec to have
 * the same structure as the non-opaque area defined by the
 * TSS2_TCTI_CONTEXT_COMMON_V1 structure. Anything after this data is opaque
 * and private to our implementation. See section 7.3 of the SAPI / TCTI spec
 * for the details.
 */
typedef struct {
    TSS2_TCTI_CONTEXT_COMMON_V1 common;
} TSS2_TCTI_CONTEXT_SGX;

#endif /* TSS2_TCTI_SGX_PRIV_H */
