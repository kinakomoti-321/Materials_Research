#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <Material/bsdf.hpp>
#include <Sampler/sampler.hpp>
#include <Core/constant.hpp>
#include <Math/vecop.hpp>
#include <Math/bsdf_math.hpp>
#include <Core/constant.hpp>
#include <Core/logger.hpp>

using namespace glm;

namespace MR_BSDF
{
    class IrienscentGGX : public BSDF
    {
    private:
        float _alpha_x;
        float _alpha_y;
        const float _val[3] = {5.4856e-13, 4.4201e-13, 5.2481e-13};
        const float _pos[3] = {1.6810e+06, 1.7953e+06, 2.2084e+06};
        const float _var[3] = {4.3278e+09, 9.3046e+09, 6.6121e+09};

        vec3 _ior;
        vec3 _extinction;
        float _tf_thickness;
        float _tf_ior;
        vec3 _F0;
        vec3 _F90;
        float _expornent;

    public:
        IrienscentGGX(float alpha_x, float alpha_y)
        {
            _alpha_x = alpha_x;
            _alpha_y = alpha_y;

            _tf_thickness = 300;
            _tf_ior = 2.3;

            _ior = vec3(2.9, 2.9, 2.9);
            _extinction = vec3(2.99, 2.99, 2.99);

            _expornent = 1.0;
            _F0 = vec3(0.8);
            _F90 = vec3(1.0);
        }

        float ggx_D(const vec3 &wm) const
        {
            float term = wm.x * wm.x / (_alpha_x * _alpha_x) + wm.z * wm.z / (_alpha_y * _alpha_y) + wm.y * wm.y;
            return 1.0f / (PI * _alpha_x * _alpha_y * term * term);
        }

        float lambda(const vec3 &w) const
        {
            float term = (w.x * w.x * _alpha_x * _alpha_x + w.z * w.z * _alpha_y * _alpha_y) / (w.y * w.y);
            return (-1.0f + std::sqrt(1.0f + term)) / 2.0f;
        }

        float smith_G1(const vec3 &w) const
        {
            return 1.0f / (lambda(w) + 1.0f);
        }

        float smith_G2(const vec3 &wo, const vec3 &wi, const vec3 &wm) const
        {
            return 1.0 / (1.0 + lambda(wo) + lambda(wi));
        }

        vec3 sampling_m(float u, float v, float &pdf) const
        {
            float tanTerm = std::tan(2.0f * PI * u) * _alpha_x / _alpha_y;
            float phi = std::atan(tanTerm);
            if (u >= 0.75)
            {
                phi += 2.0f * PI;
            }
            else if (u > 0.25)
            {
                phi += PI;
            }

            const float cosphi = std::cos(phi);
            const float sinphi = std::sin(phi);

            const float term_alpha = cosphi * cosphi / (_alpha_x * _alpha_x) + sinphi * sinphi / (_alpha_y * _alpha_y);
            const float term = v / ((1.0f - v) * term_alpha);

            float theta = std::atan(std::sqrt(term));

            const vec3 wh = MR_Math::sphericalDirection(theta, phi);
            pdf = ggx_D(wh) * BSDFMath::cosTheta(wh);

            return wh;
        }

        vec3 visibleNormalSampling(const vec3 &V_, float u, float v, float &pdf) const
        {
            vec3 Vh = normalize(vec3(_alpha_x * V_.x, V_.y, _alpha_y * V_.z));

            vec3 n = vec3(0, 1, 0);
            if (Vh.y > 0.99)
                n = vec3(0, 0, -1);

            vec3 T1 = normalize(cross(Vh, n));
            vec3 T2 = cross(T1, Vh);

            float r = std::sqrt(u);
            float phi = 2.0 * PI * v;
            float t1 = r * std::cos(phi);
            float t2 = r * std::sin(phi);
            float s = 0.5 * (1.0 + Vh.y);
            t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

            vec3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * Vh;

            vec3 Ne = normalize(vec3(_alpha_x * Nh.x, Nh.y, _alpha_y * Nh.z));

            return Ne;
        }

