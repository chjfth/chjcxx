#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <getopt/sgetopt.h>
#include <mm_snprintf.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void sgetopt_lib__sgetopt__DLL_AUTO_EXPORT_STUB(void){}

// [2016-03-25] Chj: Copied and modified from:
// http://www.opensource.apple.com/source/bc/bc-18/bc/lib/getopt.c?txt
// Chj's major change: 
// * Make all getopt operation into a sgetopt_info object.
// * Error is not fprintf to stderr, but saved to sgetopt_info.
// * Make the library stramphibian(i.e. TCHAR operation£©
// You can imagine the _T('s') prefix as "super".
//
// Usage example: 
// * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
// * http://linux.die.net/man/3/getopt_long


/* This version of `getopt' appears to the caller like standard Unix `getopt'
   but it behaves differently for the user, since it allows the user
   to intersperse the options with the other arguments.

   As `getopt' works, it permutes the elements of ARGV so that,
   when it is done, all the options precede everything else.  Thus
   all application programs are extended to handle flexible argument order.

   Setting the environment variable POSIXLY_CORRECT disables permutation.
   Then the behavior is completely standard. // chj: POSIXLY_CORRECT is disabled in this cpp.

   GNU application programs can use a third alternative mode in which
   they can distinguish the relative order of options and other arguments.  */


/* Describe how to deal with options that follow non-option ARGV-elements.

   If the caller did not specify anything,
   [ the default is REQUIRE_ORDER if the environment variable
   POSIXLY_CORRECT is defined,] PERMUTE otherwise.

   REQUIRE_ORDER means don't recognize them as options;
   stop option processing when the first non-option is seen.
   This is what Unix does.
   This mode of operation is selected by either setting the environment
   variable POSIXLY_CORRECT, or using `+' as the first character
   of the list of option characters.

   PERMUTE is the default.  We permute the contents of ARGV as we scan,
   so that eventually all the non-options are at the end.  This allows options
   to be given in any order, even with programs that were not written to
   expect this.

   RETURN_IN_ORDER is an option available to programs that were written
   to expect options and other ARGV-elements in any order and that care about
   the ordering of the two.  We describe each non-option ARGV-element
   as if it were the argument of *an option with character code 1* .
   Using `-' as the first character of the list of option characters
   selects this mode of operation. (??)

   The special argument `--' forces an end of option-scanning regardless
   of the value of `ordering'.  In the case of RETURN_IN_ORDER, only
   `--' can cause `getopt' to return EOF with `si->optind' != ARGC.  */

static enum
{
  REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;

#define SIGNATURE 0x20160325 

/* Value of POSIXLY_CORRECT environment variable.  */
static TCHAR *posixly_correct = NULL;
	// let it default to PERMUTE. 

/* Exchange two adjacent subsequences of ARGV.
   One subsequence is elements [si->first_nonopt,si->last_nonopt)
   which contains all the non-options that have been skipped so far.
   The other is elements [si->last_nonopt,si->optind), which contains all
   the options processed since those non-options were skipped.

   `si->first_nonopt' and `si->last_nonopt' are relocated so that they describe
   the new indices of the non-options in ARGV after they are moved.  */
static void
exchange(sgetopt_ctx *si, TCHAR **argv)
{
	int bottom = si->first_nonopt;
	int middle = si->last_nonopt;
	int top = si->optind;
	TCHAR *tem;

	/* Exchange the shorter segment with the far end of the longer segment.
	That puts the shorter segment into the right place.
	It leaves the longer segment in the right place overall,
	but it consists of two parts that need to be swapped next.  */

	while (top > middle && middle > bottom)
	{
		if (top - middle > middle - bottom)
		{
			/* Bottom segment is the short one.  */
			int len = middle - bottom;
			register int i;

			/* Swap it with the top part of the top segment.  */
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[top - (middle - bottom) + i];
				argv[top - (middle - bottom) + i] = tem;
			}
			/* Exclude the moved bottom segment from further swapping.  */
			top -= len;
		}
		else
		{
			/* Top segment is the short one.  */
			int len = top - middle;
			register int i;

			/* Swap it with the bottom part of the bottom segment.  */
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[middle + i];
				argv[middle + i] = tem;
			}
			/* Exclude the moved top segment from further swapping.  */
			bottom += len;
		}
	}

	/* Update records for the slots the non-options now occupy.  */

	si->first_nonopt += (si->optind - si->last_nonopt);
	si->last_nonopt = si->optind;
}


