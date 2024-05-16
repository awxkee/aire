#if defined(SPARKYUV__YUV420_INL_H) == defined(HWY_TARGET_TOGGLE)
#ifdef SPARKYUV__YUV420_INL_H
#undef SPARKYUV__YUV420_INL_H
#else
#define SPARKYUV__YUV420_INL_H
#endif

#include "hwy/highway.h"
#include "yuv-inl.h"

HWY_BEFORE_NAMESPACE();
namespace sparkyuv::HWY_NAMESPACE {
using namespace hwy;
using namespace hwy::HWY_NAMESPACE;

void
YUV420ToRGBAHWYPrecise(uint8_t *SPARKYUV_RESTRICT rgba,
                       const uint32_t rgbaStride,
                       const uint32_t width,
                       const uint32_t height,
                       const uint8_t *SPARKYUV_RESTRICT ySrc,
                       const uint32_t yPlaneStride,
                       const uint8_t *SPARKYUV_RESTRICT uSrc,
                       const uint32_t uPlaneStride,
                       const uint8_t *SPARKYUV_RESTRICT vSrc,
                       const uint32_t vPlaneStride,
                       const int bitDepth,
                       const float kr,
                       const float kb) {
  const float kg = 1.0f - kr - kb;
  assert(kg != 0.f);

  const FixedTag<uint8_t, 1> duo;
  const FixedTag<float, 1> dfo;

  const FixedTag<uint8_t, 4> du;
  const FixedTag<float, 4> df;

  using VU = Vec<decltype(du)>;
  using VF = Vec<decltype(df)>;

  using VUO = Vec<decltype(duo)>;
  using VFO = Vec<decltype(dfo)>;

  const VFO biasYo = Set(dfo, static_cast<float>(16.0f));
  const VFO biasUVo = Set(dfo, static_cast<float>(128.0f));

  const VF biasY = Set(df, static_cast<float>(16.0f));
  const VF biasUV = Set(df, static_cast<float>(128.0f));

  const VUO alphao = Set(duo, 255);

  const VU alpha = Set(du, 255);

  float CrCoeff = 0.f;
  float CbCoeff = 0.f;
  float GCoeff1 = 0.f;
  float GCoeff2 = 0.f;
  constexpr float lumaCoeff = 255.f / (235.f - 16.0f);
  computeTransform(kr, kb, 255.f, 240.f - 16.f, CrCoeff, CbCoeff, GCoeff1, GCoeff2);

  const VFO vLumaCoeffo = Set(dfo, lumaCoeff);
  const VFO vCrCoeffo = Set(dfo, CrCoeff);
  const VFO vCbCoeffo = Set(dfo, CbCoeff);
  const VFO vGCoeffo = Set(dfo, GCoeff1);
  const VFO vGCoeff2o = Set(dfo, GCoeff2);

  const VF vLumaCoeff = Set(df, lumaCoeff);
  const VF vCrCoeff = Set(df, CrCoeff);
  const VF vCbCoeff = Set(df, CbCoeff);
  const VF vGCoeff = Set(df, GCoeff1);
  const VF vGCoeff2 = Set(df, GCoeff2);

  const auto weight0 = Set(df, static_cast<float>(9.0f / 16.0f));
  const auto weight1 = Set(df, static_cast<float>(3.0f / 16.0f));
  const auto weight2 = Set(df, static_cast<float>(3.0f / 16.0f));
  const auto weight3 = Set(df, static_cast<float>(1.0f / 16.0f));

  const auto weight0o = Set(dfo, static_cast<float>(9.0f / 16.0f));
  const auto weight1o = Set(dfo, static_cast<float>(3.0f / 16.0f));
  const auto weight2o = Set(dfo, static_cast<float>(3.0f / 16.0f));
  const auto weight3o = Set(dfo, static_cast<float>(1.0f / 16.0f));

  auto mDst = reinterpret_cast<uint8_t *>(reinterpret_cast<uint8_t *>(rgba));
  auto mSrcY = reinterpret_cast<const uint8_t *>(reinterpret_cast<const int8_t *>(ySrc));

  for (uint32_t y = 0; y < height; ++y) {
    uint32_t x = 0;

    auto dst = reinterpret_cast<uint8_t *>(mDst);
    auto yCurrentSrc = reinterpret_cast<const uint8_t *>(mSrcY);

    uint32_t uy = std::min(y / 2, height / 2 - 1);
    uint32_t uyN = std::min(y / 2 + 1, height / 2 - 1);

    auto uCurrentSrc = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(uSrc) + uy * uPlaneStride);
    auto uNextSrc = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(uSrc) + uyN * uPlaneStride);

