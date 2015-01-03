#ifndef CONFIG_BUILD_HEADER
#define CONFIG_BUILD_HEADER

/* == HEADER build.h == */

/**
\file build.h
Macros and functions describing the build: platform, compiler version.
\date 2015-01-03
\author Andras Aszodi
*/

/* -- Prototypes -- */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \return A C string describing the build type: "Debug" or "Release".
 */
const char* config_build_type();

/**
 * \return A C string specifying the name and version of the C++ compiler.
 */
const char* config_build_compiler();

/**
 * \return A C string describing the system: OS, version and processor architecture.
 */
const char* config_build_system();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* CONFIG_BUILD_HEADER */