/* Initialize the internal data when the first call is made.  */
static const TCHAR *
_sgetopt_initialize (sgetopt_ctx *si, const TCHAR *optstring)
{
	/* Start processing options with ARGV-element 1 (since ARGV-element 0
	is the program name); the sequence of previously skipped
	non-option ARGV-elements is empty.  */

	si->first_nonopt = si->last_nonopt = si->optind = 1;

	si->nextchar = NULL;

	//  posixly_correct = getenv ("POSIXLY_CORRECT");

	/* Determine how to handle the ordering of options and non-options.  */

	if (optstring[0] == _T('-'))
	{
		ordering = RETURN_IN_ORDER;
		++optstring;
	}
	else if (optstring[0] == _T('+'))
	{
		ordering = REQUIRE_ORDER;
		++optstring;
	}
	else if (posixly_correct != NULL)
		ordering = REQUIRE_ORDER;
	else
		ordering = PERMUTE;

	return optstring;
}


/* Scan elements of ARGV (whose length is ARGC) for option characters
	given in OPTSTRING.

	If an element of ARGV starts with '-', and is not exactly "-" or "--",
	then it is an option element.  The characters of this element
	(aside from the initial '-') are option characters.  If `getopt'
	is called repeatedly, it returns successively each of the option characters
	from each of the option elements.

	If `getopt' finds another option character, it returns that character,
	updating `si->optind' and `si->nextchar' so that the next call to `getopt' can
	resume the scan with the following option character or ARGV-element.

	If there are no more option characters, `getopt' returns `EOF'.
	Then `si->optind' is the index in ARGV of the first ARGV-element
	that is not an option.  (The ARGV-elements have been permuted
	so that those that are not options now come last.)

	OPTSTRING is a string containing the legitimate option characters.
	If an option character is seen that is not listed in OPTSTRING,
	return '?' after printing an error message.  If you set `si->opterr' to
	zero, the error message is suppressed but we still return '?'.

	If a TCHAR in OPTSTRING is followed by a colon, that means it wants an arg,
	so the following text in the same ARGV-element, or the text of the following
	ARGV-element, is returned in `si->optarg'.  Two colons mean an option that
	wants an optional arg; if there is text in the current ARGV-element,
	it is returned in `si->optarg', otherwise `si->optarg' is set to zero.

	If OPTSTRING starts with `-' or `+', it requests different methods of
	handling the non-option ARGV-elements.
	See the comments about RETURN_IN_ORDER and REQUIRE_ORDER, above.

	Long-named options begin with `--' instead of `-'.
	Their names may be abbreviated as long as the abbreviation is unique
	or is an exact match for some defined option.  If they have an
	argument, it follows the option name in the same ARGV-element, separated
	from the option name by a `=', or else the in next ARGV-element.
	When `getopt' finds a long-named option, it returns 0 if that option's
	`flag' field is nonzero, the value of the option's `val' field
	if the `flag' field is zero.

	The elements of ARGV aren't really const, because we permute them.
	But we pretend they're const in the prototype to be compatible
	with other systems.

	LONGOPTS is a vector of `struct option' terminated by an
	element containing a name which is zero.

	LONGIND returns the index in LONGOPT of the long-named option found.
	It is only valid when a long-named option has been found by the most
	recent call.

	If LONG_ONLY is nonzero, _T('-') as well as '--' can introduce
	long-named options.  
*/
int
_sgetopt_internal (
	sgetopt_ctx *si,
	int argc,
	TCHAR *const *argv,
	const TCHAR *optstring,
	const sgetopt_option *longopts,
	int *longind,
	int long_only
	)
{
	si->optarg = NULL;

	if (si->optind == 0)
		optstring = _sgetopt_initialize (si, optstring);

	if (si->nextchar == NULL || *si->nextchar == '\0')
	{
		/* Advance to the next ARGV-element.  */

		if (ordering == PERMUTE)
		{
			/* If we have just processed some options following some non-options,
			exchange them so that the options come first.  */

			if (si->first_nonopt != si->last_nonopt && si->last_nonopt != si->optind)
				exchange (si, (TCHAR **) argv);
			else if (si->last_nonopt != si->optind)
				si->first_nonopt = si->optind;

			/* Skip any additional non-options
			and extend the range of non-options previously skipped.  */

			while (si->optind < argc
				&& (argv[si->optind][0] != _T('-') || argv[si->optind][1] == '\0'))
				si->optind++;
			si->last_nonopt = si->optind;
		}

		/* The special ARGV-element `--' means premature end of options.
		Skip it like a null option,
		then exchange with previous non-options as if it were an option,
		then skip everything else like a non-option.  */

		if (si->optind != argc && !T_strcmp(argv[si->optind], _T("--")))
		{
			si->optind++;

			if (si->first_nonopt != si->last_nonopt && si->last_nonopt != si->optind)
				exchange (si, (TCHAR **) argv);
			else if (si->first_nonopt == si->last_nonopt)
				si->first_nonopt = si->optind;
			si->last_nonopt = argc;

			si->optind = argc;
		}

		/* If we have done all the ARGV-elements, stop the scan
		and back over any non-options that we skipped and permuted.  */

		if (si->optind == argc)
		{
			/* Set the next-arg-index to point at the non-options
			that we previously skipped, so the caller will digest them.  */
			if (si->first_nonopt != si->last_nonopt)
				si->optind = si->first_nonopt;
			return EOF;
		}

		/* If we have come to a non-option and did not permute it,
		either stop the scan or describe it to the caller and pass it by.  */

		if ((argv[si->optind][0] != _T('-') || argv[si->optind][1] == '\0'))
		{
			if (ordering == REQUIRE_ORDER)
				return EOF;
			si->optarg = argv[si->optind++];
			return 1;
		}

		/* We have found another option-ARGV-element.
		Skip the initial punctuation.  */

		si->nextchar = (argv[si->optind] + 1
			+ (longopts != NULL && argv[si->optind][1] == _T('-')));
	}

	/* Decode the current option-ARGV-element.  */

	/* Check whether the ARGV-element is a long option.

	If long_only and the ARGV-element has the form "-f", where f is
	a valid short option, don't consider it an abbreviated form of
	a long option that starts with f.  Otherwise there would be no
	way to give the -f short option.

	On the other hand, if there's a long option "fubar" and
	the ARGV-element is "-fu", do consider that an abbreviation of
	the long option, just like "--fu", and not "-f" with arg "u".

	This distinction seems to be the most useful approach.  */

	if (longopts != NULL
		&& (argv[si->optind][1] == _T('-')
		|| (long_only && (argv[si->optind][2] || !T_strchr (optstring, argv[si->optind][1]))))
		)
	{
		TCHAR *nameend;
		const sgetopt_option *p;
		const sgetopt_option *pfound = NULL;
		int exact = 0;
		int ambig = 0;
		int indfound;
		int option_index;

		for (nameend = si->nextchar; *nameend && *nameend != _T('='); nameend++)
			/* Do nothing.  */ ;

		/* Test all long options for either exact match
		or abbreviated matches.  */
		for (p = longopts, option_index = 0; p->name; p++, option_index++)
			if (!T_strncmp (p->name, si->nextchar, nameend - si->nextchar))
			{
				if (nameend - si->nextchar == T_strlen (p->name))
				{
					/* Exact match found.  */
					pfound = p;
					indfound = option_index;
					exact = 1;
					break;
				}
				else if (pfound == NULL)
				{
					/* First nonexact match found.  */
					pfound = p;
					indfound = option_index;
				}
				else
					/* Second or later nonexact match found.  */
					ambig = 1;
			}

			if (ambig && !exact)
			{
				if (si->opterr)
					fprintf (stderr, "%s: option `%s' is ambiguous\n",
					argv[0], argv[si->optind]);
				si->nextchar += T_strlen (si->nextchar);
				si->optind++;
				return _T('?');
			}

			if (pfound != NULL)
			{
				option_index = indfound;
				si->optind++;
				if (*nameend)
				{
					/* Don't test has_arg with >, because some C compilers don't
					allow it to be used on enums.  */
					if (pfound->has_arg)
						si->optarg = nameend + 1;
					else
					{
						if (si->opterr)
						{
							if (argv[si->optind - 1][1] == _T('-')) {
								/* --option */
								mm_snprintf(si->errmsg, sizeof(si->errmsg),
									_T("%s: option `--%s' doesn't allow an argument\n"),
									argv[0], pfound->name);
							}
							else {
								/* +option or -option */
								mm_snprintf(si->errmsg, sizeof(si->errmsg),
									_T("%s: option `%c%s' doesn't allow an argument\n"),
									argv[0], argv[si->optind - 1][0], pfound->name);
							}
						}
						si->nextchar += T_strlen (si->nextchar);
						return _T('?');
					}
				}
				else if (pfound->has_arg == 1)
				{
					if (si->optind < argc)
						si->optarg = argv[si->optind++];
					else
					{
						if (si->opterr) {
							mm_snprintf(si->errmsg, sizeof(si->errmsg),
								_T("%s: option `%s' requires an argument\n"),
								argv[0], argv[si->optind - 1]);
						}
						si->nextchar += T_strlen (si->nextchar);
						return optstring[0] == _T(':') ? _T(':') : _T('?');
					}
				}
				si->nextchar += T_strlen (si->nextchar);
				if (longind != NULL)
					*longind = option_index;
				
				if (pfound->flag)
				{
					*(pfound->flag) = pfound->val;
					return 0;
				}
				return pfound->val;
			}

			/* Can't find it as a long option.  If this is not getopt_long_only,
			or the option starts with '--' or is not a valid short
			option, then it's an error.
			Otherwise interpret it as a short option.  */
			if (!long_only || argv[si->optind][1] == _T('-')
				|| T_strchr (optstring, *si->nextchar) == NULL)
			{
				if (si->opterr)
				{
					if (argv[si->optind][1] == _T('-')) {
						/* --option */
						mm_snprintf(si->errmsg, sizeof(si->errmsg),
							_T("%s: unrecognized option '--%s'\n"),
							argv[0], si->nextchar);
					}
					else {
						/* +option or -option */
						mm_snprintf(si->errmsg, sizeof(si->errmsg),
							_T("%s: unrecognized option '%c%s'\n"),
							argv[0], argv[si->optind][0], si->nextchar);
					}
				}
				si->nextchar = (TCHAR *) _T("");
				si->optind++;
				return _T('?');
			}
	}

	/* Look at and handle the next short option-character.  */

	{
		TCHAR c = *si->nextchar++;
		TCHAR *temp = T_strchr (optstring, c);

		/* Increment `si->optind' when we start to process its last character.  */
		if (*si->nextchar == '\0')
			++si->optind;

		if (temp == NULL || c == _T(':'))
		{
			if (si->opterr)
			{
				if (posixly_correct) {
					/* 1003.2 specifies the format of this message.  */
					mm_snprintf(si->errmsg, sizeof(si->errmsg),
						_T("%s: illegal option '-%c'\n"), argv[0], c);
				}
				else {
					mm_snprintf(si->errmsg, sizeof(si->errmsg),
						_T("%s: invalid option '-%c'\n"), argv[0], c);
				}
			}
			si->optopt = c;
			return _T('?');
		}
		if (temp[1] == _T(':'))
		{
			if (temp[2] == _T(':'))
			{
				/* This is an option that accepts an argument optionally.  */
				if (*si->nextchar != '\0')
				{
					si->optarg = si->nextchar;
					si->optind++;
				}
				else
					si->optarg = NULL;
				
				si->nextchar = NULL;
			}
			else
			{
				/* This is an option that requires an argument.  */
				if (*si->nextchar != '\0')
				{
					si->optarg = si->nextchar;
					/* If we end this ARGV-element by taking the rest as an arg,
					we must advance to the next element now.  */
					si->optind++;
				}
				else if (si->optind == argc)
				{
					if (si->opterr)
					{
						/* 1003.2 specifies the format of this message.  */
						mm_snprintf(si->errmsg, sizeof(si->errmsg),
							_T("%s: option requires an argument -- %c\n"),
							argv[0], c);
					}
					si->optopt = c;
					if (optstring[0] == _T(':'))
						c = _T(':');
					else
						c = _T('?');
				}
				else {
					/* We already incremented `si->optind' once;
					increment it again when taking next ARGV-elt as argument.  */
					si->optarg = argv[si->optind++];
				}
				si->nextchar = NULL;
			}
		}
		return c;
	}
}

