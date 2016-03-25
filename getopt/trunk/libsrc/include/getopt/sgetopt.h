#ifndef __sgetopt_h_20160325_
#define __sgetopt_h_20160325_

#ifdef __cplusplus
extern"C" {
#endif

// Note: user of this header should #include <ps_TCHAR.h> first to get TCHAR definition.

typedef struct _sgetopt_info
{
	TCHAR *optarg; // =NULL
	/* For communication from `getopt' to the caller.
	   When `getopt' finds an option that takes an argument,
	   the argument value is returned here.
	   Also, when `ordering' is RETURN_IN_ORDER,
	   each non-option ARGV-element is returned here.  */

	int optind; // =0
	/* Index in ARGV of the next element to be scanned.
	   This is used for communication to and from the caller
	   and for communication between successive calls to `getopt'.

	   On entry to `getopt', zero means this is the first call; initialize.

	   When `getopt' returns EOF, this is the index of the first of the
	   non-option elements that the caller should itself scan.

	   Otherwise, `optind' communicates from one call to the next
	   how much of ARGV has been scanned so far.  */
	/* XXX 1003.2 says this must be 1 before any call. (??) */

	
	char *nextchar; 
	/* The next char to be scanned in the option-element
	   in which the last option character we returned was found.
	   This allows us to pick up the scan where we left off.

	   If this is zero, or a null string, it means resume the scan
	   by advancing to the next ARGV-element.  */

	int opterr; // = 1;
	/* Callers store zero here to inhibit the error message
	   for unrecognized options.  */


	int optopt; // = '?';
	/* Set to an option character which was unrecognized.
	   This must be initialized on some systems to avoid linking in the
	   system's own getopt implementation.  */

	int ireserve1, ireserve2, ireserve3, ireserve4;
	void *reserve1, *reserve2, *reserve3, *reserve4;
		// For future extension ABI compatibility.
	
	TCHAR errmsg[100];
		// used to return error message to user.
	
	// Internal members starts from here:
	
	int first_nonopt; // internal
	int last_nonopt;  // internal
		/* Handle permutation of arguments.
		   Describe the part of ARGV that contains non-options that have
		   been skipped.  `first_nonopt' is the index in ARGV of the first of them;
		   `last_nonopt' is the index after the last of them.  */

} sgetopt_info;



#ifdef __cplusplus
} // extern"C" {
#endif

#endif


