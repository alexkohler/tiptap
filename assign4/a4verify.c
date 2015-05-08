#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <dirent.h>
#include <errno.h>

#include "tiptap.h"
#include "driver.h"
#include "model.h"
#include "a4verify.h"

/* WACKY VOODOO MAGIC IGNORE THIS */
void internal_set_log(int, FILE*);
void internal_set_dims(int, int, int);
int internal_verify_data(uint8_t *hash);
void internal_hw_failure(void);
int skipmem = 0;
/* WAIT WHY ARE YOU LOOKING AT THIS FILE ANYWAY GO BACK TO MODEL.C */

#define A4VERIFY_VERSION "1.5"

/* NO SERIOUSLY NOTHING TO SEE HERE */

static void print_hdr(char *hdr)
{
	fprintf(stderr, "\e[38;5;4m:: \e[38;5;15m%s\e[0m\n", hdr);
}

static void print_test(char *test)
{
	fprintf(stderr, "  *  %s\n", test);
}

static void print_result(int res)
{
	fprintf(stderr, "\e[38;5;%dm >>> %s\e[0m\n\n",
		res ? 1 : 10,
		res ? "FAIL" : "PASS");
}

/* WHAT THE HECK IS "\e[38;5;%d" SUPPOSED TO BE */
/* IT LOOKS LIKE WHAT MY OLD MODEM PRINTED WHEN MOM PICKED UP THE PHONE */

static int chunk_equal(const struct chunk *c1, const struct chunk *c2)
{
	if (c1->x != c2->x) {
		fprintf(stderr, "chunk X coordinate incorrect\n");
		return 0;
	}
	if (c1->y != c2->y) {
		fprintf(stderr, "chunk Y coordinate incorrect\n");
		return 0;
	}
	if (c1->z != c2->z) {
		fprintf(stderr, "chunk Z coordinate incorrect\n");
		return 0;
	}
	if (c1->width != c2->width) {
		fprintf(stderr, "chunk width incorrect\n");
		return 0;
	}
	if (c1->height != c2->height) {
		fprintf(stderr, "chunk height incorrect\n");
		return 0;
	}
	if (c1->depth != c2->depth) {
		fprintf(stderr, "chunk depth incorrect\n");
		return 0;
	}
	if (c1->mat != c2->mat) {
		fprintf(stderr, "chunk material incorrect\n");
		return 0;
	}
	return 1;
}

static int model_equal(const struct model *m1, const struct model *m2)
{
	uint32_t i;
	if (strncmp(m1->title, m2->title, TITLE_MAX)) {
		fprintf(stderr, "model title incorrect\n");
		return 0;
	}
	if (m1->width != m2->width) {
		fprintf(stderr, "model width incorrect\n");
		return 0;
	}
	if (m1->height != m2->height) {
		fprintf(stderr, "model height incorrect\n");
		return 0;
	}
	if (m1->depth != m2->depth) {
		fprintf(stderr, "model depth incorrect\n");
		return 0;
	}
	if (m1->nchunks != m2->nchunks) {
		fprintf(stderr, "model number of chunks incorrect\n");
		return 0;
	}
	for (i = 0; i < m1->nchunks; i++) {
		if (!chunk_equal(&m1->chunks[i], &m2->chunks[i])) {
			fprintf(stderr, "chunk %d incorrect\n", i);
			return 0;
		}
	}
	return 1;
}

/* YUP THOSE WERE THE TWO MOST OBVIOUS FUNCTIONS EVER */
/* GOOD THING YOU SPENT TIME WRITING THOSE */

static int init_random(void)
{
	struct timespec now;
	if (clock_gettime(CLOCK_REALTIME, &now)) {
		perror("internal: clock_gettime");
		return 1;
	}
	srandom(now.tv_sec + now.tv_nsec);
	return 0;
}

static void random_bytes(void *buf, size_t count)
{
	uint16_t *ibuf = buf;
	for (/* count */; count >= 2; count -= 2)
		*ibuf++ = random() & 0xffff;

	uint8_t *cbuf = (uint8_t *) ibuf;
	for (/* count */; count > 0; count--)
		*cbuf++ = random() & 0xff;
}

/* ARE YOU STILL READING THIS */

// Actually, those two functions above are put to good use, they're there to
// make absolutely sure that the struct contains garbage before you initialize
// it.  TMYK.

/* DUDE GO BACK TO MODEL.C WHAT TIME IS IT EVEN */

// Get the number of open files
static int num_files(void)
{
	int i = 0;
	DIR *fds = opendir("/proc/self/fd");
	if (!fds) {
		perror("internal: opendir");
		return -1;
	}
	errno = 0;
	while (readdir(fds))
		i++;
	if (errno) {
		perror("internal: readdir");
		i = 0;
	}
	closedir(fds);
	// Compensate for the open directory
	return i - 1;
}

#define CHECK_M(model, m) do { \
		if (!model_equal((model), (m))) \
			return 1; \
	} while (0)
#define CHECK(m) CHECK_M(&model, (m))

#define INIT_M(model, t) do { \
		random_bytes((model), sizeof(*(model))); \
		if (model_init((model), (t))) { \
			fprintf(stderr, "model_init returned failure\n"); \
			return 1; \
		} \
	} while (0)
#define INIT(m) INIT_M(&model, m)

