#include <stdio.h>
#include <ctype.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <gadgetlib/charenccvt.h>
#include <getopt/sgetopt.h>

static const TCHAR *app_short_options = _T("abc:d:0123");

enum { optn_verbose = 128, optn_filename, optn_more1, optn_more2 };

static sgetopt_option app_long_options[] = 
{
	{_T("verbose"), has_arg_no , 0,  optn_verbose },
	{_T("add"),     has_arg_yes, 0,  0 },
	{_T("append"),  has_arg_no , 0,  0 },
	{_T("delete"),  has_arg_yes, 0,  0 },
	{_T("create"),  has_arg_yes, 0, 'c'},
	{_T("file"),    has_arg_yes, 0,  optn_filename },
	{0, 0, 0, 0}
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
		T_printf(_T("No options given.\n"));
		return 0;
	}

	int c = 0;
	int digit_optind = 0; // not important

	sgetopt_ctx *si = sgetopt_ctx_create(); // important 
	si->opterr = 1; // (optional)

	while (1)
	{
		int this_option_optind = si->optind ? si->optind : 1;
		int longindex = 0;

		c = sgetopt_long(si, argc, argv, app_short_options,
			app_long_options, &longindex);
		
		if (c == -1)
			break;

		const TCHAR *optval = si->optarg;

		switch (c)
		{{

		// Two flavors of using long-options:
		// (1) Got case 0 then check app_long_options[longindex]
		// (2) Got an int-result as assigned in sgetopt_option.opt_id

		case 0:
			T_printf(_T("long option --%s"), app_long_options[longindex].name);
			if(optval)
				T_printf(_T("=%s"), optval);
			T_printf(_T("\n"));

			break;

		case optn_verbose:
			T_printf(_T("Valid option --verbose\n"));
			break;

		case optn_filename:
			T_printf(_T("Valid option --file=%s\n"), optval);
			break;

		case '0':
		case '1':
		case '2':
		case '3':
			if (digit_optind != 0 && digit_optind != this_option_optind)
				T_printf(_T("digits occur in two different argv-elements.\n"));
			digit_optind = this_option_optind;
			T_printf(_T("option %c\n"), c);
			break;

		case 'a':
			T_printf(_T("Valid option -a\n"));
			break;

		case 'b':
			T_printf(_T("Valid option -b\n"));
			break;

		case 'c':
			T_printf(_T("Valid option '-c' with argument '%s'\n"), si->optarg);
			break;

		case '?':
		{
			// Chj: Short-option error and Long-option error are determined differently.
			if(si->optopt)
			{
				// Short-option error
				if(sgetopt_is_valid_short(si->optopt, app_short_options))
					T_printf(_T("Short option '-%c' missing an argument.\n"), si->optopt);
				else
					T_printf(_T("Bad short option '-%c'.\n"), si->optopt);

				si->optopt = 0; // otherwise, the err will stay there when next error is on long-option
			}
			else
			{
				// Long-option error
				const TCHAR *problem_opt = argv[si->optind-1];
				if(sgetopt_is_valid_long(problem_opt, app_long_options))
					T_printf(_T("Long option '%s' missing an argument\n"), problem_opt); 
				else
					T_printf(_T("Bad long option '%s'\n"), problem_opt); 
			}
			break;
		}

		default:
			T_printf(_T("?? getopt returned character code 0x%X ??\n"), c);
		}}
	}

	if (si->optind < argc)
	{
		T_printf(_T("non-option ARGV-elements: "));
		while (si->optind < argc)
		{
			T_printf(_T("%s "), argv[si->optind++]);
		}
		T_printf(_T("\n"));
	}

	sgetopt_ctx_delete(si);

#if (defined UNICODE || defined _UNICODE) && (!defined _MSC_VER) // the linux case
	ggt_argvs_free_wchars(argv);
#endif

	return (0);
}

