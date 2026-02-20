.section .rodata
.hidden _binary_supplementary_ca_bundle_crt_start
.global _binary_supplementary_ca_bundle_crt_start
.type   _binary_supplementary_ca_bundle_crt_start, %object
.hidden _binary_supplementary_ca_bundle_crt_end
.global _binary_supplementary_ca_bundle_crt_end
.type   _binary_supplementary_ca_bundle_crt_end, %object
.align  4

_binary_supplementary_ca_bundle_crt_start:
.incbin "supplementary-ca-bundle.crt"

_binary_supplementary_ca_bundle_crt_end:
.previous

#ifdef USE_CET
#if defined(_M_X64) || defined(__x86_64__)
.include "cet.64.s"
#elif defined(_M_IX86) || defined(__i386__)
.include "cet.32.s"
#endif
#endif
