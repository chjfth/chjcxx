#include <stdio.h>
#include <stdarg.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <mm_snprintf.h>
#include <getopt/sgetopt.h>

#include <ctype.h>

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

verify_st gar_verifies[] =
{
	{
		_T("-a -b param0 -c val --add=yes --delete no param1 param2"), _T(
		"option -a\n"
		"option -b\n"
		"option -c : 'val'\n"
		"long option --add=yes\n"
		"long option --delete=no\n"
		"non-option ARGV-elements: param0 param1 param2 \n"
		)
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
	mm_snprintf(gbuf, GBUF_CHARS, _T("verify %s"), argvstr);
	TCHAR *pbuf = gbuf;
	int argcount = 0;
	for(;;)
	{
		while( issep(*pbuf) && *pbuf )
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

void mm_strcat_reset()
{
	gbufo[0] = _T('\0');
}

void mm_strcat(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	mm_snprintf(gbufo, GBUFO_CHARS, _T("%s%w"), gbufo, fmt, &args);

	va_end(args);
}

int
_tmain()
{
	int argc = my_split_argvstr(case1_argvstr);
	TCHAR **argv = gargv;

	int c = 0;
	int digit_optind = 0;
	sgetopt_ctx *si = sgetopt_ctx_create();
	si->opterr = 1;

	mm_strcat_reset();

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
			mm_strcat(_T("long option --%s"), app_long_options[longindex].name);
			if(si->optarg)
				mm_strcat(_T("=%s"), si->optarg);
			mm_strcat(_T("\n"));

			break;

		case '0':
		case '1':
		case '2':
		case '3':
			if (digit_optind != 0 && digit_optind != this_option_optind)
				mm_strcat(_T("digits occur in two different argv-elements.\n"));
			digit_optind = this_option_optind;
			mm_strcat(_T("option %c\n", c));
			break;

		case 'a':
			mm_strcat(_T("option -a\n"));
			break;

		case 'b':
			mm_strcat(_T("option -b\n"));
			break;

		case 'c':
			mm_strcat(_T("option -c : '%s'\n"), si->optarg);
			break;

		case '?':
		{
			if (si->optopt == 'c')
				mm_strcat(_T("Option -%c requires an argument.\n"), si->optopt);
			else if (isprint (si->optopt))
				mm_strcat(_T("Unknown option '-%c'.\n"), si->optopt);
			else {
				// fprintf (stderr, "Unknown option character '\\x%x'.\n",	si->optopt);	
				mm_strcat(_T("Bad option %s\n"), argv[si->optind-1]); 
					// Chj: looks ok, can output invalid long options.
			}
			break;
		}

		default:
			mm_strcat(_T("?? getopt returned character code 0x%X ??\n"), c);
		}}
	}

	if (si->optind < argc)
	{
		mm_strcat(_T("non-option ARGV-elements: "));
		while (si->optind < argc)
			mm_strcat(_T("%s "), argv[si->optind++]);
		mm_strcat(_T("\n"));
	}

	sgetopt_ctx_delete(si);

	int compre = T_strcmp(gar_verifies[0].output, gbufo);

/*
	int i;
	for(i=0; gbufo[i]; i++)
	{
		if(gar_verifies[0].output[i]!=gbufo[i])
			printf("XXXXXXX@%d\n", i);
	}
*/

	return compre;
}

