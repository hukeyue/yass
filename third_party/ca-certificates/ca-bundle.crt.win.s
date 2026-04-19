.section .rdata
.global _z_binary_ca_bundle_crt_start
.global _z_binary_ca_bundle_crt_end

.align  4
_z_binary_ca_bundle_crt_start:
.incbin "ca-bundle.crt.zlib"

.align  4
_z_binary_ca_bundle_crt_end:
