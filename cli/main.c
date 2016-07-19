#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "uapi/crlog.h"

enum {
	MODE_BINARY,
	MODE_FPRINTF,
	MODE_SPRINTF,
	MODE_DPRINTF,
	MODE_FPUTS,
};

int main(int argc, char *argv[])
{
	static const char str1[] = "String1 String1";
	static const char str2[] = "string2 string2 string2";
	int fdout = STDOUT_FILENO;
	bool instant_decode = false;
	bool use_decoder = false;
	int mode = MODE_BINARY;
	size_t niter = 100;
	int opt, idx;
	size_t i;

	static const char short_opts[] = "m:o:di:h";
	static struct option long_opts[] = {
		{ "mode",		required_argument,	0, 'm'	},
		{ "output",		required_argument,	0, 'o'	},
		{ "decode",		no_argument,		0, 'd'	},
		{ "instant-decode",	no_argument,		0, 1000	},
		{ "iter",		required_argument,	0, 'i'	},
		{ "help",		no_argument,		0, 'h'	},
		{ },
	};

	while (1) {
		idx = -1;
		opt = getopt_long(argc, argv, short_opts, long_opts, &idx);
		if (opt == -1)
			break;

		switch (opt) {
		case 'm':
			if (strcmp(optarg, "binary") == 0) {
				mode = MODE_BINARY;
			} else if (strcmp(optarg, "fprintf") == 0) {
				mode = MODE_FPRINTF;
			} else if (strcmp(optarg, "sprintf") == 0) {
				mode = MODE_SPRINTF;
			} else if (strcmp(optarg, "dprintf") == 0) {
				mode = MODE_DPRINTF;
			} else if (strcmp(optarg, "fputs") == 0) {
				mode = MODE_FPUTS;
			} else
				goto usage;
			break;
		case 'o':
			if (strcmp(optarg, "stdout") == 0) {
				fdout = fileno(stdout);
			} else if (strcmp(optarg, "stderr") == 0) {
				fdout = fileno(stderr);
			} else {
				fdout = open(optarg, O_RDWR | O_CREAT | O_TRUNC, 0644);
				if (fdout < 0) {
					fprintf(stderr, "Can't open %s: %s\n",
						optarg, strerror(errno));
					exit(1);
				}
			}
			break;
		case 'i':
			niter = atoi(optarg);
			break;
		case 'd':
			use_decoder = true;
			break;
		case 1000:
			instant_decode = true;
			break;
		case 'h':
		default:
			goto usage;
		}
	}

	switch (mode) {
	case MODE_BINARY:
	{
		void *crlog_ctl;
		crlog_opts_t opts = {
			.msg_fd		= (fdout == fileno(stdout) ||
					   fdout == fileno(stderr)) ? -1 : fdout,
			.msg_buf_size	= niter * 128,
			.buf_may_grow	= true,
		};

		if (use_decoder && !instant_decode)
			return crlog_process_stream(STDIN_FILENO, fdout);

		if (crlog_init(&crlog_ctl, &opts))
			return 1;

		for (i = 0; i < niter; i++) {
			if (crlog_encode(crlog_ctl, "Some message %s %s %c %li %d %lu\n",
					 str1, str2, 'c', (long)-4, (short)2,
					 (unsigned long)2))
				return 1;
		}

		if (instant_decode)
			crlog_process_all_msg(crlog_ctl, STDOUT_FILENO);

		crlog_fini(crlog_ctl);
		break;
	}
	case MODE_DPRINTF:
	{
		for (i = 0; i < niter; i++) {
			dprintf(fdout, "Some message %s %s %c %li %d %lu\n",
				str1, str2, 'c', (long)-4, (short)2,
				(unsigned long)2);
		}
		break;
	}
	case MODE_FPRINTF:
	{
		FILE *f = fdopen(fdout, "w");

		for (i = 0; i < niter; i++) {
			fprintf(f, "Some message %s %s %c %li %d %lu\n",
				str1, str2, 'c', (long)-4, (short)2,
				(unsigned long)2);
			fflush(f);
		}
		fclose(f);
		break;
	}
	case MODE_SPRINTF:
	{
		static char buf[4096];

		for (i = 0; i < niter; i++) {
			sprintf(buf, "Some message %s %s %c %li %d %lu\n",
				str1, str2, 'c', (long)-4, (short)2,
				(unsigned long)2);
		}
		break;
	}
	case MODE_FPUTS:
	{
		FILE *f = fdopen(fdout, "w");

		for (i = 0; i < niter; i++) {
			fputs("Some message ", f);
			fputs(str1, f);
			putc(' ', f);
			fputs(str2, f);
			putc(' ', f);
			putc('c', f);
			fprintf(f, " %li %d %lu\n", (long)-4, (short)2,
				(unsigned long)2);
		}
		fclose(f);
		break;
	}
	default:
		return 1;
	}
	return 0;
usage:
	fprintf(stderr,
		"crlog [--mode binary|[s|f|d]printf] [--output stdout|stderr|filename] [--decode] [--iter number]\n"
		"\n"

		"Examples:\n"
		"\n"

		" - run 100000 iterations of instant message processing (immediate dprintf calls)\n"
		"\n"
		"       crlog -m dprintf -i 100000\n"
		"\n"

		" - run 100000 iterations in binary mode without processing (queue messages only)\n"
		"\n"
		"       crlog -i 100000\n"
		"\n"

		" - run 100000 iterations in binary mode with decoding after\n"
		"\n"
		"       crlog -i 100000 -d\n"
		"\n"

		" - run 100000 iterations in binary mode with decoding after, writting results into 'out' file\n"
		"\n"
		"       crlog -i 100000 -d -o out\n"
		"\n");
	return 1;
}
