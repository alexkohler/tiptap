#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <assert.h>

#include "driver.h"
#include "tiptap.h"

#define LOG_OUTPUT(func, ...) \
	do { \
		if (logfile) func(logfile, __VA_ARGS__); \
		if (verbose) func(stdout, __VA_ARGS__); \
	} while(0);

static void usage(char *name)
{
	printf(
		"Usage: %s [OPTION]...\n"
		"Runs the workload from standard input on a virtual TipTap.\n"
		"  -h         show this help\n"
		"  -v         output verbose log data to stdout\n"
		"  -o FILE    output log data to FILE\n"
		"\n"
		"To read a workload from a file, put it on standard input:\n"
		"  %s -v < WORKLOADFILE\n"
    "  OR\n"
    "  cat WORKLOADFILE | %s -v\n",
		name, name, name);
}

int main(int argc, char **argv)
{
	void internal_get_counts(uint32_t *, uint32_t *, uint32_t *, uint32_t *);
	void internal_set_dims(int, int, int);
	void internal_set_log(int, FILE*);
	int internal_verify_counts(uint32_t inits, uint32_t deinits, uint32_t cmds, uint32_t dist);
	int internal_verify_data(uint8_t *hash);
	int internal_verify_x(uint16_t x);
	int internal_verify_y(uint16_t y);
	int internal_verify_z(uint16_t z);
	int internal_verify_pos(uint16_t x, uint16_t y, uint16_t z);
	void internal_hw_failure(void);

	int ret = 0;
	char *name = argv[0];
	uint32_t inits, deinits, cmds, dist;

	// Option defaults
	int verbose = 0;
	char *logname = NULL;

	// Parse options
	int opt;
	while ((opt = getopt(argc, argv, "hvo:")) != -1) {
		switch (opt) {
			case 'v':
				verbose = 1;
				break;
			case 'o':
				logname = optarg;
				break;
			case 'h':
			default:
				usage(name);
				return 1;
		}
	}

	// Open log file if any
	FILE *logfile = NULL;
	if (logname) {
		logfile = fopen(logname, "w");
		if (!logfile) {
			perror("opening logfile");
			return 1;
		}
	}
	internal_set_log(verbose, logfile);
	LOG_OUTPUT(fprintf, "Starting Virtual TipTap v" TIPTAP_VERSION "\n");

	// Read first data and sanity-check: dimensions
	uint16_t width, height, depth;
	if (scanf("%hu %hu %hu", &width, &height, &depth) != 3) {
		fprintf(stderr, "Workload input: failed to read dimensions\n");
		return 2;
	} else if (width >= 1024 || height >= 1024 || depth >= 1024) {
		fprintf(stderr, "Workload input: dimensions too large: %dx%dx%d\n",
				width, height, depth);
		return 2;
	}
	internal_set_dims(width, height, depth);
	LOG_OUTPUT(fprintf, "Max print dimensions: %ux%ux%u\n", width, height, depth);

	// This line is the reason we have to distribute simulate.c
	struct tiptap tt;

	// Now read commands
	char cmd[6];
	uint16_t x, y, z, w, h;
	uint8_t *buf;
	uint16_t *ibuf;
	int erv;
	int arv;
	int i;
	while (scanf("%5s", cmd) == 1) {
		if (cmd[4] == '@') {
			// Expected to fail
			erv = 1;
			cmd[4] = '\0';
			LOG_OUTPUT(fprintf, "Command: %s (driver should return error)\n", cmd);
		} else if (cmd[4] == '!') {
			// Test HW failure conditions
			erv = 1;
			internal_hw_failure();
			cmd[4] = '\0';
			LOG_OUTPUT(fprintf, "Command: %s (simulating hw failure)\n",
					cmd);
		} else {
			// Expected to succeed
			erv = 0;
			LOG_OUTPUT(fprintf, "Command: %s\n", cmd);
		}

		// Parse command
		if (!strcmp(cmd, "INIT")) {
			// Initialize driver
			arv = tiptap_init(&tt);
		} else if (!strcmp(cmd, "DEST")) {
			// Clean up driver
			tiptap_destroy(&tt);
			arv = 0;
		} else if (!strcmp(cmd, "MOVE")) {
			// Move to the specified X, Y, Z coordinates
			if (scanf("%hu %hu %hu", &x, &y, &z) != 3) {
				fprintf(stderr, "Workload input: Malformed MOVE command\n");
				ret = 2;
				goto out_destroy;
			}

			arv = tiptap_moveto(&tt, x, y, z);
		} else if (!strcmp(cmd, "GETP")) {
			x = y = z = 65535;
			tiptap_getpos(&tt, &x, &y, &z);
			if (x == 65535 || y == 65535 || z == 65535) {
				LOG_OUTPUT(fprintf, "Did not set all output parameters\n");
				ret = 1;
				goto out_destroy;
			}
			LOG_OUTPUT(fprintf, "Returned position: (%hu, %hu, %hu)\n", x, y, z);
			if (internal_verify_pos(x, y, z)) {
				ret = 1;
				goto out_destroy;
			}
		} else if (!strcmp(cmd, "PRNT")) {
			if (scanf("%hu %hu %hu %hu", &x, &y, &w, &h) != 4) {
				fprintf(stderr, "Workload input: Malformed PRNT command\n");
				ret = 2;
				goto out_destroy;
			}

			// Load voxel data
			buf = calloc(w * h, sizeof(buf[0]));
			for (i = 0; i < w * h; i++) {
				if (scanf("%2hhx", &buf[i]) != 1) {
					fprintf(stderr, "Workload input: Malformed pixel data\n");
					ret = 2;
					goto out_destroy;
				}
			}
			LOG_OUTPUT(fprintf, "Printing %ux%u layer at (%u, %u)\n", w, h, x, y);
			arv = tiptap_printlayer(&tt, x, y, w, h, buf);
			free(buf);
		} else if (!strcmp(cmd, "SCAN")) {
			if (scanf("%hu %hu %hu %hu", &x, &y, &w, &h) != 4) {
				fprintf(stderr, "Workload input: Malformed SCAN command\n");
				ret = 2;
				goto out_destroy;
			}

			// Get height data
			ibuf = calloc(w * h, sizeof(ibuf[0]));
			LOG_OUTPUT(fprintf, "Scanning %ux%u region at (%u, %u)\n", w, h, x, y);
			arv = tiptap_scan(&tt, x, y, w, h, ibuf);
			// XXX verify scan data?
			free(ibuf);
		} else if (!strcmp(cmd, "VDAT")) {
			// Verify functions should return success
			assert(erv == 0);
			arv = 0;

			LOG_OUTPUT(fprintf, "Verifying printed material data\n");

			// Read in target hash from workload
			uint8_t hash[20];
			for (i = 0; i < 20; i++) {
				if (scanf("%2hhx", &hash[i]) != 1) {
					fprintf(stderr, "Workload input: malformed VDAT command\n");
					ret = 2;
					goto out_destroy;
				}
			}

			// Verify actual hash
			if (internal_verify_data(hash)) {
				ret = 1;
				goto out_destroy;
			}
		} else if (!strcmp(cmd, "VPOS")) {
			// Verify functions should return success
			assert(erv == 0);
			arv = 0;

			LOG_OUTPUT(fprintf, "Verifying current nozzle position\n");

			if (scanf("%hu", &x) == 1) {
				arv = internal_verify_x(x) || arv;
			} else if (getchar() != '*') {
				fprintf(stderr, "Workload input: Malformed VPOS command\n");
				ret = 2;
				goto out_destroy;
			}
			if (scanf("%hu", &y) == 1) {
				arv = internal_verify_y(y) || arv;
			} else if (getchar() != '*') {
				fprintf(stderr, "Workload input: Malformed VPOS command\n");
				ret = 2;
				goto out_destroy;
			}
			if (scanf("%hu", &z) == 1) {
				arv = internal_verify_z(z) || arv;
			} else if (getchar() != '*') {
				fprintf(stderr, "Workload input: Malformed VPOS command\n");
				ret = 2;
				goto out_destroy;
			}

			if (arv) {
				ret = 1;
				goto out_destroy;
			}
		} else if (!strcmp(cmd, "VCTR")) {
			// Verify functions should return success
			assert(erv == 0);
			arv = 0;

			LOG_OUTPUT(fprintf, "Verifying poweron/off/cmd/dist counters\n");

			uint32_t ti, tdi, tc, td;
			if (scanf("%u %u %u %u", &ti, &tdi, &tc, &td) != 4) {
				fprintf(stderr, "Workload input: Malformed VCTR command\n");
				ret = 2;
				goto out_destroy;
			}

			// Verify powerons, poweroffs, cmds, and distance
			if (internal_verify_counts(ti, tdi, tc, td)) {
				ret = 1;
				goto out_destroy;
			}
		} else {
			fprintf(stderr, "Workload input: unrecognized command %s\n",
					cmd);
			ret = 2;
			goto out_destroy;
		}

		LOG_OUTPUT(fprintf, "Return value: %s\n", arv ? "error" : "success");
		if (!arv != !erv) {
			LOG_OUTPUT(fprintf, "Expected return value: %s\n", erv ?
					"error" : "success");
			ret = 1;
			goto out_destroy;
		}
	}

out_destroy:
	// Print out totals
	internal_get_counts(&inits, &deinits, &cmds, &dist);

	fprintf(stderr, "\nTotals: poweron=%u, poweroff=%u, cmds=%u, distance=%u\n",
			inits, deinits, cmds, dist);

	if (logfile)
		fclose(logfile);
	if (ret)
		fprintf(stderr, "Test FAILED\n");
	else
		fprintf(stderr, "Test passed\n");
	return ret;
}