        // https://github.com/AcademySoftwareFoundation/MaterialX/blob/main/libraries/pbrlib/genglsl/lib/mx_microfacet_specular.glsl#L370-L489
        void fresnel_dielectric_polarized(float cosTheta, float n, float &Rp, float &Rs) const
        {
            if (cosTheta < 0.0)
            {
                Rp = 1.0;
                Rs = 1.0;
                return;
            }

            float cosTheta2 = cosTheta * cosTheta;
            float sinTheta2 = 1.0 - cosTheta2;
            float n2 = n * n;

            float t0 = n2 - sinTheta2;
            float a2plusb2 = std::sqrt(t0 * t0);
            float t1 = a2plusb2 + cosTheta2;
            float a = std::sqrt(std::max(0.5 * (a2plusb2 + t0), 0.0));
            float t2 = 2.0 * a * cosTheta;
            Rs = (t1 - t2) / (t1 + t2);

            float t3 = cosTheta2 * a2plusb2 + sinTheta2 * sinTheta2;
            float t4 = t2 * sinTheta2;
            Rp = Rs * (t3 - t4) / (t3 + t4);
        }

        void fresnel_dielectric_polarized(float cosTheta, float eta1, float eta2, float &Rp, float &Rs) const
        {
            float n = eta2 / eta1;
            fresnel_dielectric_polarized(cosTheta, n, Rp, Rs);
        }

        void fresnel_conductor_polarized(float cosTheta, vec3 n, vec3 k, vec3 &Rp, vec3 &Rs) const
        {
            cosTheta = std::clamp(cosTheta, 0.0f, 1.0f);
            float cosTheta2 = cosTheta * cosTheta;
            float sinTheta2 = 1.0 - cosTheta2;
            vec3 n2 = n * n;
            vec3 k2 = k * k;

            vec3 t0 = n2 - k2 - vec3(sinTheta2);
            vec3 a2plusb2 = sqrt(t0 * t0 + 4.0f * n2 * k2);
            vec3 t1 = a2plusb2 + vec3(cosTheta2);
            vec3 a = sqrt(max(0.5f * (a2plusb2 + t0), 0.0f));
            vec3 t2 = 2.0f * a * cosTheta;
            Rs = (t1 - t2) / (t1 + t2);

            vec3 t3 = cosTheta2 * a2plusb2 + vec3(sinTheta2 * sinTheta2);
            vec3 t4 = t2 * sinTheta2;
            Rp = Rs * (t3 - t4) / (t3 + t4);
        }

        void fresnel_conductor_polarized(float cosTheta, float eta1, vec3 eta2, vec3 kappa2, vec3 &Rp, vec3 &Rs) const
        {
            vec3 n = eta2 / eta1;
            vec3 k = kappa2 / eta1;
            fresnel_conductor_polarized(cosTheta, n, k, Rp, Rs);
        }

        vec3 fresnel_conductor(float cosTheta, vec3 n, vec3 k) const
        {
            vec3 Rp, Rs;
            fresnel_conductor_polarized(cosTheta, n, k, Rp, Rs);
            return 0.5f * (Rp + Rs);
        }

        // Phase shift due to a dielectric material
        void fresnel_dielectric_phase_polarized(float cosTheta, float eta1, float eta2, float &phiP, float &phiS) const
        {
            float cosB = std::cos(std::atan(eta2 / eta1)); // Brewster's angle
            if (eta2 > eta1)
            {
                phiP = cosTheta < cosB ? M_PI : 0.0f;
                phiS = 0.0f;
            }
            else
            {
                phiP = cosTheta < cosB ? 0.0f : M_PI;
                phiS = M_PI;
            }
        }

        float square(const float &a) const
        {
            return a * a;
        }

        vec3 square(const vec3 &a) const
        {
            return a * a;
        }

        // Phase shift due to a conducting material
        void fresnel_conductor_phase_polarized(float cosTheta, float eta1, vec3 eta2, vec3 kappa2, vec3 &phiP, vec3 &phiS) const
        {
            if (kappa2 == vec3(0, 0, 0) && eta2.x == eta2.y && eta2.y == eta2.z)
            {
                // Use dielectric formula to increase performance
                fresnel_dielectric_phase_polarized(cosTheta, eta1, eta2.x, phiP.x, phiS.x);
                phiP = vec3(phiP.x, phiP.x, phiP.x);
                phiS = vec3(phiS.x, phiS.x, phiS.x);
                return;
            }
            vec3 k2 = kappa2 / eta2;
            vec3 sinThetaSqr = vec3(1.0) - cosTheta * cosTheta;
            vec3 A = eta2 * eta2 * (vec3(1.0) - k2 * k2) - eta1 * eta1 * sinThetaSqr;
            vec3 B = sqrt(A * A + square(2.0f * eta2 * eta2 * k2));
            vec3 U = sqrt((A + B) / 2.0f);
            vec3 V = max(vec3(0.0), sqrt((B - A) / 2.0f));

            phiS = atan(2.0f * eta1 * V * cosTheta, U * U + V * V - square(eta1 * cosTheta));
            phiP = atan(2.0f * eta1 * eta2 * eta2 * cosTheta * (2.0f * k2 * U - (vec3(1.0) - k2 * k2) * V),
                        square(eta2 * eta2 * (vec3(1.0) + k2 * k2) * cosTheta) - eta1 * eta1 * (U * U + V * V));
        }

        // Evaluation XYZ sensitivity curves in Fourier space
        vec3 eval_sensitivity(float opd, vec3 shift) const
        {
            // Use Gaussian fits, given by 3 parameters: val, pos and var
            float phase = 2.0 * M_PI * opd;
            vec3 val = vec3(5.4856e-13, 4.4201e-13, 5.2481e-13);
            vec3 pos = vec3(1.6810e+06, 1.7953e+06, 2.2084e+06);
            vec3 var = vec3(4.3278e+09, 9.3046e+09, 6.6121e+09);
            vec3 xyz = val * sqrt(2.0f * float(M_PI) * var) * cos(pos * phase + shift) * exp(-var * phase * phase);
            xyz.x += 9.7470e-14 * std::sqrt(2.0 * M_PI * 4.5282e+09) * std::cos(2.2399e+06 * phase + shift[0]) * std::exp(-4.5282e+09 * phase * phase);
            return xyz / float(1.0685e-7);
        }

