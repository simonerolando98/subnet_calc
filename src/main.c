#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subnet.h"

void usage()
{
	printf("%s\n",
		"subnet: missing parameter.\n"
		"Try 'subnet --help' for more information."
	);
}

void help()
{
	printf("usage:\tsubnet [OPTIONS] <ip-address>\n");
	printf("or: \tsubnet [OPTIONS_NETMASK] <subnet-mask>\n\n");
	printf("Perform NETWORK CALCUATIONS based on input\n");
}

int main(int argc, char* const argv[])
{
	/* checking arguments number */
	if (argc < 2) {
		usage();
		exit(EXIT_FAILURE);
	}

	/* parsing arguments */
	while ()

    return 0;
}