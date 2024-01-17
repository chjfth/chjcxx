#ifndef __sgetopt_h_20160325_
#define __sgetopt_h_20160325_

#ifdef __cplusplus
extern"C" {
#endif

#include "need_TCHAR.h"


// Portable getopt and getopt_long, getopt_long_only .
// Doc at http://linux.die.net/man/3/getopt_long

// Note: user of this header should #include <ps_TCHAR.h> first to get TCHAR definition.

#ifndef DLLEXPORT_sgetopt
#define DLLEXPORT_sgetopt
#endif

typedef struct _sgetopt_ctx
{
	unsigned int signature;
	
	TCHAR *optarg; // [output]
	/* For communication from `getopt' to the caller.
	   When `getopt' finds an option that takes an argument,
	   the argument value is returned here.
	   Also, when `ordering' is RETURN_IN_ORDER,
	   each non-option ARGV-element is returned here.  */

	int optind; // [output]
	/* Index in ARGV of the next element to be scanned.
	   This is used for communication to and from the caller
	   and for communication between successive calls to `getopt'.

	   On entry to `getopt', zero means this is the first call; initialize.

	   When `getopt' returns EOF, this is the index of the first of the
	   non-option elements that the caller should itself scan.

	   Otherwise, `optind' communicates from one call to the next
	   how much of ARGV has been scanned so far.  */
	
	int opterr; // [input]
	/* Callers store zero here to inhibit the error message
	   for unrecognized options.  */

	int optopt; // = '?';
	/* Set to an option character which was unrecognized.
	   This must be initialized on some systems to avoid linking in the
	   system's own getopt implementation. (??) */

	int ireserve1, ireserve2, ireserve3, ireserve4;
	void *reserve1, *reserve2, *reserve3, *reserve4;
		// For future extension ABI compatibility.
	
	TCHAR errmsg[100];
		// used to return error message to user.

	//
	// Internal members starts from here:
	//
	
	TCHAR *nextchar; 
	/* The next char to be scanned in the option-element
	   in which the last option character we returned was found.
	   This allows us to pick up the scan where we left off.

	   If this is zero, or a null string, it means resume the scan
	   by advancing to the next ARGV-element.  */

	int first_nonopt; // internal
	int last_nonopt;  // internal
		/* Handle permutation of arguments.
		   Describe the part of ARGV that contains non-options that have
		   been skipped.  `first_nonopt' is the index in ARGV of the first of them;
		   `last_nonopt' is the index after the last of them.  */

} sgetopt_ctx;

enum { has_arg_no=0, has_arg_yes=1 };

typedef struct _sgetopt_option {
	const TCHAR *name;
	int         has_arg;	// [input] has_arg_no or has_arg_yes, =2 not supported
	int        *flag;
	int         opt_id;
	const void *user_context;
} sgetopt_option;


enum sgetopt_err_et { sgetopt_ok=0, sgetopt_fail=-1 };

DLLEXPORT_sgetopt
sgetopt_ctx *sgetopt_ctx_create();

DLLEXPORT_sgetopt
sgetopt_err_et sgetopt_ctx_delete(sgetopt_ctx *si);

DLLEXPORT_sgetopt
int	sgetopt(sgetopt_ctx *si, int argc, TCHAR *const argv[], const TCHAR *optstring);

DLLEXPORT_sgetopt
int sgetopt_long(sgetopt_ctx *si, 
	int argc, TCHAR * const argv[], const TCHAR *optstring,
	const sgetopt_option *longopts, int *longindex); // preferred

DLLEXPORT_sgetopt
int sgetopt_long_only(sgetopt_ctx *si, 
	int argc, TCHAR * const argv[], const TCHAR *optstring,
	const sgetopt_option *longopts, int *longindex);

//

DLLEXPORT_sgetopt
bool sgetopt_is_valid_short(TCHAR short_opt, const TCHAR *short_options);

DLLEXPORT_sgetopt
bool sgetopt_is_valid_long(const TCHAR *long_opt, const sgetopt_option long_options[]);

#ifdef __cplusplus
} // extern"C" {
#endif

#endif