static int verify_init(void)
{
	// Allocate a model to use for tests
	struct model model;

	// Fill model with garbage, then initialize with a normal title
	print_test("Initializing with title");
	INIT("foo");
	CHECK(&empty_foo);

	// Initialize with no title
	print_test("Initializing with no title");
	INIT("");
	CHECK(&empty_notitle);

	// Initialize with a title of maximum length
	print_test("Initializing with long title");
	INIT("trololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololo");
	CHECK(&empty_lol);

	// Initialize with a title of more than maximum length
	print_test("Initializing with title longer than buffer");
	INIT("trololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololololol");
	CHECK(&empty_lol);

	// Initialize with a title with strange characters
	print_test("Initializing with a title with strange characters");
	/* YOURE A STRANGE CHARACTER OOOOOOOOOOOOOO BURNED */
	INIT("~!@#$%^&*(){``\\\e\n\t\b\a__-=#FOO#=-__[@@@@@@               \t   hello\\");
	CHECK(&init_strange);

	return 0;
}

/* NOOOOOOO GET OUT WHILE YOU STILL CAN */

#define ADD_M(model, ...) do { \
		chunk = (struct chunk) {__VA_ARGS__}; \
		if (model_add_chunk((model), &chunk)) { \
			fprintf(stderr, "model_add_chunk returned failure\n"); \
			return 1; \
		} \
	} while (0)
#define ADD(...) ADD_M(&model, __VA_ARGS__)

#define ADD_ERR_M(model, ...) do { \
		chunk = (struct chunk) {__VA_ARGS__}; \
		if (!model_add_chunk((model), &chunk)) { \
			fprintf(stderr, "model_add_chunk returned success for bad chunk\n"); \
			return 1; \
		} \
	} while (0)
#define ADD_ERR(...) ADD_ERR_M(&model, __VA_ARGS__)