sgetopt_ctx *
sgetopt_ctx_create()
{
	sgetopt_ctx *si = (sgetopt_ctx*)malloc(sizeof(sgetopt_ctx));
	if(!si)
		return NULL;

	memset(si, 0, sizeof(sgetopt_ctx));
	si->signature = SIGNATURE;
	return si;
}

sgetopt_err_et
sgetopt_ctx_delete(sgetopt_ctx *si)
{
	if(si->signature!=SIGNATURE)
		return sgetopt_fail;

	free(si);
	return sgetopt_ok;
}

int
sgetopt(sgetopt_ctx *si, int argc, TCHAR *const argv[], const TCHAR *optstring)
{
	return _sgetopt_internal (si,
		argc, argv, optstring,
		(const sgetopt_option *) 0,
		(int *) 0,
		0);
}

int 
sgetopt_long(sgetopt_ctx *si, 
	int argc, TCHAR * const argv[], const TCHAR *optstring,
	const sgetopt_option *longopts, int *longindex)
{
	return _sgetopt_internal(si,
		argc, argv, optstring,
		longopts, longindex,
		0);
}

int 
sgetopt_long_only(sgetopt_ctx *si, 
	int argc, TCHAR * const argv[], const TCHAR *optstring,
	const sgetopt_option *longopts, int *longindex)
{
	return _sgetopt_internal(si,
		argc, argv, optstring,
		longopts, longindex,
		1);
}