    auto vCurrentSrc = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(vSrc) + uy * vPlaneStride);
    auto vNextSrc = reinterpret_cast<const uint8_t *>(reinterpret_cast<const uint8_t *>(vSrc) + uyN * vPlaneStride);

    auto maxUCurrentSrc = uCurrentSrc + width / 2 - 1;
    auto maxUNextSrc = uNextSrc + width / 2 - 1;

    auto maxVCurrentSrc = vCurrentSrc + width / 2 - 1;
    auto maxVNextSrc = vNextSrc + width / 2 - 1;

    for (; x + 8 < width; x += 8) {
      VF ey = PromoteTo(df, LoadU(du, yCurrentSrc));
      ey = Sub(ey, biasY);

      const VF uo0 = PromoteTo(df, Load(du, uCurrentSrc));
      const VF uo1 = PromoteTo(df, Load(du, uCurrentSrc + 4));
      const VF uo2 = PromoteTo(df, Load(du, uNextSrc));
      const VF uo3 = PromoteTo(df, Load(du, uNextSrc + 4));

      VF u = MulAdd(uo0, weight0, MulAdd(uo1, weight1, MulAdd(uo2, weight2, Mul(uo3, weight3))));
      u = Sub(u, biasUV);

      const VF vo0 = PromoteTo(df, Load(du, vCurrentSrc));
      const VF vo1 = PromoteTo(df, Load(du, vCurrentSrc + 4));
      const VF vo2 = PromoteTo(df, Load(du, vNextSrc));
      const VF vo3 = PromoteTo(df, Load(du, vNextSrc + 4));

      VF v = MulAdd(vo0, weight0, MulAdd(vo1, weight1, MulAdd(vo2, weight2, Mul(vo3, weight3))));
      v = Sub(v, biasUV);

      VF R, G, B;

      const VF ulow = InterleaveLower(df, u, u);
      const VF vlow = InterleaveLower(df, v, v);
      const VF uhigh = InterleaveUpper(df, u, u);
      const VF vhigh = InterleaveUpper(df, v, v);

      YUVToRGBLimited(df, ey, ulow, vlow, R, G, B, vLumaCoeff, vCrCoeff, vCbCoeff, vGCoeff, vGCoeff2);

      StoreInterleaved4(DemoteTo(du, R), DemoteTo(du, G), DemoteTo(du, B), alpha, du, dst);
      dst += 4 * 4;
      yCurrentSrc += 4;
      uCurrentSrc += 4;
      vNextSrc += 4;
      uNextSrc += 4;
      vCurrentSrc += 4;

      if (x + 1 < width) {
        ey = PromoteTo(df, LoadU(du, yCurrentSrc));
        ey = Sub(ey, biasY);
        YUVToRGBLimited(df, ey, uhigh, vhigh, R, G, B, vLumaCoeff, vCrCoeff, vCbCoeff, vGCoeff, vGCoeff2);

        StoreInterleaved4(DemoteTo(du, R), DemoteTo(du, G), DemoteTo(du, B), alpha, du, dst);
        dst += 4 * 4;
        yCurrentSrc += 4;
      }
    }

    for (; x < width; x += 2) {
      VFO ey = PromoteTo(dfo, LoadU(duo, yCurrentSrc));
      ey = Sub(ey, biasYo);

      const VFO uo0 = PromoteTo(dfo, Load(duo, uCurrentSrc));
      const VFO uo1 = PromoteTo(dfo, Load(duo, std::min(uCurrentSrc + 1, maxUCurrentSrc)));
      const VFO uo2 = PromoteTo(dfo, Load(duo, uNextSrc));
      const VFO uo3 = PromoteTo(dfo, Load(duo, std::min(uNextSrc + 1, maxUNextSrc)));

      VFO u = MulAdd(uo0, weight0o, MulAdd(uo1, weight1o, MulAdd(uo2, weight2o, Mul(uo3, weight3o))));
      u = Sub(u, biasUVo);

      const VFO vo0 = PromoteTo(dfo, Load(duo, vCurrentSrc));
      const VFO vo1 = PromoteTo(dfo, Load(duo, std::min(vCurrentSrc + 1, maxVCurrentSrc)));
      const VFO vo2 = PromoteTo(dfo, Load(duo, vNextSrc));
      const VFO vo3 = PromoteTo(dfo, Load(duo, std::min(vNextSrc + 1, maxVNextSrc)));

      VFO v = MulAdd(vo0, weight0o, MulAdd(vo1, weight1o, MulAdd(vo2, weight2o, Mul(vo3, weight3o))));
      v = Sub(v, biasUVo);

      VFO R, G, B;
      YUVToRGBLimited(dfo, ey, u, v, R, G, B, vLumaCoeffo, vCrCoeffo, vCbCoeffo, vGCoeffo, vGCoeff2o);

      StoreInterleaved4(DemoteTo(duo, R), DemoteTo(duo, G), DemoteTo(duo, B), alphao, duo, dst);
      dst += 4;

      vCurrentSrc += 1;
      vNextSrc += 1;
      uNextSrc += 1;
      uCurrentSrc += 1;

      if (x + 1 < width) {
        ey = PromoteTo(dfo, LoadU(duo, yCurrentSrc + 1));
        ey = Sub(ey, biasYo);
        YUVToRGBLimited(dfo, ey, u, v, R, G, B, vLumaCoeffo, vCrCoeffo, vCbCoeffo, vGCoeffo, vGCoeff2o);

        StoreInterleaved4(DemoteTo(duo, R), DemoteTo(duo, G), DemoteTo(duo, B), alphao, duo, dst);
        dst += 4;
      }
    }

    mDst += rgbaStride;
    mSrcY += yPlaneStride;
  }
}

