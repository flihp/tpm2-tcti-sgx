#ifndef TSS2TCTI_SKELETON_PRIV_H
#define TSS2TCTI_SKELETON_PRIV_H

#define TSS2_TCTI_SKELETON_MAGIC 0x1c8e03ff00db0f92

/* This is our private TCTI structure. We're required by the spec to have
 * the same structure as the non-opaque area defined by the
 * TSS2_TCTI_CONTEXT_COMMON_V1 structure. Anything after this data is opaque
 * and private to our implementation. See section 7.3 of the SAPI / TCTI spec
 * for the details.
 */
typedef struct {
    TSS2_TCTI_CONTEXT_COMMON_V1 common;
} TSS2_TCTI_SKELETON_CONTEXT;

#endif /* TSS2TCTI_SKELETON_PRIV_H */