static int verify_add_chunk()
{
	// This function will probably leak a lot of memory #yolo
	/* ^ OH MAN CHECK OUT THIS GUY SOOOOOO HIP WITH ALL HIS HASHTAGS */
	struct model model;
	struct chunk chunk;
	/* NOT */

	print_test("Add one chunk at (0,0,0)");
	INIT("foo");
	ADD(0, 0, 0, 3, 4, 5, 1);
	CHECK(&add1chunk);
	
	print_test("Add one chunk in another location");
	INIT("foo");
	ADD(6, 2, 0, 2, 5, 7, 1);
	CHECK(&add1chunk_off);

	print_test("Add chunk with invalid width");
	INIT("foo");
	ADD_ERR(6, 2, 0, 0, 5, 7, 1);

	print_test("Add chunk with invalid height");
	INIT("foo");
	ADD_ERR(6, 2, 0, 2, 0, 7, 1);

	print_test("Add chunk with invalid depth");
	INIT("foo");
	ADD_ERR(6, 2, 0, 2, 5, 0, 1);

	print_test("Chunk inside a chunk");
	INIT("fooception");
	ADD(0, 0, 0, 8, 8, 8, 1);
	ADD(2, 2, 2, 4, 4, 4, 5);
	CHECK(&add_2nested);

	print_test("Two separate chunks");
	INIT("foo");
	ADD(0, 0, 0, 2, 3, 4, 3);
	ADD(5, 6, 0, 4, 4, 4, 8);
	CHECK(&add_2sep);

	print_test("Two separate chunks, different depths");
	INIT("oof");
	ADD(0, 0, 0, 2, 3, 6, 3);
	ADD(5, 6, 0, 4, 4, 4, 8);
	CHECK(&add_2sep_depth);

	print_test("Two chunks, same location");
	INIT("bar");
	ADD(2, 3, 0, 5, 1, 6, 2);
	ADD(2, 3, 0, 3, 1, 3, 9);
	CHECK(&add_2coinc);

	print_test("Two chunks, same location and dimensions");
	INIT("bar");
	ADD(2, 3, 0, 5, 1, 6, 2);
	ADD(2, 3, 0, 5, 1, 6, 9);
	CHECK(&add_2vcoinc);

	print_test("Ten random chunks");
	INIT("oarus.8xkheshusrcoe");
	ADD(209, 73, 168, 159, 36, 76, 4);
	ADD(147, 216, 46, 11, 24, 180, 8);
	ADD(189, 61, 178, 230, 90, 177, 14);
	ADD(74, 118, 102, 85, 243, 41, 7);
	ADD(121, 91, 28, 242, 76, 118, 5);
	ADD(60, 103, 86, 17, 67, 118, 10);
	ADD(210, 254, 125, 101, 103, 27, 4);
	ADD(242, 170, 205, 220, 159, 96, 3);
	ADD(147, 16, 232, 238, 207, 232, 6);
	ADD(216, 33, 68, 110, 132, 225, 8);
	CHECK(&add_10random);

	print_test("One hundred random chunks");
	INIT("gad.rsual.guichasroahnud,jmgy,rc.l smuoae sguadstoeg,.uh asoerusaoerchpy,.");
	ADD(112, 208, 99, 5, 51, 149, 8);
	ADD(145, 4, 148, 167, 117, 55, 6);
	ADD(162, 12, 254, 24, 253, 148, 10);
	ADD(18, 140, 55, 169, 104, 189, 6);
	ADD(142, 59, 212, 98, 103, 188, 1);
	ADD(247, 162, 2, 12, 147, 117, 7);
	ADD(87, 48, 157, 221, 243, 122, 9);
	ADD(209, 144, 237, 60, 116, 85, 7);
	ADD(117, 50, 30, 1, 119, 99, 6);
	ADD(16, 196, 217, 134, 121, 36, 4);
	ADD(192, 180, 6, 79, 224, 115, 7);
	ADD(87, 69, 255, 245, 94, 38, 2);
	ADD(128, 3, 222, 21, 230, 238, 10);
	ADD(239, 181, 62, 142, 52, 110, 9);
	ADD(199, 4, 109, 134, 64, 15, 4);
	ADD(133, 72, 200, 3, 126, 1, 7);
	ADD(255, 192, 71, 211, 150, 62, 5);
	ADD(222, 180, 218, 92, 226, 131, 7);
	ADD(133, 239, 214, 130, 225, 147, 5);
	ADD(119, 222, 22, 21, 196, 144, 9);
	ADD(54, 244, 204, 158, 243, 246, 5);
	ADD(87, 55, 251, 161, 226, 173, 2);
	ADD(125, 136, 181, 68, 103, 156, 6);
	ADD(23, 65, 161, 34, 188, 46, 3);
	ADD(151, 147, 254, 118, 173, 243, 1);
	ADD(93, 119, 10, 185, 184, 194, 1);
	ADD(16, 35, 42, 146, 197, 68, 5);
	ADD(120, 177, 185, 136, 143, 14, 8);
	ADD(146, 73, 109, 208, 45, 147, 6);
	ADD(2, 12, 150, 118, 237, 128, 5);
	ADD(71, 221, 96, 183, 206, 25, 6);
	ADD(241, 149, 161, 89, 154, 29, 5);
	ADD(66, 164, 112, 237, 104, 226, 5);
	ADD(50, 56, 9, 181, 162, 33, 8);
	ADD(170, 37, 135, 41, 70, 174, 7);
	ADD(141, 172, 131, 175, 87, 111, 5);
	ADD(73, 180, 12, 225, 206, 18, 1);
	ADD(9, 213, 21, 218, 43, 199, 8);
	ADD(86, 138, 44, 84, 62, 13, 5);
	/* OK NOW YOURE JUST BEING SILLY */
	ADD(78, 21, 108, 193, 168, 239, 1);
	ADD(86, 77, 152, 228, 180, 208, 3);
	ADD(91, 222, 170, 229, 196, 113, 10);
	ADD(135, 43, 28, 253, 3, 209, 7);
	ADD(74, 101, 161, 246, 86, 95, 1);
	ADD(154, 52, 142, 242, 154, 17, 4);
	ADD(38, 104, 152, 73, 215, 33, 10);
	ADD(37, 117, 102, 126, 97, 231, 6);
	ADD(82, 113, 189, 98, 133, 59, 3);
	ADD(180, 98, 255, 138, 171, 155, 7);
	ADD(10, 198, 186, 173, 191, 150, 8);
	ADD(142, 151, 65, 141, 77, 129, 1);
	ADD(12, 110, 163, 70, 166, 223, 4);
	ADD(15, 192, 235, 201, 65, 41, 4);
	ADD(28, 64, 44, 139, 83, 223, 8);
	ADD(43, 58, 22, 3, 13, 71, 1);
	ADD(26, 218, 185, 108, 228, 164, 2);
	ADD(179, 147, 178, 35, 83, 107, 2);
	ADD(199, 199, 218, 28, 197, 58, 1);
	ADD(190, 207, 193, 73, 222, 33, 4);
	ADD(167, 239, 55, 97, 136, 160, 10);
	ADD(39, 145, 131, 150, 202, 58, 10);
	ADD(153, 8, 103, 114, 20, 105, 6);
	ADD(196, 239, 156, 164, 162, 50, 2);
	ADD(233, 51, 65, 69, 56, 153, 8);
	ADD(185, 188, 35, 162, 36, 172, 8);
	ADD(75, 68, 62, 186, 192, 218, 10);
	ADD(97, 52, 38, 223, 227, 233, 3);
	ADD(21, 39, 10, 247, 205, 164, 8);
	ADD(11, 82, 240, 44, 200, 105, 7);
	ADD(177, 178, 126, 148, 158, 86, 4);
	ADD(205, 31, 48, 183, 90, 28, 3);
	ADD(50, 161, 6, 90, 131, 218, 4);
	ADD(226, 141, 156, 68, 120, 216, 8);
	ADD(28, 248, 32, 4, 144, 20, 3);
	ADD(60, 98, 86, 212, 13, 239, 5);
	ADD(15, 235, 161, 123, 214, 87, 6);
	ADD(172, 224, 146, 237, 208, 70, 3);
	ADD(40, 224, 170, 161, 44, 134, 9);
	ADD(11, 170, 139, 188, 253, 52, 8);
	ADD(146, 92, 94, 160, 29, 73, 7);
	ADD(250, 171, 182, 76, 226, 116, 10);
	ADD(17, 4, 160, 238, 56, 246, 3);
	ADD(177, 255, 108, 231, 113, 62, 2);
	ADD(236, 170, 219, 80, 15, 185, 2);
	ADD(97, 51, 98, 238, 74, 150, 7);
	ADD(215, 59, 48, 26, 123, 100, 1);
	ADD(133, 250, 209, 158, 60, 61, 5);
	ADD(3, 7, 66, 47, 235, 133, 4);
	ADD(188, 85, 131, 105, 185, 90, 2);
	ADD(65, 233, 115, 104, 165, 30, 9);
	ADD(22, 89, 33, 85, 129, 184, 1);
	ADD(176, 243, 97, 246, 142, 166, 4);
	ADD(11, 70, 45, 211, 97, 136, 4);
	ADD(131, 242, 110, 30, 46, 2, 6);
	ADD(90, 12, 60, 197, 111, 216, 5);
	ADD(8, 191, 75, 57, 224, 89, 6);
	ADD(176, 67, 168, 81, 154, 158, 7);
	ADD(102, 133, 133, 151, 242, 238, 5);
	ADD(184, 32, 43, 51, 139, 245, 10);
	ADD(223, 193, 234, 7, 153, 17, 3);
	CHECK(&add_100random);

	/* WELL MR MEMORY LEAK IF YOURE SO FANCY THEN WHY NOT DO 1000 HUH */
	/* I TRIPLE DOG DARE YOU */

	return 0;
	/* FINE JUST RETURN ZERO THEN */

	/* WHATS THE MATTER MCFLY */

	/* CHICKEN */
}

