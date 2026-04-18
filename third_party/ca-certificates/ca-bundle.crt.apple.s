.section __DATA_CONST,__const
.private_extern __z_binary_ca_bundle_crt_start
.global __z_binary_ca_bundle_crt_start
.private_extern __z_binary_ca_bundle_crt_end
.global __z_binary_ca_bundle_crt_end
.p2align  6

__z_binary_ca_bundle_crt_start:
.incbin "ca-bundle.crt.zlib"

__z_binary_ca_bundle_crt_end:
.previous
