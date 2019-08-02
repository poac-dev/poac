#ifndef POAC_UTIL_TARGET_HPP
#define POAC_UTIL_TARGET_HPP

#include <boost/predef.h>

namespace poac::util::target {
    enum class arch {
        alpha,
        arm,
        blackfin,
        convex,
        ia64,
        m68k,
        mips,
        s390x,
        x86,
        x86_32,
        x86_64,
    };

    enum class feature {
        aes,
        avx,
        avx2,
        bmi1,
        bmi2,
        fma,
        fxsr,
        lzcnt,
        pclmulqdq,
        popcnt,
        rdrand,
        rdseed,
        sha,
        sse,
        sse2,
        sse3,
        sse4_1,
        sse4_2,
        ssse3,
        xsave,
        xsavec,
        xsaveopt,
        xsaves,
    };

    enum class os {
        windows,
        macos,
        ios,
        linux,
        android,
        freebsd,
        dragonfly,
        bitrig,
        openbsd,
        netbsd,
    };

    enum class family {
        windows,
        unix,
    };

    enum class compiler {
        gcc,
        clang,
        apple_clang,
        msvc,
        icc,
    };

    enum class platform {
        mingw,
    };

} // end namespace poac::util

#endif // !POAC_UTIL_TARGET_HPP
