.section .rdata
.global _z_binary_supplementary_ca_bundle_crt_start
.global _z_binary_supplementary_ca_bundle_crt_end

.align  4
_z_binary_supplementary_ca_bundle_crt_start:
.incbin "supplementary-ca-bundle.crt.zlib"

.align  4
_z_binary_supplementary_ca_bundle_crt_end:
