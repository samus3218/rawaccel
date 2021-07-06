#pragma once

#include "accel-classic.hpp"
#include "accel-jump.hpp"
#include "accel-natural.hpp"
#include "accel-power.hpp"
#include "accel-motivity.hpp"
#include "accel-noaccel.hpp"

namespace rawaccel {

    enum class internal_mode {
        classic_lgcy,
        classic_gain,
        jump_lgcy,
        jump_gain,
        natural_lgcy,
        natural_gain,
        motivity_lgcy,
        motivity_gain,
        power,
        lut_arb,
        lut_log,
        lut_lin,
        noaccel
    };

    constexpr internal_mode make_mode(accel_mode mode, spaced_lut_mode lut_mode, bool legacy) 
    {
        if (lut_mode != spaced_lut_mode::off) {
            switch (lut_mode) {
            case spaced_lut_mode::binlog: return internal_mode::lut_log;
            case spaced_lut_mode::linear: return internal_mode::lut_lin;
            default: return internal_mode::noaccel;
            }
        }
        else if (mode == accel_mode::power) {
            return internal_mode::power;
        }
        else if (mode == accel_mode::arb_lookup) {
            return internal_mode::lut_arb;
        }
        else if (mode >= accel_mode::noaccel) {
            return internal_mode::noaccel;
        }
        else {
            int im = static_cast<int>(mode) * 2 + (legacy ? 0 : 1);
            return static_cast<internal_mode>(im);
        }
    }

    constexpr internal_mode make_mode(const accel_args& args) 
    {
        return make_mode(args.mode, args.spaced_args.mode, args.legacy);
    }

    template <typename Visitor, typename AccelUnion>
    constexpr auto visit_accel(Visitor vis, internal_mode mode, AccelUnion&& u)
    {
        switch (mode) {
        case internal_mode::classic_lgcy:  return vis(u.classic_l);
        case internal_mode::classic_gain:  return vis(u.classic_g);
        case internal_mode::jump_lgcy:     return vis(u.jump_l);
        case internal_mode::jump_gain:     return vis(u.jump_g);
        case internal_mode::natural_lgcy:  return vis(u.natural_l);
        case internal_mode::natural_gain:  return vis(u.natural_g);
        case internal_mode::motivity_lgcy: return vis(u.motivity_l);
        case internal_mode::motivity_gain: return vis(u.motivity_g);
        case internal_mode::power:         return vis(u.power);
        case internal_mode::lut_arb:       return vis(u.arb_lut);
        case internal_mode::lut_log:       return vis(u.log_lut);
        case internal_mode::lut_lin:       return vis(u.lin_lut);
        default:                           return vis(u.noaccel);
        }
    }

    union accel_union {
        classic classic_g;
        classic_legacy classic_l;
        jump jump_g;
        jump_legacy jump_l;
        natural natural_g;
        natural_legacy natural_l;
        power power;
        sigmoid motivity_l;
        motivity motivity_g;
        linear_lut lin_lut;
        binlog_lut log_lut;
        arbitrary_lut arb_lut;
        accel_noaccel noaccel = {};

        accel_union(const accel_args& args)
        {
            visit_accel([&](auto& impl) {
                impl = { args };
            }, make_mode(args), *this);
        }

        accel_union() = default;
    };

}