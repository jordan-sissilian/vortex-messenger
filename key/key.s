.data
.global _key_bin
_key_bin:
  .space 128 - (.-_key_bin)
  .incbin "key/key.bin"
