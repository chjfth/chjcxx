#include <stdio.h>
#include <ctype.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <getopt/sgetopt.h>

static const TCHAR *app_short_options = _T("abc:d:0123");

static sgetopt_option app_long_options[] = 
{
	{_T("add"),     has_arg_yes, 0,  0 },
	{_T("append"),  has_arg_no , 0,  0 },
	{_T("delete"),  has_arg_yes, 0,  0 },
	{_T("verbose"), has_arg_no , 0,  0 },
	{_T("create"),  has_arg_yes, 0, 'c'},
	{_T("file"),    has_arg_yes, 0,  0 },
	{0, 0, 0, 0}
};

bool is_app_option(const TCHAR *opt)
{
	// Check if opt is a defined(=expected) option.
	// opt is one argv[n] element
	int i=0;
	if(opt[0]==_T('-') && opt[1]==_T('-'))
	{
		// long option format
		for(i=0; app_long_options[i].name!=NULL; i++)
		{
			if( T_strcmp(app_long_options[i].name, opt+2)==0 )
				return true;
		}
		return false;
	}
	else if(opt[0]==_T('-'))
	{
		// short option format
		int len = T_strlen(app_short_options);
		for(i=0; i<len; i++)
		{
			if( app_short_options[i]==opt[1] )
				return true;
		}
		return false;
	}
	return false;
}


#ifdef _MSC_VER
int _tmain(int argc, TCHAR *argv[])
#else
int main(int argc, char *ansi_argv[])
#endif
{
	// We need to have argv[] always match TCHAR.

#if (defined UNICODE || defined _UNICODE) && (!defined _MSC_VER) // the linux case
	wchar_t **argv = ggt_argvs_utf8_to_wchars(argc, argv);
#endif
	if(argc==1)
	{
		T_printf(_T("No options given.\n"));
		return 0;
	}

	int c = 0;
	int digit_optind = 0;
	sgetopt_ctx *si = sgetopt_ctx_create();
	si->opterr = 1;

	while (1)
	{
		int this_option_optind = si->optind ? si->optind : 1;
		int longindex = 0;

		c = sgetopt_long(si, argc, argv, app_short_options,
			app_long_options, &longindex);
		
		if (c == -1)
			break;

		switch (c)
		{{
		case 0:
			T_printf(_T("long option --%s"), app_long_options[longindex].name);
			if(si->optarg)
				T_printf(_T("=%s"), si->optarg);
			T_printf(_T("\n"));

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
			T_printf(_T("option -a\n"));
			break;

		case 'b':
			T_printf(_T("option -b\n"));
			break;

		case 'c':
			T_printf(_T("option -c : '%s'\n"), si->optarg);
			break;

		case '?':
		{
			const TCHAR *problem_opt = argv[si->optind-1];
			if(is_app_option(problem_opt))
				T_printf(_T("Option '%s' missing an argument\n"), problem_opt); 
			else
				T_printf(_T("Bad option '%s'\n"), problem_opt); 
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
			T_printf(_T("%s "), argv[si->optind++]); // an extra tail space
		}
		T_printf(_T("\n"));
	}

	sgetopt_ctx_delete(si);

#if (defined UNICODE || defined _UNICODE) && (!defined _MSC_VER) // the linux case
	ggt_argvs_free_wchars(argc, argv);
#endif

	return (0);
}