#define CHECK_MALLOC() do { \
		if (skipmem) \
			break; \
		mi = mallinfo(); \
		if (mi.uordblks > bytes) { \
			fprintf(stderr, "model_destroy did not deallocate all memory\n"); \
			return 1; \
		} else if (mi.uordblks < bytes) { \
			fprintf(stderr, "model_destroy freed too much memory??\n"); \
			return 1; \
		} \
	} while (0)

#define DEST_M model_destroy
#define DEST() DEST_M(&model)

static int verify_destroy(void)
{
	struct model model, mods[100];
	struct chunk chunk;

	// Get currently allocated bytes
	struct mallinfo mi = mallinfo();
	int bytes = mi.uordblks;

	if (skipmem)
		fprintf(stderr, "\e[38;5;1m !!! WARNING: \e[0mSkipping memory allocation checks.\n"
				"              Run with -M only when using Valgrind.\n");

	print_test("Empty model");
	INIT("nothing here!");
	DEST();
	CHECK_MALLOC();

	print_test("Two chunks");
	INIT("pizza pizza");
	/* JUST HAD LITTLE CAESARS HUH */
	/* WHEN WILL YOU MAKE ENOUGH MONEY FOR REAL FOOD */
	ADD(2, 3, 0, 5, 1, 6, 2);
	ADD(2, 3, 0, 3, 1, 3, 9);
	DEST();
	CHECK_MALLOC();

	print_test("One hundred random chunks");
	INIT("gad.rsual.guichasroahnud,jmgy,rc.l smuoae sguadstoeg,.uh asoerusaoerchpy,.");
	ADD(112, 208, 99, 5, 51, 149, 8);
	ADD(145, 4, 148, 167, 117, 55, 6);
	ADD(162, 12, 254, 24, 253, 148, 10);
	ADD(18, 140, 55, 169, 104, 189, 6);
	ADD(142, 59, 212, 98, 103, 188, 1);
	ADD(247, 162, 2, 12, 147, 117, 7);
	ADD(87, 48, 157, 221, 243, 122, 9);
	ADD(209, 144, 237, 60, 116, 85, 7);
	ADD(117, 50, 30, 1, 119, 99, 6);
	ADD(16, 196, 217, 134, 121, 36, 4);
	ADD(192, 180, 6, 79, 224, 115, 7);
	ADD(87, 69, 255, 245, 94, 38, 2);
	ADD(128, 3, 222, 21, 230, 238, 10);
	ADD(239, 181, 62, 142, 52, 110, 9);
	ADD(199, 4, 109, 134, 64, 15, 4);
	ADD(133, 72, 200, 3, 126, 1, 7);
	ADD(255, 192, 71, 211, 150, 62, 5);
	ADD(222, 180, 218, 92, 226, 131, 7);
	ADD(133, 239, 214, 130, 225, 147, 5);
	ADD(119, 222, 22, 21, 196, 144, 9);
	ADD(54, 244, 204, 158, 243, 246, 5);
	ADD(87, 55, 251, 161, 226, 173, 2);
	ADD(125, 136, 181, 68, 103, 156, 6);
	ADD(23, 65, 161, 34, 188, 46, 3);
	ADD(151, 147, 254, 118, 173, 243, 1);
	ADD(93, 119, 10, 185, 184, 194, 1);
	ADD(16, 35, 42, 146, 197, 68, 5);
	ADD(120, 177, 185, 136, 143, 14, 8);
	ADD(146, 73, 109, 208, 45, 147, 6);
	ADD(2, 12, 150, 118, 237, 128, 5);
	ADD(71, 221, 96, 183, 206, 25, 6);
	ADD(241, 149, 161, 89, 154, 29, 5);
	ADD(66, 164, 112, 237, 104, 226, 5);
	ADD(50, 56, 9, 181, 162, 33, 8);
	ADD(170, 37, 135, 41, 70, 174, 7);
	ADD(141, 172, 131, 175, 87, 111, 5);
	ADD(73, 180, 12, 225, 206, 18, 1);
	ADD(9, 213, 21, 218, 43, 199, 8);
	ADD(86, 138, 44, 84, 62, 13, 5);
	ADD(78, 21, 108, 193, 168, 239, 1);
	ADD(86, 77, 152, 228, 180, 208, 3);
	ADD(91, 222, 170, 229, 196, 113, 10);
	ADD(135, 43, 28, 253, 3, 209, 7);
	ADD(74, 101, 161, 246, 86, 95, 1);
	ADD(154, 52, 142, 242, 154, 17, 4);
	ADD(38, 104, 152, 73, 215, 33, 10);
	ADD(37, 117, 102, 126, 97, 231, 6);
	ADD(82, 113, 189, 98, 133, 59, 3);
	ADD(180, 98, 255, 138, 171, 155, 7);
	ADD(10, 198, 186, 173, 191, 150, 8);
	ADD(142, 151, 65, 141, 77, 129, 1);
	ADD(12, 110, 163, 70, 166, 223, 4);
	ADD(15, 192, 235, 201, 65, 41, 4);
	ADD(28, 64, 44, 139, 83, 223, 8);
	ADD(43, 58, 22, 3, 13, 71, 1);
	ADD(26, 218, 185, 108, 228, 164, 2);
	/* SERIOUSLY THIS AGAIN */
	ADD(179, 147, 178, 35, 83, 107, 2);
	ADD(199, 199, 218, 28, 197, 58, 1);
	ADD(190, 207, 193, 73, 222, 33, 4);
	ADD(167, 239, 55, 97, 136, 160, 10);
	ADD(39, 145, 131, 150, 202, 58, 10);
	ADD(153, 8, 103, 114, 20, 105, 6);
	ADD(196, 239, 156, 164, 162, 50, 2);
	ADD(233, 51, 65, 69, 56, 153, 8);
	ADD(185, 188, 35, 162, 36, 172, 8);
	ADD(75, 68, 62, 186, 192, 218, 10);
	ADD(97, 52, 38, 223, 227, 233, 3);
	ADD(21, 39, 10, 247, 205, 164, 8);
	ADD(11, 82, 240, 44, 200, 105, 7);
	ADD(177, 178, 126, 148, 158, 86, 4);
	ADD(205, 31, 48, 183, 90, 28, 3);
	ADD(50, 161, 6, 90, 131, 218, 4);
	ADD(226, 141, 156, 68, 120, 216, 8);
	ADD(28, 248, 32, 4, 144, 20, 3);
	ADD(60, 98, 86, 212, 13, 239, 5);
	ADD(15, 235, 161, 123, 214, 87, 6);
	ADD(172, 224, 146, 237, 208, 70, 3);
	ADD(40, 224, 170, 161, 44, 134, 9);
	ADD(11, 170, 139, 188, 253, 52, 8);
	ADD(146, 92, 94, 160, 29, 73, 7);
	ADD(250, 171, 182, 76, 226, 116, 10);
	ADD(17, 4, 160, 238, 56, 246, 3);
	ADD(177, 255, 108, 231, 113, 62, 2);
	ADD(236, 170, 219, 80, 15, 185, 2);
	ADD(97, 51, 98, 238, 74, 150, 7);
	ADD(215, 59, 48, 26, 123, 100, 1);
	ADD(133, 250, 209, 158, 60, 61, 5);
	ADD(3, 7, 66, 47, 235, 133, 4);
	ADD(188, 85, 131, 105, 185, 90, 2);
	ADD(65, 233, 115, 104, 165, 30, 9);
	ADD(22, 89, 33, 85, 129, 184, 1);
	ADD(176, 243, 97, 246, 142, 166, 4);
	ADD(11, 70, 45, 211, 97, 136, 4);
	ADD(131, 242, 110, 30, 46, 2, 6);
	ADD(90, 12, 60, 197, 111, 216, 5);
	ADD(8, 191, 75, 57, 224, 89, 6);
	ADD(176, 67, 168, 81, 154, 158, 7);
	ADD(102, 133, 133, 151, 242, 238, 5);
	ADD(184, 32, 43, 51, 139, 245, 10);
	ADD(223, 193, 234, 7, 153, 17, 3);
	DEST();
	CHECK_MALLOC();

	print_test("One hundred empty models");
	/* LIKE A BEAUTY PAGEANT */
	// Hey, that wasn't nice.  A lot of pageants have many entrants who are
	// genuinely intelligent and not jus--
	/* OH SHUT UP DOUBLE-SLASH GO BACK TO WRITING CODE */
	for (int i = 0; i < 100; i++)
		INIT_M(&mods[i], "the blank");
	for (int i = 0; i < 100; i++)
		DEST_M(&mods[i]);
	CHECK_MALLOC();

	print_test("One hundred models, two chunks apiece");
	for (int i = 0; i < 100; i++) {
		INIT_M(&mods[i], "100 x 2");
		ADD_M(&mods[i], 50, 56, 9, 181, 162, 33, 8);
		ADD_M(&mods[i], 170, 37, 135, 41, 70, 174, 7);
	}
	for (int i = 0; i < 100; i++)
		DEST_M(&mods[i]);
	CHECK_MALLOC();

	return 0;
}

