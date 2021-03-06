#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <commdefs.h>
#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <mm_snprintf.h>
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

struct verify_st
{
	const TCHAR *input;
	const TCHAR *output;
};

// Use this struct array to add more test cases.
verify_st gar_verify_cases[] =
{
	{
		_T("-a -b param0 -c val --add=yes --delete no param1 param2"),

		_T("option -a\n")
		_T("option -b\n")
		_T("option -c : 'val'\n")
		_T("long option --add=yes\n")
		_T("long option --delete=no\n")
		_T("non-option ARGV-elements: param0 param1 param2\n")
	},
	{	// the same option twice:
		_T("-a -c val1 -b -c val2 param1 --file=file1 --file file2"),

		_T("option -a\n")
		_T("option -c : 'val1'\n")
		_T("option -b\n")
		_T("option -c : 'val2'\n")
		_T("long option --file=file1\n")
		_T("long option --file=file2\n")
		_T("non-option ARGV-elements: param1\n")
	},
	{	// inject some invalid short options:
		_T("-x -b -c 11"),

		_T("Bad option '-x'\n")
		_T("option -b\n")
		_T("option -c : '11'\n")			
	},
	{	// inject some invalid long options:
		_T("--badopt --badget popo"),

		_T("Bad option '--badopt'\n")
		_T("Bad option '--badget'\n")
		_T("non-option ARGV-elements: popo\n")
	},
	{	// inject some invalid short and long options:
		_T("-x -b -c 11 --badopt --badget xxoo"),

		_T("Bad option '-x'\n")
		_T("option -b\n")
		_T("option -c : '11'\n")
		_T("Bad option '--badopt'\n")
		_T("Bad option '--badget'\n")
		_T("non-option ARGV-elements: xxoo\n")
	},
	{	// = between --add and foo can be omitted
		_T("-c -b --add foo bar.ini"),

		_T("option -c : '-b'\n")
		_T("long option --add=foo\n")
		_T("non-option ARGV-elements: bar.ini\n")
	},
	{	// appearance of -- stops options
		_T("-b -- -a --verbose"),

		_T("option -b\n")
		_T("non-option ARGV-elements: -a --verbose\n")
	},
	{	// option's argument missing (short)
		_T("coco -c"),

		_T("Option '-c' missing an argument\n")
		_T("non-option ARGV-elements: coco\n")
	},
	{	// option's argument missing (short, with extra non-opt-ARGV)
		_T("coco AA BB CC -c"),

		_T("Option '-c' missing an argument\n")
		_T("non-option ARGV-elements: coco AA BB CC\n")
	},
	{	// option's argument missing (long)
		_T("codo --file"),

		_T("Option '--file' missing an argument\n")
		_T("non-option ARGV-elements: codo\n")
	},
	{	// a single argument and it is a bad short option
		_T("-z"),

		_T("Bad option '-z'\n")
	},
	{	// a single argument and it is a bad long option
		_T("--badlong"),

		_T("Bad option '--badlong'\n")
	},
	{	// Error checking implemented in v1.0.2 example.
		_T("-XYZ"),

		_T("Bad option '-X'\n")
		_T("Bad option '-Y'\n")
		_T("Bad option '-Z'\n")
	},
	{	// condensed short options
		_T("-ab"),

		_T("option -a\n")
		_T("option -b\n")
	},
	{	// condensed short options with -c missing argument
		_T("-abc"),

		_T("option -a\n")
		_T("option -b\n")
		_T("Option '-c' missing an argument\n")
	},
	{	// condensed short options with argument
		_T("-cba"),

		_T("option -c : 'ba'\n")
	},
	{	// condensed short options with argument, mix long and bad opts
		_T("--badlong -cba --delete oldfile mytail"),

		_T("Bad option '--badlong'\n")
		_T("option -c : 'ba'\n")
		_T("long option --delete=oldfile\n")
		_T("non-option ARGV-elements: mytail\n")
	},
	{	// Passing same optname multiple times are all recognized
		_T("--file=f1.txt --file=f2.txt --file=f3.txt"),

		_T("long option --file=f1.txt\n")
		_T("long option --file=f2.txt\n")
		_T("long option --file=f3.txt\n")
	},
	{	// casual 1
		_T("--c"),
			
		_T("Bad option '--c'\n")
	},
	{	// casual 2
		_T("-zxa --c"),
			
		_T("Bad option '-z'\n")
		_T("Bad option '-x'\n")
		_T("option -a\n")
		_T("Bad option '--c'\n")
	},

};


const TCHAR *case1_argvstr=_T("-a -b param0 -c val --add=yes --delete no param1 param2");

const int GBUFO_CHARS = 4000; // output text buffer
TCHAR gbufo[GBUFO_CHARS];

const int GBUF_CHARS = 200;
TCHAR gbuf[GBUF_CHARS];
TCHAR *gargv[100]; // hope it enough

bool issep(TCHAR c) // is separator
{
	return c==_T(' ') || c==_T('\t') ? true : false;
}

