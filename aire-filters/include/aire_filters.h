#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

extern "C" {

void aire_reformat_surface_u8_to_linear(const uint8_t *src,
                                        uint32_t src_stride,
                                        const float *dst,
                                        uint32_t dst_stride,
                                        uint32_t width,
                                        uint32_t height);

void aire_reformat_surface_linear_to_u8(const float *src,
                                        uint32_t src_stride,
                                        const uint8_t *dst,
                                        uint32_t dst_stride,
                                        uint32_t width,
                                        uint32_t height);

void aire_reformat_surface_u8_to_laba(const uint8_t *src,
                                      uint32_t src_stride,
                                      const float *dst,
                                      uint32_t dst_stride,
                                      uint32_t width,
                                      uint32_t height);

void aire_reformat_surface_laba_to_u8(const float *src,
                                      uint32_t src_stride,
                                      const uint8_t *dst,
                                      uint32_t dst_stride,
                                      uint32_t width,
                                      uint32_t height);

} // extern "C"
