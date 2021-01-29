// copyright (c) 2013 Victor Cherepanov
// distributed under BSD 3-clause license

#pragma once

#if !defined(__DIMANCHE_SYSTEM_CPU_OPT_H)
#define __DIMANCHE_SYSTEM_CPU_OPT_H

#include <dimanche/system/cpu.h>

#define DECL_PROC_TYPES(return_type, function, formal_args) \
    namespace function##_namespace \
    { \
        /* declare function type */ \
        using cpu_t = dimanche::system::cpu::eCpuType; \
        using return_t = return_type; \
        typedef return_t (__CDECL function_t) formal_args; \
        /* declare implementation table type */ \
        struct impl_table_t \
        { \
            /* required CPU features for a particular optimization */ \
            cpu_t requiredCpu; \
            /* pointer to an optimized function */ \
            function_t *pFunction; \
        }; \
        /* declare a function to initialize the pointer */ \
        template <typename impl, function_t *(table[]), const size_t idx, typename ... params_t> \
        return_t __CDECL init_pointer (params_t ... params) { \
            const cpu_t currentCpu = dimanche::system::cpu::GetType(); \
            /* find the most appropriate optimized implementation */ \
            for (ptrdiff_t implIdx = impl::size - 1; implIdx >= 0; --implIdx) { \
                const cpu_t requiredCpu = impl::table[implIdx].requiredCpu; \
                if (requiredCpu == (requiredCpu & currentCpu)) { \
                    table[idx]= impl::table[implIdx].pFunction; \
                    break; \
                } \
            } \
            return (table[idx]) (params...); \
        } \
    } /* namespace function##_namespace */ \


#define DECL_PROC(return_type, function, formal_args, actual_args) \
    /* declare function types */ \
    DECL_PROC_TYPES(return_type, function, formal_args) \
    namespace function##_namespace \
    { \
        /* declare the implementation table */ \
        class impl \
        { \
        public: \
            static \
            const struct impl_table_t table[]; \
            static \
            const size_t size; \
        }; \
        /* declare the function pointer */ \
        extern \
        function_t *p; \
    } /* namespace function##_namespace */ \
    inline \
    function##_namespace::return_t function formal_args { \
        return (*function##_namespace::p) actual_args; \
    } \


#define IMPL_PROC_0(function) \
    /* declare optimized functions */ \
    extern "C" \
    { \
        function##_namespace::function_t function##_c; \
    } \
    /* implement function's stuff */ \
    namespace function##_namespace \
    { \
        /* implement the implementation table */ \
        const struct impl_table_t impl::table[] = { \
            {cpu_t::plain, &function##_c} \
        }; \
        /* implement the implementation table size */ \
        const size_t impl::size = sizeof(impl::table) / sizeof(impl::table[0]); \
        /* implement the function pointer */ \
        function_t *p = init_pointer<impl, &p, 0>; \
    } /* namespace function_name##_disp */


#define IMPL_PROC_1(function, type0) \
    /* declare optimized functions */ \
    extern "C" \
    { \
        function##_namespace::function_t function##_c; \
        function##_namespace::function_t function##_##type0##_asm; \
    } \
    /* implement function's stuff */ \
    namespace function##_namespace \
    { \
        /* implement the implementation table */ \
        const struct impl_table_t impl::table[] = { \
            {cpu_t::plain, &function##_c}, \
            {cpu_t::type0, &function##_##type0##_asm}, \
        }; \
        /* implement the implementation table size */ \
        const size_t impl::size = sizeof(impl::table) / sizeof(impl::table[0]); \
        /* implement the function pointer */ \
        function_t *p = init_pointer<impl, &p, 0>; \
    } /* namespace function_name##_disp */


#define IMPL_PROC_2(function, type0, type1) \
    /* declare optimized functions */ \
    extern "C" \
    { \
        function##_namespace::function_t function##_c; \
        function##_namespace::function_t function##_##type0##_asm; \
        function##_namespace::function_t function##_##type1##_asm; \
    } \
    /* implement function's stuff */ \
    namespace function##_namespace \
    { \
        /* implement the implementation table */ \
        const struct impl_table_t impl::table[] = { \
            {cpu_t::plain, &function##_c}, \
            {cpu_t::type0, &function##_##type0##_asm}, \
            {cpu_t::type1, &function##_##type1##_asm}, \
        }; \
        /* implement the implementation table size */ \
        const size_t impl::size = sizeof(impl::table) / sizeof(impl::table[0]); \
        /* implement the function pointer */ \
        function_t *p = init_pointer<impl, &p, 0>; \
    } /* namespace function_name##_disp */


#define IMPL_PROC_3(function, type0, type1, type2) \
    /* declare optimized functions */ \
    extern "C" \
    { \
        function##_namespace::function_t function##_c; \
        function##_namespace::function_t function##_##type0##_asm; \
        function##_namespace::function_t function##_##type1##_asm; \
        function##_namespace::function_t function##_##type2##_asm; \
    } \
    /* implement function's stuff */ \
    namespace function##_namespace \
    { \
        /* implement the implementation table */ \
        const struct impl_table_t impl::table[] = { \
            {cpu_t::plain, &function##_c}, \
            {cpu_t::type0, &function##_##type0##_asm}, \
            {cpu_t::type1, &function##_##type1##_asm}, \
            {cpu_t::type2, &function##_##type2##_asm}, \
        }; \
        /* implement the implementation table size */ \
        const size_t impl::size = sizeof(impl::table) / sizeof(impl::table[0]); \
        /* implement the function pointer */ \
        function_t *p = init_pointer<impl, &p, 0>; \
    } /* namespace function_name##_disp */

#endif // !defined(__DIMANCHE_SYSTEM_CPU_OPT_H)