#define LOAD_M(model, fn) do { \
		if (model_load((model), (fn))) { \
			fprintf(stderr, "model_load failed for good file\n"); \
			return 1; \
		} \
	} while (0)
#define LOAD(...) LOAD_M(&model, __VA_ARGS__)

#define LOAD_ERR_M(model, fn) do { \
		if (!model_load((model), (fn))) { \
			fprintf(stderr, "model_load succeeded for bad file\n"); \
			model_destroy((model)); \
			return 1; \
		} \
	} while (0)
#define LOAD_ERR(...) LOAD_ERR_M(&model, __VA_ARGS__)

#define CHECK_NFILES() do { \
		nf = num_files(); \
		if (nf > starting_nf) { \
			fprintf(stderr, "did not close opened file(s)\n"); \
			return 1; \
		} else if (nf < starting_nf) { \
			fprintf(stderr, "closed too many files??\n"); \
			return 1; \
		} \
	} while (0)

/* YOU ARE REALLY KNEE-DEEP IN THIS PREPROCESSOR CRAP MAN */
/* HAVE YOU CONSIDERED TALKING TO SOMEONE ABOUT THAT */

#define CHECK_MALLOC_ERR() do { \
		if (skipmem) \
			break; \
		mi = mallinfo(); \
		if (mi.uordblks > bytes) { \
			fprintf(stderr, "did not deallocate all memory on error\n"); \
			return 1; \
		} else if (mi.uordblks < bytes) { \
			fprintf(stderr, "freed too much memory on error??\n"); \
			return 1; \
		} \
	} while (0)
/* APPARENTLY NOT */

