/*
Getopt replacement for Windows.
Downloaded from http://www.rohitab.com/discuss/topic/39420-a-proper-port-of-getopt/
Slightly modified by AA.
*/

#ifndef WGETOPT_H
#define WGETOPT_H

#ifdef _WIN32

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern char *optarg;
extern int optind, opterr, optopt, optreset;

int getopt(int nargc, char * const nargv[], const char *ostr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _WIN32 */
#endif /* !WGETOPT_H */
