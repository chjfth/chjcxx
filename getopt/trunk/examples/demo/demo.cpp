#include <stdio.h>
#include <ctype.h>

#include <ps_TCHAR.h>
//#include <gadgetlib/T_string.h>
#include <mm_snprintf.h>
#include <gadgetlib/charenccvt.h>
#include <getopt/sgetopt.h>

static const TCHAR *app_short_options = _T("abc:d:0123");

enum { optn_verbose = 128, optn_filename, optn_more1, optn_more2 };

static sgetopt_option app_long_options[] = 
{
	{_T("verbose"), has_arg_no , 0,  optn_verbose, _T("v.ctx")},
	{_T("add"),     has_arg_yes, 0,  0 },
	{_T("append"),  has_arg_no , 0,  0 },
	{_T("delete"),  has_arg_yes, 0,  0 },
	{_T("create"),  has_arg_yes, 0, 'c', _T("c.ctx")},
	{_T("file"),    has_arg_yes, 0,  optn_filename },
	{0}
};

#ifdef _MSC_VER
int _tmain(int argc, TCHAR *init_argv[])
#else
int main(int argc, char *init_argv[])
#endif
{
#if (defined UNICODE || defined _UNICODE) && (!defined _MSC_VER) // the linux case
	wchar_t **argv = ggt_argvs_utf8_to_wchars(argc, init_argv);
#else
	TCHAR **argv = init_argv;
#endif
	// Now argv matches TCHAR, this is our purpose.

	if(argc==1)
	{
		mm_printf(_T("No options given.\n"));
		return 0;
	}

	sgetopt_ctx *si = sgetopt_ctx_create(); // important 
	si->opterr = 1; // (optional)

	while (1)
	{
		int longindex = 0;

		int optc = sgetopt_long(si, argc, argv, app_short_options,
			app_long_options, &longindex);
		
		if (optc == -1)
			break;

		if(optc=='?')
		{
			// Chj: Short-option error and Long-option error are determined differently.
			if(si->optopt)
			{
				// Short-option error
				if(sgetopt_is_valid_short(si->optopt, app_short_options))
					mm_printf(_T("Short option '-%c' missing an argument.\n"), si->optopt);
				else
					mm_printf(_T("Bad short option '-%c'.\n"), si->optopt);

				si->optopt = 0; // otherwise, the err will still be there when next error is on a long-option
			}
			else
			{
				// Long-option error
				const TCHAR *problem_opt = argv[si->optind-1];
				if(sgetopt_is_valid_long(problem_opt, app_long_options))
					mm_printf(_T("Long option '%s' missing an argument\n"), problem_opt); 
				else
					mm_printf(_T("Bad long option '%s'\n"), problem_opt); 
			}
			continue; // yes, we can go on checking further valid params
		}

		const TCHAR *optarg = si->optarg;

		// Two flavors of checking long-options:
		// (1) Check short-options and long-options separately.
		//
		// (2) If you have set all different .opt_id in app_long_options[],
		//     you can directly check optc value for both short and long options.
		//
		// The following code uses flavor (1); 
		// Another demo program, Verify-sgetopt, uses flavor (2).

		if(longindex>=0) // meet a long option
		{
			const TCHAR *longopt = app_long_options[longindex].name;
			if(optarg)
				mm_printf(_T("long option --%s=%s"), longopt, optarg);
			else
				mm_printf(_T("long option --%s"), longopt);

			const TCHAR *usrctx = (const TCHAR*)app_long_options[longindex].user_context;
			if(usrctx)
				mm_printf(_T(" [%s]\n"), usrctx);
			else
				mm_printf(_T("\n"));
		}
		else // meet a short option
		{
			if(optarg)
				mm_printf(_T("short option -%c %s\n"), optc, optarg);
			else
				mm_printf(_T("short option -%c\n"), optc);
		}
	
	}

	if (si->optind < argc)
	{
		mm_printf(_T("non-option ARGV-elements: "));
		while (si->optind < argc)
		{
			mm_printf(_T("%s "), argv[si->optind++]);
		}
		mm_printf(_T("\n"));
	}

	sgetopt_ctx_delete(si);

#if (defined UNICODE || defined _UNICODE) && (!defined _MSC_VER) // the linux case
	ggt_argvs_free_wchars(argv);
#endif

	return (0);
}

/* Sample run options:

-c  "shortc" --verbose --create=longc

*/
