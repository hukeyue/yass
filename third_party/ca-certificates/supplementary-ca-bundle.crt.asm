%ifidn __OUTPUT_FORMAT__,win32
$@feat.00 equ 1
global __z_binary_supplementary_ca_bundle_crt_start
global __z_binary_supplementary_ca_bundle_crt_end
section .rdata

__z_binary_supplementary_ca_bundle_crt_start:
ALIGN 4
incbin "supplementary-ca-bundle.crt.zlib"
__z_binary_supplementary_ca_bundle_crt_end:
%else
global _z_binary_supplementary_ca_bundle_crt_start
global _z_binary_supplementary_ca_bundle_crt_end
section .rdata

_z_binary_supplementary_ca_bundle_crt_start:
ALIGN 4
incbin "supplementary-ca-bundle.crt.zlib"
_z_binary_supplementary_ca_bundle_crt_end:
%endif
