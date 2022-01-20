#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#ifdef __APPLE__
#include <TargetConditionals.h> // needed for detecting if on mac or on ios
#endif

#if __cplusplus >= 201703L
#define DETECT_INLINE inline
#else
#define DETECT_INLINE
#endif

#if (defined(_M_AMD64) || defined(_M_X64) || defined(__amd64)) && !defined(__x86_64__)
#define __x86_64__ 1
#endif

namespace detect
{
        namespace detail
        {
                struct tag_type_t
                {
                        tag_type_t() = delete;
                        ~tag_type_t() = delete;

                        tag_type_t(tag_type_t const &) = delete;
                        tag_type_t &operator=(tag_type_t const &) = delete;

                        tag_type_t(tag_type_t &&) = delete;
                        tag_type_t &operator=(tag_type_t &&) = delete;
                };
        } // namespace detail

        //////////////////////
        // os-related stuff //
        //////////////////////

        struct windows_t final : detail::tag_type_t
        {
        };

        struct linux_t final : detail::tag_type_t
        {
        };

        struct macos_t final : detail::tag_type_t
        {
        };

        struct ios_t final : detail::tag_type_t
        {
        };

        struct android_t final : detail::tag_type_t
        {
        };

        struct unix_t final : detail::tag_type_t
        {
        };

        template <typename O>
        struct is_os;

