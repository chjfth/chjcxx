#include <stdio.h>

#include <ps_TCHAR.h>
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

const TCHAR *case1_argvstr=_T("-a -b param0 -c val --add=yes --delete no param1 param2");

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
		while( issep(*pbuf) )
			pbuf++;
		
		if(*pbuf==_T('\0'))
			break;
		
		gargv[argcount++] = pbuf;

		while( !issep(*pbuf) )
			pbuf++;
		
		if(*pbuf==_T('\0'))
			break;
		
		*pbuf++ = _T('\0');
	}
	
	gargv[argcount] = NULL;
	return argcount;
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
			printf("long option --%s", app_long_options[longindex].name);
			if(si->optarg)
				printf("=%s", si->optarg);
			printf("\n");

			break;

		case '0':
		case '1':
		case '2':
		case '3':
			if (digit_optind != 0 && digit_optind != this_option_optind)
				printf ("digits occur in two different argv-elements.\n");
			digit_optind = this_option_optind;
			printf ("option %c\n", c);
			break;

		case 'a':
			printf ("option -a\n");
			break;

		case 'b':
			printf ("option -b\n");
			break;

		case 'c':
			printf ("option -c : '%s'\n", si->optarg);
			break;

		case '?':
		{
			if (si->optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", si->optopt);
			else if (isprint (si->optopt))
				fprintf (stderr, "Unknown option '-%c'.\n", si->optopt);
			else {
				// fprintf (stderr, "Unknown option character '\\x%x'.\n",	si->optopt);	
				fprintf(stderr, "Bad option %s\n", argv[si->optind-1]); 
					// Chj: looks ok, can output invalid long options.
			}
			break;
		}

		default:
			printf ("?? getopt returned character code 0x%X ??\n", c);
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
	return (0);
}

