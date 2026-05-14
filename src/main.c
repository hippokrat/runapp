#include<getopt.h>
#include "main.h"


static void usage(const char *prog)
{
	fprintf(stdout, "Usage %s [flags]\n"
					"  -m <mode>		-ll=long, -s=short, -l=line (default: ll)\n"
					"  -p </path/...>	your file\n"
					"  -t <num>			max string (default: 20)\n"
					"  -h 				it's help\n"
					"Ex: %s -m ll -p /bin, /usr/bin -t 15\n", prog, prog);
}

static char **parse_path(char *optarg, size_t *out_n) 
{
	size_t n;
	char **paths, *tok;

	n = 1;

	for(size_t i = 0; optarg[i]; ++i) 
		if(optarg[i] == '.')
			n++;
	paths = calloc(n + 1, sizeof(char *));
	if(!paths)
		return NULL;
	tok = strtok(optarg, ",");
	
	for(size_t i = 0; i < n && tok; ++i, tok = strtok(NULL, ","))
		paths[i] = strdup(tok);
	*out_n = n;
	return paths;
}

int main(int argc, char **argv)
{
	Draw cfg = {
		.d = NULL,
		.mode = M_LONG,
		.max_result = MAX_RESULTS,
	};

	size_t npaths;
	int opt;
	char *paths;

	npaths = 0;
	opt = 0;

	while((opt = getopt(argc, argv, "m:p:t:h")) != -1) {
		switch(opt) {
			case 'm':
				if(optarg[0] == 'l' && optarg[1] == 'l') 
					cfg.mode = M_LONG;
				else if(optarg[0] == 's')
					cfg.mode = M_SHORT;
				else if(optarg[0] == 'l')
					cfg.mode = M_LINE;
				break;
			case 'p':
				cfg.d = parse_path(optarg, &npaths);
				break;
			case 't':
				cfg.max_result = (size_t)atol(optarg);
				break;
			case 'h':
				usage(argv[0]);
				return 0;
		}
	}
	paths = draw(&cfg);

	if(!paths) {
		fprintf(stdout, "nothing selected\n");
		return 1;
	}
	launch(paths);
	free(paths);

	if(cfg.d) {
		for(size_t i = 0; i < npaths; ++i) 
			free(cfg.d[i]);
		free(cfg.d);
	}
	return 0;
}