int my_split_argvstr(const TCHAR *argvstr)
{
	// Copy argvstr into writable buffer gbuf[], with a "verify " prefix.
	mm_snprintf(gbuf, GBUF_CHARS, _T("verify %s"), argvstr);

	// Then split argvstr into chunks of sub-strings.
	// Example: 
	// Input(gbuf[]):
	//	verify a1 b2 c3
	// Output:
	//	gargv[0] -> "verify\0";
	//	gargv[1] -> "a1\0";
	//	gargv[2] -> "b2\0";
	//	gargv[3] -> "c3\0";

	TCHAR *pbuf = gbuf;
	int argcount = 0;
	for(;;)
	{
		while( issep(*pbuf) )
			pbuf++;
		
		if(*pbuf==_T('\0'))
			break;
		
		gargv[argcount++] = pbuf;

		while( !issep(*pbuf) && *pbuf)
			pbuf++;
		
		if(*pbuf==_T('\0'))
			break;
		
		*pbuf++ = _T('\0');
	}
	
	gargv[argcount] = NULL;
	return argcount;
}

void gbufo_reset()
{
	gbufo[0] = _T('\0');
}

void gbufo_strcat(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	mm_Strcat(gbufo, GBUFO_CHARS, _T("%w"), MM_WPAIR_PARAM(fmt, args));

	va_end(args);
}

sgetopt_err_et verify_one_case(int argc, TCHAR *argv[], const TCHAR *boilerplate)
{
	int c = 0;
	int digit_optind = 0;
	sgetopt_ctx *si = sgetopt_ctx_create();

	gbufo_reset();

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
			gbufo_strcat(_T("long option --%s"), app_long_options[longindex].name);
			if(si->optarg)
				gbufo_strcat(_T("=%s"), si->optarg);
			gbufo_strcat(_T("\n"));

			break;

		case '0':
		case '1':
		case '2':
		case '3':
			if (digit_optind != 0 && digit_optind != this_option_optind)
				gbufo_strcat(_T("digits occur in two different argv-elements.\n"));
			digit_optind = this_option_optind;
			gbufo_strcat(_T("option %c\n"), c);
			break;

		case 'a':
			gbufo_strcat(_T("option -a\n"));
			break;

		case 'b':
			gbufo_strcat(_T("option -b\n"));
			break;

		case 'c':
			gbufo_strcat(_T("option -c : '%s'\n"), si->optarg);
			break;

		case '?':
		{
			// Chj: Short-option error and Long-option error are determined differently.
			if(si->optopt)
			{
				// Short-option error
				if(sgetopt_is_valid_short(si->optopt, app_short_options))
					gbufo_strcat(_T("Option '-%c' missing an argument\n"), si->optopt);
				else
					gbufo_strcat(_T("Bad option '-%c'\n"), si->optopt);

				si->optopt = 0; // otherwise, the err will stay there when next error is on long-option
			}
			else
			{
				// Long-option error
				const TCHAR *problem_opt = argv[si->optind-1];
				if(sgetopt_is_valid_long(problem_opt, app_long_options))
					gbufo_strcat(_T("Option '%s' missing an argument\n"), problem_opt); 
				else
					gbufo_strcat(_T("Bad option '%s'\n"), problem_opt); 
			}
			break;
		}

		default:
			gbufo_strcat(_T("?? getopt returned character code 0x%X ??\n"), c);
		}}
	}

	if (si->optind < argc)
	{
		gbufo_strcat(_T("non-option ARGV-elements: "));
		while (si->optind < argc)
		{
			if(si->optind == argc-1)
				gbufo_strcat(_T("%s"), argv[si->optind++]);
			else
				gbufo_strcat(_T("%s "), argv[si->optind++]); // an extra tail space
		}
		gbufo_strcat(_T("\n"));
	}

	sgetopt_ctx_delete(si);

	int compre = T_strcmp(gbufo, boilerplate);

	if(compre!=0)
	{
/*
		int i;
		for(i=0; gbufo[i]; i++)
		{
			if(gar_verifies[0].output[i]!=gbufo[i])
				printf("XXXXXXX@%d\n", i);
		}
*/
	}

	return compre==0 ? sgetopt_ok : sgetopt_fail;
}

#ifdef _MSC_VER
int _tmain()
#else
int main()
#endif
{
	int casecount = GetEleQuan_i(gar_verify_cases);

	mm_printf(_T("Verifying %d cases...\n"), casecount);

	int i;
	for(i=0; i<casecount; i++)
	{
		mm_printf(_T("[Case %d] %s\n"), i, gar_verify_cases[i].input);

		int argc = my_split_argvstr(gar_verify_cases[i].input); // outputs gargv[]
		
		if(verify_one_case(argc, gargv, gar_verify_cases[i].output)!=sgetopt_ok)
			break;
	}

	if(i==casecount)
	{
		mm_printf(_T("sgetopt: All %d cases verify success.\n"), i);
		return 0; // success
	}
	else
	{
		mm_printf(_T("sgetopt: Case idx %d verify fail!\n"), i);

		mm_printf(_T("========= Input command line ==========\n"));
		mm_printf(_T("%s\n"), gar_verify_cases[i].input);

		mm_printf(_T("=========== Expected output ===========\n"));
		mm_printf(_T("%s\n"), gar_verify_cases[i].output);

		mm_printf(_T("====== Program output (mismatch) ======\n"));
		mm_printf(_T("%s\n"), gbufo);

		return 4; // fail
	}
}