        template <>
        struct is_os<windows_t>
        {
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_os<linux_t>
        {
#if defined(__linux__) && !defined(__android__)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_os<macos_t>
        {
#ifdef TARGET_OS_MAC
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_os<ios_t>
        {
#if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_os<android_t>
        {
#ifdef __android__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_os<unix_t>
        {
#if !defined(__linux__) && defined(__unix__)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <typename O>
        DETECT_INLINE static constexpr bool is_os_v = is_os<O>::value;

        DETECT_INLINE static constexpr bool is_windows_v = is_os<windows_t>::value;
        DETECT_INLINE static constexpr bool is_linux_v = is_os<linux_t>::value;
        DETECT_INLINE static constexpr bool is_macos_v = is_os<macos_t>::value;
        DETECT_INLINE static constexpr bool is_ios_v = is_os<ios_t>::value;
        DETECT_INLINE static constexpr bool is_android_v = is_os<android_t>::value;
        DETECT_INLINE static constexpr bool is_unix_v = is_os<unix_t>::value;

        ////////////////////////////
        // compiler-related stuff //
        ////////////////////////////

        struct gcc_t final : detail::tag_type_t
        {
        };

        struct clang_t final : detail::tag_type_t
        {
        };

        struct msvc_t final : detail::tag_type_t
        {
        };

        template <typename C>
        struct is_compiler;

        template <>
        struct is_compiler<gcc_t>
        {
#if defined(__GNUC__) && !defined(__clang__)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_compiler<clang_t>
        {
#ifdef __clang__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_compiler<msvc_t>
        {
#if defined(_MSC_VER) && !defined(__clang__)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <typename C>
        DETECT_INLINE static constexpr bool is_compiler_v = is_compiler<C>::value;

        DETECT_INLINE static constexpr bool is_gcc_v = is_compiler<gcc_t>::value;
        DETECT_INLINE static constexpr bool is_clang_v = is_compiler<clang_t>::value;
        DETECT_INLINE static constexpr bool is_msvc_v = is_compiler<msvc_t>::value;

        ////////////////////////////////
        // C++ standard-related stuff //
        ////////////////////////////////

        struct cxx_11 final : detail::tag_type_t
        {
        };

        struct cxx_14 final : detail::tag_type_t
        {
        };

        struct cxx_17 final : detail::tag_type_t
        {
        };

        struct cxx_20 final : detail::tag_type_t
        {
        };

        template <typename T>
        struct is_std;

        template <>
        struct is_std<cxx_11>
        {
#if __cplusplus == 201103L
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_std<cxx_14>
        {
#if __cplusplus == 201402L
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_std<cxx_17>
        {
#if __cplusplus == 201703L
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_std<cxx_20>
        {
#if __cplusplus == 202002L
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <typename T>
        DETECT_INLINE static constexpr bool is_std_v = is_std<T>::value;

        DETECT_INLINE static constexpr bool is_cxx_11 = is_std<cxx_11>::value;
        DETECT_INLINE static constexpr bool is_cxx_14 = is_std<cxx_14>::value;
        DETECT_INLINE static constexpr bool is_cxx_17 = is_std<cxx_17>::value;
        DETECT_INLINE static constexpr bool is_cxx_20 = is_std<cxx_20>::value;

        /////////////////////////////////
        // debug/release related stuff //
        /////////////////////////////////

        struct debug_t final : detail::tag_type_t
        {
        };

        struct release_t final : detail::tag_type_t
        {
        };

        template <typename T>
        struct is_build;

        template <>
        struct is_build<debug_t>
        {
#if defined(_DEBUG) || defined(DEBUG)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_build<release_t>
        {
#if defined(NDEBUG) || !(defined(DEBUG) && defined(_DEBUG))
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <typename T>
        DETECT_INLINE static constexpr bool is_build_v = is_build<T>::value;

        DETECT_INLINE static constexpr bool is_debug_v = is_build<debug_t>::value;
        DETECT_INLINE static constexpr bool is_release_v = is_build<release_t>::value;

        ///////////////////////
        // simd instructions //
        ///////////////////////

        struct x86sse_t final : detail::tag_type_t
        {
        };

        struct x86sse2_t final : detail::tag_type_t
        {
        };

        struct sse_t final : detail::tag_type_t
        {
        };

        struct sse2_t final : detail::tag_type_t
        {
        };

        struct sse3_t final : detail::tag_type_t
        {
        };

        struct ssse3_t final : detail::tag_type_t
        {
        };

        struct sse4_1_t final : detail::tag_type_t
        {
        };

        struct sse4_2_t final : detail::tag_type_t
        {
        };

        struct avx_t final : detail::tag_type_t
        {
        };

        struct avx2_t final : detail::tag_type_t
        {
        };

        template <typename S>
        struct is_simd;

        template <>
        struct is_simd<x86sse_t>
        {
#if defined(_M_IX86_FP) && (_M_IX86_FP == 1)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<x86sse2_t>
        {
#if defined(_M_IX86_FP) && (_M_IX86_FP == 2)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<sse_t>
        {
#ifdef __SSE__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<sse2_t>
        {
#if defined(__SSE2__) || defined(__x86_x64__)
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<sse3_t>
        {
#ifdef __SSE3__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<ssse3_t>
        {
#ifdef __SSSE3__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<sse4_1_t>
        {
#ifdef __SSE4_1__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<sse4_2_t>
        {
#ifdef __SSE4_2__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<avx_t>
        {
#ifdef __AVX__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <>
        struct is_simd<avx2_t>
        {
#ifdef __AVX2__
                DETECT_INLINE static constexpr bool value = true;
#else
                DETECT_INLINE static constexpr bool value = false;
#endif
        };

        template <typename S>
        DETECT_INLINE constexpr bool is_simd_v = is_simd<S>::value;

        DETECT_INLINE constexpr bool is_x86_sse_t = is_simd<x86sse_t>::value;
        DETECT_INLINE constexpr bool is_x86_sse2_t = is_simd<x86sse2_t>::value;
        DETECT_INLINE constexpr bool is_sse_t = is_simd<sse_t>::value;
        DETECT_INLINE constexpr bool is_sse2_t = is_simd<sse2_t>::value;
        DETECT_INLINE constexpr bool is_sse3_t = is_simd<sse3_t>::value;
        DETECT_INLINE constexpr bool is_ssse3_t = is_simd<ssse3_t>::value;
        DETECT_INLINE constexpr bool is_sse4_1_t = is_simd<sse4_1_t>::value;
        DETECT_INLINE constexpr bool is_sse4_2_t = is_simd<sse4_2_t>::value;
        DETECT_INLINE constexpr bool is_avx_t = is_simd<avx_t>::value;
        DETECT_INLINE constexpr bool is_avx2_t = is_simd<avx2_t>::value;
} // namespace detect

#ifdef __x86_64__
#undef __x86_64__
#endif

#endif // !DETECTOR_HPP