static int verify_load()
{
	struct model model;
	struct chunk chunk;
	char fname[MAX_FNAME_LEN];

	// Get currently allocated bytes
	struct mallinfo mi = mallinfo();
	int bytes = mi.uordblks;

	// Get currently open files
	int nf;
	int starting_nf = num_files();
	if (starting_nf < 0)
		return -1;

	if (skipmem)
		fprintf(stderr, "\e[38;5;1m !!! WARNING: \e[0mSkipping memory allocation checks.\n"
				"              Run with -M only when using Valgrind.\n");

	// Test legitimate models
	for (int i = 0; i < NUM_GOOD_MODELS; i++) {
		snprintf(fname, MAX_FNAME_LEN, "models/good%02d.3dm", i);
		fname[MAX_FNAME_LEN-1] = '\0';
		print_test(fname);
		INIT("this part doesn't really matter all that much but it's probably worth making it a long string just to catch some bugs or something you know like fireflies oh summer i miss you");
		LOAD(fname);
		CHECK_NFILES();
		CHECK(&good_model[i]);
		DEST();
		CHECK_MALLOC();
	}

	// Load+add
	print_test("models/good02.3dm, plus two more chunks");
	INIT("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.");
	LOAD("models/good02.3dm");
	CHECK_NFILES();
	ADD(152, 3, 0, 5, 1, 6, 2);
	ADD(2, 153, 0, 3, 1, 3, 9);
	CHECK(&load_add);
	DEST();
	CHECK_MALLOC();

	// Test error models
	for (int i = 0; i < NUM_ERROR_MODELS; i++) {
		snprintf(fname, MAX_FNAME_LEN, "models/error%02d.3dm", i);
		fname[MAX_FNAME_LEN-1] = '\0';
		print_test(fname);
		INIT("this part still doesn't really matter all that much but it's probably worth making it a long string just to catch some bugs or something you know like fireflies oh summer i miss you");
		LOAD_ERR(fname);
		CHECK_NFILES();
		CHECK_MALLOC_ERR();
	}

	return 0;
}

#define TT_INIT_P(tt, ...) do { \
		internal_set_dims(__VA_ARGS__); \
		if (tiptap_init((tt))) { \
			fprintf(stderr, "failed to initialize TipTap\n"); \
			return 1; \
		} \
	} while (0)
#define TT_INIT(...) TT_INIT_P(&tt, __VA_ARGS__)

#define TT_DEST_P(tt) do { \
		tiptap_destroy((tt)); \
	} while (0)
#define TT_DEST() TT_DEST_P(&tt)

#define PRINT(...) do { \
		if (model_print(&model, &tt, __VA_ARGS__)) { \
			fprintf(stderr, "model_print returned failure for good model\n"); \
			return 1; \
		} \
	} while (0)

#define PRINT_ERR(...) do { \
		if (!model_print(&model, &tt, __VA_ARGS__)) { \
			fprintf(stderr, "model_print returned success for bad parameters/model\n"); \
			return 1; \
		} \
	} while (0)

#define VDAT(str) do { \
		uint8_t sha[20]; \
		sscanf((str), "%02hhx%02hhx%02hhx%02hhx%02hhx" \
				"%02hhx%02hhx%02hhx%02hhx%02hhx" \
				"%02hhx%02hhx%02hhx%02hhx%02hhx" \
				"%02hhx%02hhx%02hhx%02hhx%02hhx", \
				sha, sha + 1, sha + 2, sha + 3, sha + 4, \
				sha + 5, sha + 6, sha + 7, sha + 8, sha + 9, \
				sha + 10, sha + 11, sha + 12, sha + 13, sha + 14, \
				sha + 15, sha + 16, sha + 17, sha + 18, sha + 19); \
		if (internal_verify_data(sha)) { \
			fprintf(stderr, "model_print did not print model correctly\n"); \
			return 1; \
		} \
	} while (0)


#define FOUR_PRINT(tl, tr, bl, br, fit, four) do { \
		TT_INIT(model.width + 1, model.height + 1, model.depth + 1); \
		PRINT(0, 0); \
		VDAT((tl)); \
		TT_DEST(); \
		print_test("... in top-right"); \
		TT_INIT(model.width + 1, model.height + 1, model.depth + 1); \
		PRINT(1, 0); \
		VDAT((tr)); \
		TT_DEST(); \
		print_test("... in bottom-left"); \
		TT_INIT(model.width + 1, model.height + 1, model.depth + 1); \
		PRINT(0, 1); \
		VDAT((bl)); \
		TT_DEST(); \
		print_test("... in bottom-right"); \
		TT_INIT(model.width + 1, model.height + 1, model.depth + 1); \
		PRINT(1, 1); \
		VDAT((br)); \
		TT_DEST(); \
		print_test("... on printer of exact width/height"); \
		TT_INIT(model.width, model.height, model.depth + 1); \
		PRINT(0, 0); \
		VDAT((fit)); \
		TT_DEST(); \
		print_test("... four copies, two by two"); \
		TT_INIT(model.width * 2, model.height * 2, model.depth + 1); \
		PRINT(0, 0); \
		PRINT(model.width, 0); \
		PRINT(0, model.height); \
		PRINT(model.width, model.height); \
		VDAT((four)); \
		TT_DEST(); \
	} while (0)
/* ... THAT WAS TERRIFYING DUDE */
/* WHEN ARE YOU GOING TO FIND A GIRL OR GET A LIFE OR SOMETHING */
/* SERIOUSLY MAN THIS IS GETTING OUT OF HAND */

