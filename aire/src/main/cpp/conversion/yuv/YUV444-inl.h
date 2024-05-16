#if defined(SPARKYUV__YUV444_INL_H) == defined(HWY_TARGET_TOGGLE)
#ifdef SPARKYUV__YUV444_INL_H
#undef SPARKYUV__YUV444_INL_H
#else
#define SPARKYUV__YUV444_INL_H
#endif

#include "hwy/highway.h"
#include "yuv-inl.h"

HWY_BEFORE_NAMESPACE();
namespace sparkyuv::HWY_NAMESPACE {
void YUV444ToRGBAHWY(uint8_t *SPARKYUV_RESTRICT dst,
                     const uint32_t dstStride,
                     const uint32_t width,
                     const uint32_t height,
                     const uint8_t *SPARKYUV_RESTRICT yPlane,
                     const uint32_t yStride,
                     const uint8_t *SPARKYUV_RESTRICT uPlane,
                     const uint32_t uStride,
                     const uint8_t *SPARKYUV_RESTRICT vPlane,
                     const uint32_t vStride,
                     const float kr,
                     const float kb) {
  const ScalableTag<int16_t> du16x8;
  const Rebind<uint8_t, decltype(du16x8)> du8x8;
  using VU8x8 = Vec<decltype(du8x8)>;
  using VU16 = Vec<decltype(du16x8)>;

  const VU16 uvCorrection = Set(du16x8, 128);

  auto mYSrc = reinterpret_cast<const uint8_t *>(yPlane);
  auto mUSrc = reinterpret_cast<const uint8_t *>(uPlane);
  auto mVSrc = reinterpret_cast<const uint8_t *>(vPlane);

  const VU8x8 alpha8x8 = Set(du8x8, 255);

  float fCrCoeff = 0.f;
  float fCbCoeff = 0.f;
  float fGCoeff1 = 0.f;
  float fGCoeff2 = 0.f;
  constexpr float flumaCoeff = 255.f / (235.f - 16.0f);
  computeTransform(kr, kb, 255.f, 240.f - 16.f, fCrCoeff, fCbCoeff, fGCoeff1, fGCoeff2);

  int precision = 6;

  const int CrCoeff = static_cast<int>(std::roundf(fCrCoeff * static_cast<float>( 1 << precision )));
  const int CbCoeff = static_cast<int>(std::roundf(fCbCoeff * static_cast<float>( 1 << precision )));
  const int GCoeff1 = static_cast<int>(std::roundf(fGCoeff1 * static_cast<float>( 1 << precision )));
  const int GCoeff2 = static_cast<int>(std::roundf(fGCoeff2 * static_cast<float>( 1 << precision )));

  const int iLumaCoeff = static_cast<int>(std::roundf(flumaCoeff * static_cast<float>( 1 << precision )));

  const VU16 ivLumaCoeff = Set(du16x8, iLumaCoeff);
  const VU16 ivCrCoeff = Set(du16x8, CrCoeff);
  const VU16 ivCbCoeff = Set(du16x8, CbCoeff);
  const VU16 ivGCoeff1 = Set(du16x8, GCoeff1);
  const VU16 ivGCoeff2 = Set(du16x8, GCoeff2);

  const VU16 vZero = Zero(du16x8);

  const auto uvCorrIY = Set(du16x8, 16);

  const int lanes = Lanes(du8x8);

  for (int y = 0; y < height; ++y) {
    auto uSource = reinterpret_cast<const uint8_t *>(mUSrc);
    auto vSource = reinterpret_cast<const uint8_t *>(mVSrc);
    auto ySrc = reinterpret_cast<const uint8_t *>(mYSrc);
    auto store = reinterpret_cast<uint8_t *>(dst);

    uint32_t x = 0;

    for (; x + lanes < width; x += lanes) {
      const VU16 luma = Mul(Sub(PromoteTo(du16x8, LoadU(du8x8, ySrc)), uvCorrIY), ivLumaCoeff);
      const VU16 ul = Sub(PromoteTo(du16x8, LoadU(du8x8, uSource)), uvCorrection);
      const VU16 vl = Sub(PromoteTo(du16x8, LoadU(du8x8, vSource)), uvCorrection);

      const VU16 r = Max(ShiftRight<6>(MulAdd(ivCrCoeff, vl, luma)), vZero);
      const VU16 b = Max(ShiftRight<6>(MulAdd(ivCbCoeff, ul, luma)), vZero);
      const VU16 g = Max(ShiftRight<6>(NegMulAdd(ivGCoeff2, ul, NegMulAdd(ivGCoeff1, vl, luma))), vZero);

      StoreInterleaved4(DemoteTo(du8x8, r), DemoteTo(du8x8, g), DemoteTo(du8x8, b), alpha8x8, du8x8, store);

      store += lanes * 4;
      ySrc += lanes;
      uSource += lanes;
      vSource += lanes;
    }

    for (; x < width; ++x) {
      const uint8_t uValue = reinterpret_cast<const uint8_t *>(uSource)[0];
      const uint8_t vValue = reinterpret_cast<const uint8_t *>(vSource)[0];

      int Y = (static_cast<int>(ySrc[0]) - 16) * iLumaCoeff;
      const int Cr = (static_cast<int>(vValue) - 128);
      const int Cb = (static_cast<int>(uValue) - 128);

      int R = (Y + CrCoeff * Cr) >> precision;
      int B = (Y + CbCoeff * Cb) >> precision;
      int G = (Y - GCoeff1 * Cr - GCoeff2 * Cb) >> precision;

      store[0] = static_cast<uint8_t>(std::clamp(R, 0, 255));
      store[1] = static_cast<uint8_t>(std::clamp(G, 0, 255));
      store[2] = static_cast<uint8_t>(std::clamp(B, 0, 255));
      store[3] = 255;
      store += 4;
      ySrc += 1;
      vSource += 1;
      uSource += 1;
    }

    mUSrc += uStride;
    mVSrc += vStride;
    mYSrc += yStride;
    dst += dstStride;
  }
}
}
HWY_AFTER_NAMESPACE();

#endif