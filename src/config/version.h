#ifndef CONFIG_VERSION_HEADER
#define CONFIG_VERSION_HEADER

/* == HEADER version.h == */

/**
\file version.h
Macros and functions describing the version, incl. Git revision data if available.
\date 2015-01-03
\author Andras Aszodi
*/

/* -- Prototypes -- */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \return A C version string like "1.2"
 */
const char* config_version();

/**
 * \return A C string containing the full Git revision SHA1 checksum or "(unknown)".
 */
const char* config_gitrevision();

/**
 * \return A C string containing the short Git revision SHA1 checksum (7 chars) or "(unknown)".
 */
const char* config_gitrev();

/**
 * \return A C string containing the Git revision date like "2015-01-02 22:32:52 +0100".
 * If Git was not available, then the CMake configuration timestamp is returned.
 */
const char* config_gitrevdate();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* CONFIG_VERSION_HEADER */