static int verify_print()
{
	struct model model;
	struct chunk chunk;
	struct tiptap tt;


	print_test("One chunk at (0,0,0) in model, printed in top-left");
	INIT("john jacob jingleheimer-schmitt, his name is my name too.  whenever we go out, the people always shout, 'there goes john jacob jingleheimer-schmitt.'  DAA DAA DAA DAA DAA DAA DAA");
	/* YO WHAT DOES THAT SONG HAVE TO DO WITH ANYTHING */
	// Just had it stuck in my head is all, searching for a long title for
	// the model and--
	/* HOW OLD ARE YOU LIKE EIGHT */
	
	ADD(0, 0, 0, 5, 4, 3, 12);
	FOUR_PRINT("ca476a4be77e37d0af8a6079f55c9b680626b29f",
			"77b252746417cf44cb1615225b580e1499100eb8",
			"c0d035daa4356f718f06ed0f88f4c38eb6d9b1d2",
			"99f4847d58bb3fe248bbd343087ab4faeba8b5fa",
			"93cd4f8fa0d5a30a80a179d286bdfc91a7e3efc9",
			"25f90972ac43bb332ad548ed8ad9d43f89072878");
	DEST();
	// (right, like you're one to speak)
	
	print_test("One chunk not at (0,0,0) in model, printed in top-left");
	INIT("john jacob jingleheimer-schmitt, his name is my name too.  whenever we go out, the people always shout, 'there goes john jacob jingleheimer-schmitt.'  DAA DAA DAA DAA DAA DAA DAA");
	/* I HEARD THAT */
	ADD(2, 4, 0, 5, 4, 3, 15);
	FOUR_PRINT("a0f7b9d4e60490760ec11d856e66a7780c169d3f",
			"1fa278b1edf3f8ab93e7cfd2e424f0379fd7b180",
			"fd3ae34f06072ba350ea82e530273ca4daf36f11",
			"bba24b1ae8c575cc847a3675212a9eb2109a2e4c",
			"7c6aa034f82c11c849038f5b19d3c8d30bd8e9fb",
			"60c90f1481f565a3fcf71e28b77070d0ac6f325a");
	DEST();

	print_test("One chunk, printed out of bounds");
	INIT("JJJS HNIMNT WWGO TPAS TGJJJS DDDDDDD~~~");
	ADD(2, 4, 0, 5, 4, 3, 15);
	TT_INIT(10, 10, 10);
	PRINT_ERR(15, 0);
	TT_DEST();
	DEST();

	print_test("One chunk, printed out of bounds");
	INIT("JJJS HNIMNT WWGO TPAS TGJJJS DDDDDDD~~~");
	ADD(2, 4, 0, 5, 4, 3, 15);
	TT_INIT(10, 10, 10);
	PRINT_ERR(0, 15);
	TT_DEST();
	DEST();

	print_test("One chunk, extending out of bounds");
	INIT("JJJS HNIMNT WWGO TPAS TGJJJS DDDDDDD~~~");
	ADD(2, 4, 0, 5, 4, 3, 15);
	TT_INIT(10, 10, 10);
	PRINT_ERR(5, 0);
	TT_DEST();
	DEST();

	print_test("One chunk, extending out of bounds");
	INIT("JJJS HNIMNT WWGO TPAS TGJJJS DDDDDDD~~~");
	ADD(2, 4, 0, 5, 4, 3, 15);
	TT_INIT(10, 10, 10);
	PRINT_ERR(0, 5);
	TT_DEST();
	DEST();

	print_test("Hardware failure handling");
	INIT("<>!*''# ^\"`$$- !*=@$_ %*<>~#4 &[]../ |{,,SYSTEM HALTED");
	ADD(2, 4, 0, 5, 4, 3, 15);
	TT_INIT(10, 10, 10);
	internal_hw_failure();
	PRINT_ERR(0, 5);
	TT_DEST();
	DEST();

	print_test("Chunk inside a chunk, printed in top-left");
	INIT("jingle(jingle(jingleheimer-schmitt)heimer-schmitt)heimer-schmitt");
	ADD(0, 0, 0, 8, 8, 8, 1);
	ADD(2, 2, 2, 4, 4, 4, 5);
	FOUR_PRINT("19a633ea5d82a6f00ff33ef8a66b3f9e0728f5e9",
			"5175a3f3ea19b4b83a02e651ae624f1483444ef2",
			"f6cc05216229f42f4c97ea03aa1f6ea005dfe1e5",
			"37bdc8cd3e3dcc817d8b3d80cf42b47c0396632f",
			"fb9bbebdfca34de3bd0f9a1819d4064155de0a35",
			"f25d8c165534c75295180190044162d4d3dd39ca");
	DEST();

	print_test("Two separate chunks, printed in top-left");
	INIT("foo");
	ADD(1, 1, 0, 2, 3, 4, 3);
	ADD(6, 7, 0, 4, 4, 4, 8);
	FOUR_PRINT("28e209a12abcbedbb1627d0eb0b792d99a988ddf",
			"3e6760575c32b1b188da17fc27a6f9989d5da31a",
			"2df902295df6c3ee3bcc5e9dd06cc369de1d2af3",
			"ef76ddc2de6c0c0d508c87db90a062d2656da9d2",
			"8b1b5a29b99afe4a5b9979577bf16113838c5eda",
			"d2f40562fc183815ec964eeb8673b204da107a1e");
	DEST();

	print_test("Two separate chunks, different depths, printed in top-left");
	INIT("oof");
	ADD(0, 0, 0, 2, 3, 6, 3);
	ADD(5, 6, 0, 4, 4, 4, 8);
	FOUR_PRINT("292d01c2a5643dcdb9d0dc97614c2572c9735d27",
			"590108fb258616123f2b6538377360e414ebfbc4",
			"f115a3beeb520fb0d3565e239152a6939030d002",
			"1c213792950871d8567642de2562665f0aefd400",
			"f808d48cb10d69ce7d390d9e0c72dfa79acde859",
			"5b653f7cfe810d4ca89a6027a5013d7778261d7d");
	DEST();

	print_test("Two chunks, same location, printed in top-left");
	INIT("bar");
	ADD(2, 3, 0, 5, 1, 6, 2);
	ADD(2, 3, 0, 3, 2, 3, 9);
	FOUR_PRINT("17f726205ece95f6e0f8b23f542377ebcd788d08",
			"9e9c1c871f47cd49828926779940d8f9ab558e29",
			"e382765b4ccec7ee7a9662c92133d82c6f885765",
			"def096c7ea0a1db7705d96af6f842c37b55e5f3f",
			"d5a7718d885105255b3f36ae00083912171f76c0",
			"f1688f095a15d7128b6e3179f9024d4cf47fe8dd");
	DEST();

	print_test("Ten random chunks with base, printed in top-left");
	INIT("OARUS.8XKHESHUSRCOE!!!!1");
	/* SYNTAX ERROR PROGRAMMER IS DUMB AND LET GO OF THE SHIFT KEY */
	ADD(69, 73, 18, 59, 36, 76, 4);
	ADD(10, 10, 0, 150, 150, 50, 15);
	ADD(47, 116, 46, 111, 24, 80, 8);
	// So, uh, technically that has nothing to do with syntax
	ADD(89, 61, 28, 30, 90, 77, 14);
	/* YOUR FACE HAS SYNTAX */
	ADD(74, 118, 2, 85, 13, 41, 7);
	// ...
	ADD(21, 41, 28, 42, 76, 18, 5);
	ADD(60, 13, 26, 17, 57, 18, 10);
	ADD(110, 54, 25, 1, 13, 27, 4);
	ADD(42, 130, 5, 20, 19, 96, 3);
	ADD(47, 16, 32, 38, 107, 32, 6);
	// I... I don't even.
	ADD(16, 33, 18, 110, 102, 25, 8);
	FOUR_PRINT("1f935f69eda85834ad61daf77b75d1d9da1b99f1",
			"88e29cb118150ae3e0007c1655e3d118ac59da73",
			"2832c3ca95be2de08421cbc5ef98281983b4f973",
			"867ca47a3ac7f5a0eb9eaf2582313da0c0b3b1f1",
			"4bc2e90954a7e14a443101815286c7f01c81917b",
			"04bd857d30448fae673b215ad56a49eda249fedc");
	DEST();
	
	return 0;
}

