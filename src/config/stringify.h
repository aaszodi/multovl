#ifndef CONFIG_STRINGIFY_HEADER
#define CONFIG_STRINGIFY_HEADER

/* == HEADER stringify.h == */

/**
\file stringify.h
Macros to convert preprocessor definitions to C string constants.
\date 2015-01-03
\author Andras Aszodi
*/

#define CONFIG_STRINGIFY_MACROS
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#endif  /* CONFIG_STRINGIFY_HEADER */