        vec3 fresnel_airy(float cosTheta, vec3 ior, vec3 extinction, float tf_thickness, float tf_ior, vec3 f0, vec3 f90, float exponent, bool use_schlick) const
        {
            // Convert nm -> m
            float d = tf_thickness * 1.0e-9;

            // Assume vacuum on the outside
            float eta1 = 1.0;
            float eta2 = std::max(tf_ior, eta1);
            vec3 eta3 = ior;
            vec3 kappa3 = extinction;

            // Compute the Spectral versions of the Fresnel reflectance and
            // transmitance for each interface.
            float R12p, T121p, R12s, T121s;
            vec3 R23p, R23s;

            // Reflected and transmitted parts in the thin film
            fresnel_dielectric_polarized(cosTheta, eta1, eta2, R12p, R12s);

            // Reflected part by the base
            float scale = eta1 / eta2;
            float cosThetaTSqr = 1.0 - (1.0 - cosTheta * cosTheta) * scale * scale;
            float cosTheta2 = std::sqrt(cosThetaTSqr);

            fresnel_conductor_polarized(cosTheta2, eta2, eta3, kappa3, R23p, R23s);

            // Check for total internal reflection
            if (cosThetaTSqr <= 0.0f)
            {
                R12s = 1.0;
                R12p = 1.0;
            }

            // Compute the transmission coefficients
            T121p = 1.0 - R12p;
            T121s = 1.0 - R12s;

            // Optical path difference
            float D = 2.0 * eta2 * d * cosTheta2;
            vec3 Dphi = vec3(2.0 * M_PI * D / 580.0, 2.0 * M_PI * D / 550.0, 2.0 * M_PI * D / 450.0);

            float phi21p, phi21s;
            vec3 phi23p, phi23s, r123s, r123p;

            // Evaluate the phase shift
            fresnel_dielectric_phase_polarized(cosTheta, eta1, eta2, phi21p, phi21s);
            if (use_schlick)
            {
                phi23p = vec3(
                    (eta3[0] < eta2) ? M_PI : 0.0,
                    (eta3[1] < eta2) ? M_PI : 0.0,
                    (eta3[2] < eta2) ? M_PI : 0.0);
                phi23s = phi23p;
            }
            else
            {
                fresnel_conductor_phase_polarized(cosTheta2, eta2, eta3, kappa3, phi23p, phi23s);
            }

            phi21p = M_PI - phi21p;
            phi21s = M_PI - phi21s;

            r123p = max(vec3(0.0), sqrt(R12p * R23p));
            r123s = max(vec3(0.0), sqrt(R12s * R23s));

            // Evaluate iridescence term
            vec3 I = vec3(0.0);
            vec3 C0, Cm, Sm;

            // Iridescence term using spectral antialiasing for Parallel polarization

            vec3 S0 = vec3(1.0);

            // Reflectance term for m=0 (DC term amplitude)
            vec3 Rs = (T121p * T121p * R23p) / (vec3(1.0) - R12p * R23p);
            C0 = R12p + Rs;
            I += C0 * S0;

            // Reflectance term for m>0 (pairs of diracs)
            Cm = Rs - T121p;
            for (int m = 1; m <= 2; ++m)
            {
                Cm *= r123p;
                Sm = 2.0f * eval_sensitivity(float(m) * D, float(m) * (phi23p + vec3(phi21p)));
                I += Cm * Sm;
            }

            // Iridescence term using spectral antialiasing for Perpendicular polarization

            // Reflectance term for m=0 (DC term amplitude)
            vec3 Rp = (T121s * T121s * R23s) / (vec3(1.0) - R12s * R23s);
            C0 = R12s + Rp;
            I += C0 * S0;

            // Reflectance term for m>0 (pairs of diracs)
            Cm = Rp - T121s;
            for (int m = 1; m <= 2; ++m)
            {
                Cm *= r123s;
                Sm = 2.0f * eval_sensitivity(float(m) * D, float(m) * (phi23s + vec3(phi21s)));
                I += Cm * Sm;
            }

            // Average parallel and perpendicular polarization
            I *= 0.5;

            vec3 RGB;
            RGB.r = 3.2410 * I.x - 1.5274 * I.y - 0.4986 * I.z;
            RGB.g = -0.9692 * I.x + 1.8760 * I.y + 0.0416 * I.z;
            RGB.b = 0.0556 * I.x - 0.2040 * I.y + 1.0507 * I.z;
            return RGB;
        }

        vec3 sampleBSDF(const vec3 &wo, vec3 &wi, float &pdf, std::shared_ptr<MR::Sampler> &sampler) const override
        {
            vec3 wr = MR_Math::reflect(wo, vec3(0, 1, 0));
            wi = wr;

            pdf = 1.0;

            vec3 F = fresnel_airy(wo.y, _ior, _extinction, _tf_thickness, _tf_ior, _F0, _F90, _expornent, false);

            return F / wi.y;
        };

        vec3 evaluateBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);

            float D = ggx_D(wm);
            float G = smith_G2(wo, wi, wm);
            vec3 F = fresnel_airy(dot(wo, wm), _ior, _extinction, _tf_thickness, _tf_ior, _F0, _F90, _expornent, false);

            float in = BSDFMath::cosTheta(wi);
            float on = BSDFMath::cosTheta(wo);

            return F * G * D / (4.0f * in * on);
        };

        float pdfBSDF(const vec3 &wo, const vec3 &wi) const override
        {
            vec3 wm = normalize(wo + wi);
            return ggx_D(wm) * BSDFMath::cosTheta(wm) / (4.0f * dot(wi, wm));
        };
    };
}