#ifdef TEST

/* Compile with -DTEST to make an executable for use in testing
the above definition of `getopt'.  */

#include <ctype.h>

int
_tmain (int argc, TCHAR **argv)
{
	int c;
	int digit_optind = 0;
	sgetopt_ctx *si = sgetopt_ctx_create();

	while (1)
	{
		int this_option_optind = si->optind ? si->optind : 1;

		c = sgetopt (si, argc, argv, "abc:d:0123456789");
		if (c == EOF)
			break;

		switch (c)
		{{
		case _T('0'):
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
		case _T('8'):
		case _T('9'):
			if (digit_optind != 0 && digit_optind != this_option_optind)
				printf ("digits occur in two different argv-elements.\n");
			digit_optind = this_option_optind;
			printf ("option %c\n", c);
			break;

		case _T('a'):
			printf ("option a\n");
			break;

		case _T('b'):
			printf ("option b\n");
			break;

		case _T('c'):
			printf ("option c with value '%s'\n", si->optarg);
			break;

		case _T('?'):
		{
			if (si->optopt == _T('c'))
				fprintf (stderr, "Option -%c requires an argument.\n", si->optopt);
			else if (isprint (si->optopt))
				fprintf (stderr, "Unknown option '-%c'.\n", si->optopt);
			else
				fprintf (stderr, "Unknown option character '\\x%x'.\n",	si->optopt);		
			break;
		}

		default:
			printf ("?? getopt returned character code 0%o ??\n", c);
		}}
	}

	if (si->optind < argc)
	{
		printf ("non-option ARGV-elements: ");
		while (si->optind < argc)
			printf ("%s ", argv[si->optind++]);
		printf ("\n");
	}

	sgetopt_ctx_delete(si);
	exit (0);
}

#endif /* TEST */
