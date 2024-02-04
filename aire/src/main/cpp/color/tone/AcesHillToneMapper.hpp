//
// Created by Radzivon Bartoshyk on 04/02/2024.
//

#include "ToneMapper.h"
#include <fast_math-inl.h>

namespace aire {
    template<typename D>
    class AcesHillToneMapper : public ToneMapper<D> {
    private:
        using V = Vec<D>;
        D df_;
        TFromD<D> exposure;

    public:
        AcesHillToneMapper(const TFromD<D> exposure = 1.0f) : exposure(exposure), ToneMapper<D>() {
        }

        ~AcesHillToneMapper() override = default;

        HWY_FAST_MATH_INLINE void Execute(V &R, V &G, V &B) {
            const V mExposure = Set(df_, exposure);
            R = Mul(R, mExposure);
            G = Mul(G, mExposure);
            B = Mul(B, mExposure);
            auto R1 = Add(Add(Mul(R, Set(df_, 0.59719f)), Mul(G, Set(df_, 0.35458f))), Mul(Set(df_, 0.04823f), B));
            auto G1 = Add(Add(Mul(R, Set(df_, 0.07600f)), Mul(G, Set(df_, 0.90834f))), Mul(Set(df_, 0.01566f), B));
            auto C1 = Add(Add(Mul(R, Set(df_, 0.02840f)), Mul(G, Set(df_, 0.13383f))), Mul(Set(df_, 0.83777f), B));

            R = R1;
            G = G1;
            B = C1;

            R = AcesCurve(R);
            G = AcesCurve(G);
            B = AcesCurve(B);

            R1 = Add(Add(Mul(R, Set(df_, 1.60475f)), Mul(G, Set(df_, -0.53108f))), Mul(Set(df_, -0.07367f), B));
            G1 = Add(Add(Mul(R, Set(df_, -0.10208f)), Mul(G, Set(df_, 1.10813f))), Mul(Set(df_, -0.00605f), B));
            C1 = Add(Add(Mul(R, Set(df_, -0.00327f)), Mul(G, Set(df_, -0.07276f))), Mul(Set(df_, 1.07602f), B));

            R = R1;
            G = G1;
            B = C1;
        }

        HWY_FAST_MATH_INLINE void Execute(TFromD<D> &r, TFromD<D> &g, TFromD<D> &b) {
            auto mulInput = [](float& r, float& g, float& b) {
                float a1 = 0.59719f * r + 0.35458f * g + 0.04823f * b,
                        b1 = 0.07600f * r + 0.90834f * g + 0.01566f * b,
                        c1 = 0.02840f * r + 0.13383f * g + 0.83777f * b;
                r = a1;
                g = b1;
                b = c1;
            };

            auto mulOutput = [](float& r, float& g, float& b) {
                float a1 =  1.60475f * r - 0.53108f * g - 0.07367f * b,
                        b1 = -0.10208f * r + 1.10813f * g - 0.00605f * b,
                        c1 = -0.00327f * r - 0.07276f * g + 1.07602f * b;
                r = a1;
                g = b1;
                b = c1;
            };

            r = r*exposure;
            g = g*exposure;
            b = b*exposure;

            mulInput(r,g, b);
            r = AcesCurve(r);
            g = AcesCurve(g);
            b = AcesCurve(b);
            mulOutput(r, g, b);
        }

    private:

        HWY_FAST_MATH_INLINE V AcesCurve(const V Cin) {
            const V a = MulSub(Cin, Add(Cin, Set(df_, 0.0245786f)), Set(df_, 0.000090537f));
            const V b = MulAdd(Cin, MulAdd(Set(df_, 0.983729f), Cin, Set(df_, 0.4329510f)), Set(df_, 0.238081f));
            const V Cout = Div(a, b);
            return Cout;
        }

        HWY_FAST_MATH_INLINE TFromD<D> AcesCurve(const TFromD<D> Cin) {
            const TFromD<D> a = Cin * (Cin + 0.0245786f) - 0.000090537f;
            const TFromD<D> b = Cin * (0.983729f * Cin + 0.4329510f) + 0.238081f;
            const TFromD<D> Cout = a / b;
            return Cout;
        }
    };
}