void
YUV420ToRGBAHWY(uint8_t *SPARKYUV_RESTRICT dst,
                const uint32_t rgbaStride,
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
  const Half<decltype(du8x8)> du8h;
  using VU8x8 = Vec<decltype(du8x8)>;
  using VU16 = Vec<decltype(du16x8)>;
  const Half<decltype(du16x8)> du16h;
  using VU16H = Vec<decltype(du16h)>;

  const VU16H uvCorrection = Set(du16h, 128);

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
  const int uvLanes = Lanes(du8h);

  for (int y = 0; y < height; ++y) {
    auto uSource = reinterpret_cast<const uint8_t *>(mUSrc);
    auto vSource = reinterpret_cast<const uint8_t *>(mVSrc);
    auto ySrc = reinterpret_cast<const uint8_t *>(mYSrc);
    auto store = reinterpret_cast<uint8_t *>(dst);

    uint32_t x = 0;

    for (; x + lanes < width; x += lanes) {
      const VU16 luma = Mul(Sub(PromoteTo(du16x8, LoadU(du8x8, ySrc)), uvCorrIY), ivLumaCoeff);
      const VU16H ulFull = Sub(PromoteTo(du16h, LoadU(du8h, uSource)), uvCorrection);
      const VU16H vlFull = Sub(PromoteTo(du16h, LoadU(du8h, vSource)), uvCorrection);

      const auto ull = InterleaveLower(ulFull, ulFull);
      const auto ulh = InterleaveUpper(du16h, ulFull, ulFull);
      const auto ulf = Combine(du16x8, ulh, ull);

      const auto vll = InterleaveLower(vlFull, vlFull);
      const auto vlh = InterleaveUpper(du16h, vlFull, vlFull);
      const auto vlf = Combine(du16x8, vlh, vll);

      const VU16 r = Max(ShiftRight<6>(MulAdd(ivCrCoeff, vlf, luma)), vZero);
      const VU16 b = Max(ShiftRight<6>(MulAdd(ivCbCoeff, ulf, luma)), vZero);
      const VU16 g = Max(ShiftRight<6>(NegMulAdd(ivGCoeff2, ulf, NegMulAdd(ivGCoeff1, vlf, luma))), vZero);

      StoreInterleaved4(DemoteTo(du8x8, r), DemoteTo(du8x8, g), DemoteTo(du8x8, b), alpha8x8, du8x8, store);

      store += lanes * 4;
      ySrc += lanes;

      uSource += uvLanes;
      vSource += uvLanes;
    }

    for (; x < width; x += 2) {
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

      if (x + 1 < width) {
        Y = (static_cast<int>(ySrc[0]) - 16) * iLumaCoeff;
        R = (Y + CrCoeff * Cr) >> precision;
        B = (Y + CbCoeff * Cb) >> precision;
        G = (Y - GCoeff1 * Cr - GCoeff2 * Cb) >> precision;
        store[0] = static_cast<uint8_t>(std::clamp(R, 0, 255));
        store[1] = static_cast<uint8_t>(std::clamp(G, 0, 255));
        store[2] = static_cast<uint8_t>(std::clamp(B, 0, 255));
        store[3] = 255;
        store += 4;
        ySrc += 1;
      }

      vSource += 1;
      uSource += 1;
    }

    if (y & 1) {
      mUSrc += uStride;
      mVSrc += vStride;
    }
    mYSrc += yStride;
    dst += rgbaStride;
  }
}

