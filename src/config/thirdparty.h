#ifndef CONFIG_THIRDPARTY_HEADER
#define CONFIG_THIRDPARTY_HEADER

/* == HEADER thirdparty.h == */

/**
\file thirdparty.h
Macros and functions describing third-party libraries.
\date 2015-01-03
\author Andras Aszodi
*/

/* -- Prototypes -- */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \return A C string "Boost version X.Y.Z"
 */
const char* config_boost_version();

/**
 * \return A C string "BAM file input supported" or "No BAM file support"
 */
const char* config_bamtools();

/**
 * \return 1 if BAM input is supported, 0 if not.
 */
int config_have_bamtools();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // CONFIG_THIRDPARTY_HEADER

