#include "driver.h"

int run_client(int, char **, struct tiptap *);

int main(int argc, char **argv)
{
	struct tiptap tt;
	return run_client(argc, argv, &tt);
}