void
YUV420ToRGBHWY(uint8_t *SPARKYUV_RESTRICT dst,
                const uint32_t rgbStride,
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
  const Half<decltype(du8x8)> du8h;
  using VU16 = Vec<decltype(du16x8)>;
  const Half<decltype(du16x8)> du16h;
  using VU16H = Vec<decltype(du16h)>;

  const VU16H uvCorrection = Set(du16h, 128);

  auto mYSrc = reinterpret_cast<const uint8_t *>(yPlane);
  auto mUSrc = reinterpret_cast<const uint8_t *>(uPlane);
  auto mVSrc = reinterpret_cast<const uint8_t *>(vPlane);

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
  const int uvLanes = Lanes(du8h);

  for (int y = 0; y < height; ++y) {
    auto uSource = reinterpret_cast<const uint8_t *>(mUSrc);
    auto vSource = reinterpret_cast<const uint8_t *>(mVSrc);
    auto ySrc = reinterpret_cast<const uint8_t *>(mYSrc);
    auto store = reinterpret_cast<uint8_t *>(dst);

    uint32_t x = 0;

    for (; x + lanes < width; x += lanes) {
      const VU16 luma = Mul(Sub(PromoteTo(du16x8, LoadU(du8x8, ySrc)), uvCorrIY), ivLumaCoeff);
      const VU16H ulFull = Sub(PromoteTo(du16h, LoadU(du8h, uSource)), uvCorrection);
      const VU16H vlFull = Sub(PromoteTo(du16h, LoadU(du8h, vSource)), uvCorrection);

      const auto ull = InterleaveLower(ulFull, ulFull);
      const auto ulh = InterleaveUpper(du16h, ulFull, ulFull);
      const auto ulf = Combine(du16x8, ulh, ull);

      const auto vll = InterleaveLower(vlFull, vlFull);
      const auto vlh = InterleaveUpper(du16h, vlFull, vlFull);
      const auto vlf = Combine(du16x8, vlh, vll);

      const VU16 r = Max(ShiftRight<6>(MulAdd(ivCrCoeff, vlf, luma)), vZero);
      const VU16 b = Max(ShiftRight<6>(MulAdd(ivCbCoeff, ulf, luma)), vZero);
      const VU16 g = Max(ShiftRight<6>(NegMulAdd(ivGCoeff2, ulf, NegMulAdd(ivGCoeff1, vlf, luma))), vZero);

      StoreInterleaved3(DemoteTo(du8x8, r), DemoteTo(du8x8, g), DemoteTo(du8x8, b), du8x8, store);

      store += lanes * 3;
      ySrc += lanes;

      uSource += uvLanes;
      vSource += uvLanes;
    }

    for (; x < width; x += 2) {
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
      store += 3;
      ySrc += 1;

      if (x + 1 < width) {
        Y = (static_cast<int>(ySrc[0]) - 16) * iLumaCoeff;
        R = (Y + CrCoeff * Cr) >> precision;
        B = (Y + CbCoeff * Cb) >> precision;
        G = (Y - GCoeff1 * Cr - GCoeff2 * Cb) >> precision;
        store[0] = static_cast<uint8_t>(std::clamp(R, 0, 255));
        store[1] = static_cast<uint8_t>(std::clamp(G, 0, 255));
        store[2] = static_cast<uint8_t>(std::clamp(B, 0, 255));
        store += 3;
        ySrc += 1;
      }

      vSource += 1;
      uSource += 1;
    }

    if (y & 1) {
      mUSrc += uStride;
      mVSrc += vStride;
    }
    mYSrc += yStride;
    dst += rgbStride;
  }
}

}
HWY_AFTER_NAMESPACE();

#endif