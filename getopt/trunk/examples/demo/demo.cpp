#include <stdio.h>

#include <ps_TCHAR.h>
#include <getopt/sgetopt.h>

#include <ctype.h>


static sgetopt_option long_options[] = 
{
	{"add",     has_arg_yes, 0,  0 },
	{"append",  has_arg_no , 0,  0 },
	{"delete",  has_arg_yes, 0,  0 },
	{"verbose", has_arg_no , 0,  0 },
	{"create",  has_arg_yes, 0, 'c'},
	{"file",    has_arg_yes, 0,  0 },
	{0,         0,           0,  0 }
};

int
_tmain(int argc, TCHAR **argv)
{
	if(argc==1)
	{
		printf("No options given.\n");
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

		c = sgetopt_long(si, argc, argv, "abc:d:0123",
			long_options, &longindex);
		
		if (c == -1)
			break;

		switch (c)
		{{
		case 0:
			printf("long option --%s", long_options[longindex].name);
			if(si->optarg)
				printf(" with arg %s", si->optarg);
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
			printf ("option -c with value '%s'\n", si->optarg);
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