static int verify_save()
{
	// Bonus function, no tests provided

	/* NOT GONNA HAPPEN EH DOUBLE-SLASH */
	/* WHATS THE BIG DEAL YOU BUSY OR SOMETHING */
	// Well I do kinda want to graduate someday, and I've got to do research
	// and publish papers to keep my adviser happy...
	/* PAPER SHMAPER GET ON THIS VERIFY FUNCTION */
	// You know, I don't have to take your crap bub.

	/* ... */

	/* ... */

	/* AHAHAHAHAHAA THATS GREAT TELL ANOTHER ONE */

	// ... (sigh) dude, why don't you go shout at /dev/null or something
	print_test("testing saving empty model");
	struct model model;
	model_init(&model, "scheistercoff");
	model_save(&model, "models/myModel.3dm");

	return 0;
}

#define VERIFY_FUNC(func) do { \
		print_hdr("Testing " #func "..."); \
		int ret = verify_##func(); \
		print_result(ret); \
		if (ret) \
			return ret; \
	} while(0)
#define NOTIMP(func) do { \
		print_hdr("Tests for " #func " in upcoming DLC..."); \
	} while(0)

int run_tests(void)
{
/*	VERIFY_FUNC(init);
	VERIFY_FUNC(add_chunk);
	VERIFY_FUNC(destroy);
	VERIFY_FUNC(load);
	VERIFY_FUNC(print);*/
	VERIFY_FUNC(save);	

	fprintf(stderr, "\e[38;5;9m-\e[38;5;11m= "
			"\e[38;5;10mA\e[38;5;14mL\e[38;5;12mL "
			"\e[38;5;13mP\e[38;5;12mA\e[38;5;14mS\e[38;5;10mS "
			"\e[38;5;11m=\e[38;5;9m-\e[0m\n\n");

	return 0;
}

static void usage(char *name)
{
	printf(
		"Usage: %s [OPTION]...\n"
		"Runs unit tests for the 3DM file format library.\n"
		"  -h         show this help\n"
		"  -v         output verbose log data to stdout\n"
		"  -o FILE    output log data to FILE\n"
		"  -M         suppresses memory allocation checks (use this when\n"
		"             running with Valgrind)\n",
		name);
}

#define LOG_OUTPUT(func, ...) \
	do { \
		if (logfile) func(logfile, __VA_ARGS__); \
		if (verbose) func(stdout, __VA_ARGS__); \
	} while(0);

int main(int argc, char **argv)
{
	int ret = 0;
	char *name = argv[0];
	fprintf(stderr, "Starting 3DM verifier version " A4VERIFY_VERSION "\n\n");

	// Option defaults
	int verbose = 0;
	char *logname = NULL;

	// Parse options
	int opt;
	while ((opt = getopt(argc, argv, "hvo:M")) != -1) {
		switch (opt) {
			case 'h':
				usage(name);
				return 0;
			case 'v':
				verbose = 1;
				break;
			case 'o':
				logname = optarg;
				break;
			case 'M':
				skipmem = 1;
				break;
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

	if (init_random())
		return 1;

	ret = run_tests();
	if (logfile)
		fclose(logfile);

	return ret;
}

/* YUP TOLD YOU NOTHING TO SEE NOBODY HERE BUT US CHICKENS */
