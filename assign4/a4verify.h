#ifndef A4VERIFY_H_
#define A4VERIFY_H_

#define MAX_FNAME_LEN 24

static const struct model empty_notitle = {
	.title = "",
	.width = 0, .height = 0, .depth = 0,
	.nchunks = 0,
	.chunks = NULL,
};

static const struct model empty_foo = {
	.title = "foo",
	.width = 0, .height = 0, .depth = 0,
	.nchunks = 0,
	.chunks = NULL,
};

static const struct model empty_lol = {
	.title = "trololololololololololololololololololololololololololololo"
		"lolololololololololololololololololololololololololololololo"
		"lolololololololololololololololololololololololololololololo"
		"lolololololololololololololololololololololololololololololo"
		"lolololololololo",
	.width = 0, .height = 0, .depth = 0,
	.nchunks = 0,
	.chunks = NULL,
};

static struct chunk add1chunk_chunks[] = {
	{
		.x = 0, .y = 0, .z = 0,
		.width = 3, .height = 4, .depth = 5,
		.mat = 1,
	},
};
static const struct model add1chunk = {
	.title = "foo",
	.width = 3, .height = 4, .depth = 5,
	.nchunks = 1,
	.chunks = add1chunk_chunks,
};
static struct chunk add1chunk_off_chunks[] = {
	{
		.x = 6, .y = 2, .z = 0,
		.width = 2, .height = 5, .depth = 7,
		.mat = 1,
	},
};
static const struct model add1chunk_off = {
	.title = "foo",
	.width = 8, .height = 7, .depth = 7,
	.nchunks = 1,
	.chunks = add1chunk_off_chunks,
};
static struct chunk add_2nested_chunks[] = {
	{
		.x = 0, .y = 0, .z = 0,
		.width = 8, .height = 8, .depth = 8,
		.mat = 1,
	},
	{
		.x = 2, .y = 2, .z = 2,
		.width = 4, .height = 4, .depth = 4,
		.mat = 5,
	},
};
static const struct model add_2nested = {
	.title = "fooception",
	.width = 8, .height = 8, .depth = 8,
	.nchunks = 2,
	.chunks = add_2nested_chunks,
};
static struct chunk add_2sep_chunks[] = {
	{
		.x = 0, .y = 0, .z = 0,
		.width = 2, .height = 3, .depth = 4,
		.mat = 3,
	},
	{
		.x = 5, .y = 6, .z = 0,
		.width = 4, .height = 4, .depth = 4,
		.mat = 8,
	},
};
static const struct model add_2sep = {
	.title = "foo",
	.width = 9, .height = 10, .depth = 4,
	.nchunks = 2,
	.chunks = add_2sep_chunks,
};
static struct chunk add_2sep_depth_chunks[] = {
	{
		.x = 0, .y = 0, .z = 0,
		.width = 2, .height = 3, .depth = 6,
		.mat = 3,
	},
	{
		.x = 5, .y = 6, .z = 0,
		.width = 4, .height = 4, .depth = 4,
		.mat = 8,
	},
};
static const struct model add_2sep_depth = {
	.title = "oof",
	.width = 9, .height = 10, .depth = 6,
	.nchunks = 2,
	.chunks = add_2sep_depth_chunks,
};
static struct chunk add_2coinc_chunks[] = {
	{
		.x = 2, .y = 3, .z = 0,
		.width = 5, .height = 1, .depth = 6,
		.mat = 2,
	},
	{
		.x = 2, .y = 3, .z = 0,
		.width = 3, .height = 1, .depth = 3,
		.mat = 9,
	},
};
static const struct model add_2coinc = {
	.title = "bar",
	.width = 7, .height = 4, .depth = 6,
	.nchunks = 2,
	.chunks = add_2coinc_chunks,
};
static struct chunk add_2vcoinc_chunks[] = {
	{
		.x = 2, .y = 3, .z = 0,
		.width = 5, .height = 1, .depth = 6,
		.mat = 2,
	},
	{
		.x = 2, .y = 3, .z = 0,
		.width = 5, .height = 1, .depth = 6,
		.mat = 9,
	},
};
static const struct model add_2vcoinc = {
	.title = "bar",
	.width = 7, .height = 4, .depth = 6,
	.nchunks = 2,
	.chunks = add_2vcoinc_chunks,
};
static struct chunk add_10random_chunks[] = {
	{
		.x = 209, .y = 73, .z = 168,
		.width = 159, .height = 36, .depth = 76,
		.mat = 4,
	},
	{
		.x = 147, .y = 216, .z = 46,
		.width = 11, .height = 24, .depth = 180,
		.mat = 8,
	},
	{
		.x = 189, .y = 61, .z = 178,
		.width = 230, .height = 90, .depth = 177,
		.mat = 14,
	},
	{
		.x = 74, .y = 118, .z = 102,
		.width = 85, .height = 243, .depth = 41,
		.mat = 7,
	},
	{
		.x = 121, .y = 91, .z = 28,
		.width = 242, .height = 76, .depth = 118,
		.mat = 5,
	},
	{
		.x = 60, .y = 103, .z = 86,
		.width = 17, .height = 67, .depth = 118,
		.mat = 10,
	},
	{
		.x = 210, .y = 254, .z = 125,
		.width = 101, .height = 103, .depth = 27,
		.mat = 4,
	},
	{
		.x = 242, .y = 170, .z = 205,
		.width = 220, .height = 159, .depth = 96,
		.mat = 3,
	},
	{
		.x = 147, .y = 16, .z = 232,
		.width = 238, .height = 207, .depth = 232,
		.mat = 6,
	},
	{
		.x = 216, .y = 33, .z = 68,
		.width = 110, .height = 132, .depth = 225,
		.mat = 8,
	},
};
static const struct model add_10random = {
	.title = "oarus.8xkheshusrcoe",
	.width = 462, .height = 361, .depth = 464,
	.nchunks = 10,
	.chunks = add_10random_chunks,
};
static const struct model init_strange = {
	.title = "~!@#$%^&*(){``\\\e\n\t\b\a__-=#FOO#=-__[@@@@@@               \t   hello\\",
	.width = 0, .height = 0, .depth = 0,
	.nchunks = 0,
	.chunks = NULL,
};
static struct chunk add_100random_chunks[] = {
	{
		.x = 112, .y = 208, .z = 99,
		.width = 5, .height = 51, .depth = 149,
		.mat = 8,
	},
	{
		.x = 145, .y = 4, .z = 148,
		.width = 167, .height = 117, .depth = 55,
		.mat = 6,
	},
	{
		.x = 162, .y = 12, .z = 254,
		.width = 24, .height = 253, .depth = 148,
		.mat = 10,
	},
	{
		.x = 18, .y = 140, .z = 55,
		.width = 169, .height = 104, .depth = 189,
		.mat = 6,
	},
	{
		.x = 142, .y = 59, .z = 212,
		.width = 98, .height = 103, .depth = 188,
		.mat = 1,
	},
	{
		.x = 247, .y = 162, .z = 2,
		.width = 12, .height = 147, .depth = 117,
		.mat = 7,
	},
	{
		.x = 87, .y = 48, .z = 157,
		.width = 221, .height = 243, .depth = 122,
		.mat = 9,
	},
	{
		.x = 209, .y = 144, .z = 237,
		.width = 60, .height = 116, .depth = 85,
		.mat = 7,
	},
	{
		.x = 117, .y = 50, .z = 30,
		.width = 1, .height = 119, .depth = 99,
		.mat = 6,
	},
	{
		.x = 16, .y = 196, .z = 217,
		.width = 134, .height = 121, .depth = 36,
		.mat = 4,
	},
	{
		.x = 192, .y = 180, .z = 6,
		.width = 79, .height = 224, .depth = 115,
		.mat = 7,
	},
	{
		.x = 87, .y = 69, .z = 255,
		.width = 245, .height = 94, .depth = 38,
		.mat = 2,
	},
	{
		.x = 128, .y = 3, .z = 222,
		.width = 21, .height = 230, .depth = 238,
		.mat = 10,
	},
	{
		.x = 239, .y = 181, .z = 62,
		.width = 142, .height = 52, .depth = 110,
		.mat = 9,
	},
	{
		.x = 199, .y = 4, .z = 109,
		.width = 134, .height = 64, .depth = 15,
		.mat = 4,
	},
	{
		.x = 133, .y = 72, .z = 200,
		.width = 3, .height = 126, .depth = 1,
		.mat = 7,
	},
	{
		.x = 255, .y = 192, .z = 71,
		.width = 211, .height = 150, .depth = 62,
		.mat = 5,
	},
	{
		.x = 222, .y = 180, .z = 218,
		.width = 92, .height = 226, .depth = 131,
		.mat = 7,
	},
	{
		.x = 133, .y = 239, .z = 214,
		.width = 130, .height = 225, .depth = 147,
		.mat = 5,
	},
	{
		.x = 119, .y = 222, .z = 22,
		.width = 21, .height = 196, .depth = 144,
		.mat = 9,
	},
	{
		.x = 54, .y = 244, .z = 204,
		.width = 158, .height = 243, .depth = 246,
		.mat = 5,
	},
	{
		.x = 87, .y = 55, .z = 251,
		.width = 161, .height = 226, .depth = 173,
		.mat = 2,
	},
	{
		.x = 125, .y = 136, .z = 181,
		.width = 68, .height = 103, .depth = 156,
		.mat = 6,
	},
	{
		.x = 23, .y = 65, .z = 161,
		.width = 34, .height = 188, .depth = 46,
		.mat = 3,
	},
	{
		.x = 151, .y = 147, .z = 254,
		.width = 118, .height = 173, .depth = 243,
		.mat = 1,
	},
	{
		.x = 93, .y = 119, .z = 10,
		.width = 185, .height = 184, .depth = 194,
		.mat = 1,
	},
	{
		.x = 16, .y = 35, .z = 42,
		.width = 146, .height = 197, .depth = 68,
		.mat = 5,
	},
	{
		.x = 120, .y = 177, .z = 185,
		.width = 136, .height = 143, .depth = 14,
		.mat = 8,
	},
	{
		.x = 146, .y = 73, .z = 109,
		.width = 208, .height = 45, .depth = 147,
		.mat = 6,
	},
	{
		.x = 2, .y = 12, .z = 150,
		.width = 118, .height = 237, .depth = 128,
		.mat = 5,
	},
	{
		.x = 71, .y = 221, .z = 96,
		.width = 183, .height = 206, .depth = 25,
		.mat = 6,
	},
	{
		.x = 241, .y = 149, .z = 161,
		.width = 89, .height = 154, .depth = 29,
		.mat = 5,
	},
	{
		.x = 66, .y = 164, .z = 112,
		.width = 237, .height = 104, .depth = 226,
		.mat = 5,
	},
	{
		.x = 50, .y = 56, .z = 9,
		.width = 181, .height = 162, .depth = 33,
		.mat = 8,
	},
	{
		.x = 170, .y = 37, .z = 135,
		.width = 41, .height = 70, .depth = 174,
		.mat = 7,
	},
	{
		.x = 141, .y = 172, .z = 131,
		.width = 175, .height = 87, .depth = 111,
		.mat = 5,
	},
	{
		.x = 73, .y = 180, .z = 12,
		.width = 225, .height = 206, .depth = 18,
		.mat = 1,
	},
	{
		.x = 9, .y = 213, .z = 21,
		.width = 218, .height = 43, .depth = 199,
		.mat = 8,
	},
	{
		.x = 86, .y = 138, .z = 44,
		.width = 84, .height = 62, .depth = 13,
		.mat = 5,
	},
	{
		.x = 78, .y = 21, .z = 108,
		.width = 193, .height = 168, .depth = 239,
		.mat = 1,
	},
	{
		.x = 86, .y = 77, .z = 152,
		.width = 228, .height = 180, .depth = 208,
		.mat = 3,
	},
	{
		.x = 91, .y = 222, .z = 170,
		.width = 229, .height = 196, .depth = 113,
		.mat = 10,
	},
	{
		.x = 135, .y = 43, .z = 28,
		.width = 253, .height = 3, .depth = 209,
		.mat = 7,
	},
	{
		.x = 74, .y = 101, .z = 161,
		.width = 246, .height = 86, .depth = 95,
		.mat = 1,
	},
	{
		.x = 154, .y = 52, .z = 142,
		.width = 242, .height = 154, .depth = 17,
		.mat = 4,
	},
	{
		.x = 38, .y = 104, .z = 152,
		.width = 73, .height = 215, .depth = 33,
		.mat = 10,
	},
	{
		.x = 37, .y = 117, .z = 102,
		.width = 126, .height = 97, .depth = 231,
		.mat = 6,
	},
	{
		.x = 82, .y = 113, .z = 189,
		.width = 98, .height = 133, .depth = 59,
		.mat = 3,
	},
	{
		.x = 180, .y = 98, .z = 255,
		.width = 138, .height = 171, .depth = 155,
		.mat = 7,
	},
	{
		.x = 10, .y = 198, .z = 186,
		.width = 173, .height = 191, .depth = 150,
		.mat = 8,
	},
	{
		.x = 142, .y = 151, .z = 65,
		.width = 141, .height = 77, .depth = 129,
		.mat = 1,
	},
	{
		.x = 12, .y = 110, .z = 163,
		.width = 70, .height = 166, .depth = 223,
		.mat = 4,
	},
	{
		.x = 15, .y = 192, .z = 235,
		.width = 201, .height = 65, .depth = 41,
		.mat = 4,
	},
	{
		.x = 28, .y = 64, .z = 44,
		.width = 139, .height = 83, .depth = 223,
		.mat = 8,
	},
	{
		.x = 43, .y = 58, .z = 22,
		.width = 3, .height = 13, .depth = 71,
		.mat = 1,
	},
	{
		.x = 26, .y = 218, .z = 185,
		.width = 108, .height = 228, .depth = 164,
		.mat = 2,
	},
	{
		.x = 179, .y = 147, .z = 178,
		.width = 35, .height = 83, .depth = 107,
		.mat = 2,
	},
	{
		.x = 199, .y = 199, .z = 218,
		.width = 28, .height = 197, .depth = 58,
		.mat = 1,
	},
	{
		.x = 190, .y = 207, .z = 193,
		.width = 73, .height = 222, .depth = 33,
		.mat = 4,
	},
	{
		.x = 167, .y = 239, .z = 55,
		.width = 97, .height = 136, .depth = 160,
		.mat = 10,
	},
	{
		.x = 39, .y = 145, .z = 131,
		.width = 150, .height = 202, .depth = 58,
		.mat = 10,
	},
	{
		.x = 153, .y = 8, .z = 103,
		.width = 114, .height = 20, .depth = 105,
		.mat = 6,
	},
	{
		.x = 196, .y = 239, .z = 156,
		.width = 164, .height = 162, .depth = 50,
		.mat = 2,
	},
	{
		.x = 233, .y = 51, .z = 65,
		.width = 69, .height = 56, .depth = 153,
		.mat = 8,
	},
	{
		.x = 185, .y = 188, .z = 35,
		.width = 162, .height = 36, .depth = 172,
		.mat = 8,
	},
	{
		.x = 75, .y = 68, .z = 62,
		.width = 186, .height = 192, .depth = 218,
		.mat = 10,
	},
	{
		.x = 97, .y = 52, .z = 38,
		.width = 223, .height = 227, .depth = 233,
		.mat = 3,
	},
	{
		.x = 21, .y = 39, .z = 10,
		.width = 247, .height = 205, .depth = 164,
		.mat = 8,
	},
	{
		.x = 11, .y = 82, .z = 240,
		.width = 44, .height = 200, .depth = 105,
		.mat = 7,
	},
	{
		.x = 177, .y = 178, .z = 126,
		.width = 148, .height = 158, .depth = 86,
		.mat = 4,
	},
	{
		.x = 205, .y = 31, .z = 48,
		.width = 183, .height = 90, .depth = 28,
		.mat = 3,
	},
	{
		.x = 50, .y = 161, .z = 6,
		.width = 90, .height = 131, .depth = 218,
		.mat = 4,
	},
	{
		.x = 226, .y = 141, .z = 156,
		.width = 68, .height = 120, .depth = 216,
		.mat = 8,
	},
	{
		.x = 28, .y = 248, .z = 32,
		.width = 4, .height = 144, .depth = 20,
		.mat = 3,
	},
	{
		.x = 60, .y = 98, .z = 86,
		.width = 212, .height = 13, .depth = 239,
		.mat = 5,
	},
	{
		.x = 15, .y = 235, .z = 161,
		.width = 123, .height = 214, .depth = 87,
		.mat = 6,
	},
	{
		.x = 172, .y = 224, .z = 146,
		.width = 237, .height = 208, .depth = 70,
		.mat = 3,
	},
	{
		.x = 40, .y = 224, .z = 170,
		.width = 161, .height = 44, .depth = 134,
		.mat = 9,
	},
	{
		.x = 11, .y = 170, .z = 139,
		.width = 188, .height = 253, .depth = 52,
		.mat = 8,
	},
	{
		.x = 146, .y = 92, .z = 94,
		.width = 160, .height = 29, .depth = 73,
		.mat = 7,
	},
	{
		.x = 250, .y = 171, .z = 182,
		.width = 76, .height = 226, .depth = 116,
		.mat = 10,
	},
	{
		.x = 17, .y = 4, .z = 160,
		.width = 238, .height = 56, .depth = 246,
		.mat = 3,
	},
	{
		.x = 177, .y = 255, .z = 108,
		.width = 231, .height = 113, .depth = 62,
		.mat = 2,
	},
	{
		.x = 236, .y = 170, .z = 219,
		.width = 80, .height = 15, .depth = 185,
		.mat = 2,
	},
	{
		.x = 97, .y = 51, .z = 98,
		.width = 238, .height = 74, .depth = 150,
		.mat = 7,
	},
	{
		.x = 215, .y = 59, .z = 48,
		.width = 26, .height = 123, .depth = 100,
		.mat = 1,
	},
	{
		.x = 133, .y = 250, .z = 209,
		.width = 158, .height = 60, .depth = 61,
		.mat = 5,
	},
	{
		.x = 3, .y = 7, .z = 66,
		.width = 47, .height = 235, .depth = 133,
		.mat = 4,
	},
	{
		.x = 188, .y = 85, .z = 131,
		.width = 105, .height = 185, .depth = 90,
		.mat = 2,
	},
	{
		.x = 65, .y = 233, .z = 115,
		.width = 104, .height = 165, .depth = 30,
		.mat = 9,
	},
	{
		.x = 22, .y = 89, .z = 33,
		.width = 85, .height = 129, .depth = 184,
		.mat = 1,
	},
	{
		.x = 176, .y = 243, .z = 97,
		.width = 246, .height = 142, .depth = 166,
		.mat = 4,
	},
	{
		.x = 11, .y = 70, .z = 45,
		.width = 211, .height = 97, .depth = 136,
		.mat = 4,
	},
	{
		.x = 131, .y = 242, .z = 110,
		.width = 30, .height = 46, .depth = 2,
		.mat = 6,
	},
	{
		.x = 90, .y = 12, .z = 60,
		.width = 197, .height = 111, .depth = 216,
		.mat = 5,
	},
	{
		.x = 8, .y = 191, .z = 75,
		.width = 57, .height = 224, .depth = 89,
		.mat = 6,
	},
	{
		.x = 176, .y = 67, .z = 168,
		.width = 81, .height = 154, .depth = 158,
		.mat = 7,
	},
	{
		.x = 102, .y = 133, .z = 133,
		.width = 151, .height = 242, .depth = 238,
		.mat = 5,
	},
	{
		.x = 184, .y = 32, .z = 43,
		.width = 51, .height = 139, .depth = 245,
		.mat = 10,
	},
	{
		.x = 223, .y = 193, .z = 234,
		.width = 7, .height = 153, .depth = 17,
		.mat = 3,
	},
};
static const struct model add_100random = {
	.title = "gad.rsual.guichasroahnud,jmgy,rc.l smuoae sguadstoeg,.uh asoerusaoerchpy,.",
	.width = 466, .height = 487, .depth = 497,
	.nchunks = 100,
	.chunks = add_100random_chunks,
};
static struct chunk load_add_chunks[] = {
	{
		.x = 10, .y = 10, .z = 10,
		.width = 5, .height = 5, .depth = 6,
		.mat = 1,
	},
	{
		.x = 100, .y = 100, .z = 100,
		.width = 5, .height = 5, .depth = 6,
		.mat = 3,
	},
	{
		.x = 20, .y = 20, .z = 20,
		.width = 3, .height = 5, .depth = 6,
		.mat = 2,
	},
	{
		.x = 15, .y = 15, .z = 15,
		.width = 5, .height = 5, .depth = 6,
		.mat = 2,
	},
	{
		.x = 75, .y = 34, .z = 30,
		.width = 5, .height = 7, .depth = 6,
		.mat = 1,
	},
	{
		.x = 152, .y = 3, .z = 0,
		.width = 5, .height = 1, .depth = 6,
		.mat = 2,
	},
	{
		.x = 2, .y = 153, .z = 0,
		.width = 3, .height = 1, .depth = 3,
		.mat = 9,
	},
};
static const struct model load_add = {
	.title = "PrintJob2",
	.width = 157, .height = 154, .depth = 106,
	.nchunks = 7,
	.chunks = load_add_chunks,
};

#define NUM_GOOD_MODELS 8
#define NUM_ERROR_MODELS 12
static struct chunk good01_chunks[] = {
	{
		.x = 10, .y = 10, .z = 10,
		.width = 5, .height = 5, .depth = 6,
		.mat = 1,
	},
};
static struct chunk good02_chunks[] = {
	{
		.x = 10, .y = 10, .z = 10,
		.width = 5, .height = 5, .depth = 6,
		.mat = 1,
	},
	{
		.x = 100, .y = 100, .z = 100,
		.width = 5, .height = 5, .depth = 6,
		.mat = 3,
	},
	{
		.x = 20, .y = 20, .z = 20,
		.width = 3, .height = 5, .depth = 6,
		.mat = 2,
	},
	{
		.x = 15, .y = 15, .z = 15,
		.width = 5, .height = 5, .depth = 6,
		.mat = 2,
	},
	{
		.x = 75, .y = 34, .z = 30,
		.width = 5, .height = 7, .depth = 6,
		.mat = 1,
	},
};
static struct chunk good03_chunks[] = {
	{
		.x = 254, .y = 244, .z = 448,
		.width = 10, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 15, .y = 78, .z = 165,
		.width = 2, .height = 2, .depth = 3,
		.mat = 1,
	},
	{
		.x = 108, .y = 278, .z = 82,
		.width = 10, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 206, .y = 147, .z = 311,
		.width = 11, .height = 4, .depth = 4,
		.mat = 3,
	},
	{
		.x = 105, .y = 201, .z = 429,
		.width = 4, .height = 3, .depth = 3,
		.mat = 2,
	},
	{
		.x = 52, .y = 151, .z = 464,
		.width = 6, .height = 3, .depth = 5,
		.mat = 1,
	},
	{
		.x = 63, .y = 81, .z = 204,
		.width = 4, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 188, .y = 55, .z = 378,
		.width = 7, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 207, .y = 201, .z = 291,
		.width = 7, .height = 3, .depth = 4,
		.mat = 2,
	},
	{
		.x = 231, .y = 244, .z = 254,
		.width = 5, .height = 4, .depth = 2,
		.mat = 3,
	},
	{
		.x = 125, .y = 151, .z = 217,
		.width = 7, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 191, .y = 257, .z = 250,
		.width = 10, .height = 2, .depth = 4,
		.mat = 1,
	},
	{
		.x = 61, .y = 72, .z = 500,
		.width = 6, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 31, .y = 72, .z = 253,
		.width = 2, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 63, .y = 168, .z = 16,
		.width = 8, .height = 3, .depth = 2,
		.mat = 2,
	},
	{
		.x = 196, .y = 267, .z = 275,
		.width = 8, .height = 3, .depth = 3,
		.mat = 3,
	},
	{
		.x = 116, .y = 196, .z = 150,
		.width = 3, .height = 2, .depth = 5,
		.mat = 2,
	},
	{
		.x = 134, .y = 260, .z = 394,
		.width = 7, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 78, .y = 114, .z = 477,
		.width = 3, .height = 3, .depth = 6,
		.mat = 2,
	},
	{
		.x = 48, .y = 19, .z = 14,
		.width = 5, .height = 4, .depth = 6,
		.mat = 1,
	},
	{
		.x = 165, .y = 297, .z = 319,
		.width = 3, .height = 2, .depth = 3,
		.mat = 2,
	},
	{
		.x = 229, .y = 202, .z = 193,
		.width = 6, .height = 3, .depth = 2,
		.mat = 3,
	},
	{
		.x = 143, .y = 158, .z = 197,
		.width = 10, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 58, .y = 274, .z = 422,
		.width = 8, .height = 2, .depth = 2,
		.mat = 1,
	},
	{
		.x = 91, .y = 132, .z = 451,
		.width = 9, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 104, .y = 82, .z = 230,
		.width = 4, .height = 4, .depth = 2,
		.mat = 1,
	},
	{
		.x = 120, .y = 129, .z = 193,
		.width = 3, .height = 4, .depth = 2,
		.mat = 2,
	},
	{
		.x = 13, .y = 106, .z = 395,
		.width = 4, .height = 3, .depth = 4,
		.mat = 3,
	},
	{
		.x = 249, .y = 97, .z = 25,
		.width = 9, .height = 3, .depth = 4,
		.mat = 2,
	},
	{
		.x = 217, .y = 110, .z = 95,
		.width = 7, .height = 2, .depth = 3,
		.mat = 1,
	},
	{
		.x = 231, .y = 153, .z = 161,
		.width = 6, .height = 2, .depth = 4,
		.mat = 2,
	},
	{
		.x = 96, .y = 99, .z = 480,
		.width = 3, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 229, .y = 116, .z = 264,
		.width = 2, .height = 2, .depth = 2,
		.mat = 2,
	},
	{
		.x = 92, .y = 20, .z = 148,
		.width = 9, .height = 4, .depth = 3,
		.mat = 3,
	},
	{
		.x = 95, .y = 123, .z = 225,
		.width = 6, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 135, .y = 182, .z = 60,
		.width = 2, .height = 4, .depth = 2,
		.mat = 1,
	},
	{
		.x = 16, .y = 43, .z = 364,
		.width = 3, .height = 2, .depth = 3,
		.mat = 2,
	},
	{
		.x = 11, .y = 285, .z = 437,
		.width = 2, .height = 2, .depth = 6,
		.mat = 1,
	},
	{
		.x = 80, .y = 85, .z = 232,
		.width = 4, .height = 3, .depth = 5,
		.mat = 2,
	},
	{
		.x = 51, .y = 112, .z = 93,
		.width = 9, .height = 4, .depth = 6,
		.mat = 3,
	},
	{
		.x = 236, .y = 21, .z = 421,
		.width = 8, .height = 2, .depth = 5,
		.mat = 2,
	},
	{
		.x = 25, .y = 182, .z = 393,
		.width = 2, .height = 3, .depth = 4,
		.mat = 1,
	},
	{
		.x = 65, .y = 19, .z = 195,
		.width = 7, .height = 3, .depth = 5,
		.mat = 2,
	},
	{
		.x = 126, .y = 277, .z = 81,
		.width = 7, .height = 2, .depth = 3,
		.mat = 1,
	},
	{
		.x = 51, .y = 247, .z = 120,
		.width = 5, .height = 4, .depth = 5,
		.mat = 2,
	},
	{
		.x = 34, .y = 178, .z = 402,
		.width = 9, .height = 4, .depth = 5,
		.mat = 3,
	},
	{
		.x = 63, .y = 91, .z = 206,
		.width = 3, .height = 2, .depth = 2,
		.mat = 2,
	},
	{
		.x = 122, .y = 280, .z = 284,
		.width = 7, .height = 3, .depth = 6,
		.mat = 1,
	},
	{
		.x = 136, .y = 210, .z = 286,
		.width = 4, .height = 4, .depth = 6,
		.mat = 2,
	},
	{
		.x = 149, .y = 288, .z = 248,
		.width = 2, .height = 4, .depth = 2,
		.mat = 1,
	},
	{
		.x = 64, .y = 140, .z = 255,
		.width = 7, .height = 3, .depth = 5,
		.mat = 2,
	},
	{
		.x = 193, .y = 167, .z = 231,
		.width = 2, .height = 4, .depth = 6,
		.mat = 3,
	},
	{
		.x = 102, .y = 182, .z = 314,
		.width = 11, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 213, .y = 115, .z = 289,
		.width = 11, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 155, .y = 61, .z = 172,
		.width = 4, .height = 4, .depth = 3,
		.mat = 2,
	},
	{
		.x = 195, .y = 207, .z = 201,
		.width = 9, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 151, .y = 168, .z = 357,
		.width = 11, .height = 2, .depth = 2,
		.mat = 2,
	},
	{
		.x = 131, .y = 168, .z = 405,
		.width = 9, .height = 4, .depth = 4,
		.mat = 3,
	},
	{
		.x = 166, .y = 189, .z = 480,
		.width = 9, .height = 3, .depth = 6,
		.mat = 2,
	},
	{
		.x = 164, .y = 61, .z = 66,
		.width = 11, .height = 4, .depth = 4,
		.mat = 1,
	},
	{
		.x = 173, .y = 217, .z = 136,
		.width = 7, .height = 4, .depth = 2,
		.mat = 2,
	},
	{
		.x = 5, .y = 60, .z = 228,
		.width = 3, .height = 3, .depth = 6,
		.mat = 1,
	},
	{
		.x = 12, .y = 177, .z = 242,
		.width = 9, .height = 4, .depth = 2,
		.mat = 2,
	},
	{
		.x = 231, .y = 187, .z = 204,
		.width = 4, .height = 4, .depth = 6,
		.mat = 3,
	},
	{
		.x = 31, .y = 148, .z = 305,
		.width = 3, .height = 2, .depth = 5,
		.mat = 2,
	},
	{
		.x = 226, .y = 114, .z = 32,
		.width = 3, .height = 2, .depth = 6,
		.mat = 1,
	},
	{
		.x = 247, .y = 253, .z = 128,
		.width = 10, .height = 3, .depth = 6,
		.mat = 2,
	},
	{
		.x = 242, .y = 174, .z = 98,
		.width = 10, .height = 4, .depth = 3,
		.mat = 1,
	},
	{
		.x = 212, .y = 77, .z = 279,
		.width = 3, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 185, .y = 191, .z = 162,
		.width = 2, .height = 4, .depth = 4,
		.mat = 3,
	},
	{
		.x = 199, .y = 248, .z = 239,
		.width = 2, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 12, .y = 8, .z = 444,
		.width = 9, .height = 2, .depth = 4,
		.mat = 1,
	},
	{
		.x = 210, .y = 234, .z = 229,
		.width = 2, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 235, .y = 142, .z = 500,
		.width = 7, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 73, .y = 32, .z = 331,
		.width = 9, .height = 3, .depth = 3,
		.mat = 2,
	},
	{
		.x = 197, .y = 234, .z = 17,
		.width = 3, .height = 4, .depth = 3,
		.mat = 3,
	},
	{
		.x = 178, .y = 85, .z = 28,
		.width = 10, .height = 4, .depth = 4,
		.mat = 2,
	},
	{
		.x = 162, .y = 90, .z = 261,
		.width = 3, .height = 2, .depth = 3,
		.mat = 1,
	},
	{
		.x = 145, .y = 156, .z = 186,
		.width = 4, .height = 2, .depth = 6,
		.mat = 2,
	},
	{
		.x = 212, .y = 80, .z = 449,
		.width = 4, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 162, .y = 252, .z = 272,
		.width = 9, .height = 3, .depth = 3,
		.mat = 2,
	},
	{
		.x = 111, .y = 118, .z = 154,
		.width = 4, .height = 3, .depth = 5,
		.mat = 3,
	},
	{
		.x = 172, .y = 76, .z = 304,
		.width = 7, .height = 4, .depth = 3,
		.mat = 2,
	},
	{
		.x = 55, .y = 72, .z = 500,
		.width = 5, .height = 4, .depth = 5,
		.mat = 1,
	},
	{
		.x = 24, .y = 39, .z = 367,
		.width = 10, .height = 2, .depth = 5,
		.mat = 2,
	},
	{
		.x = 146, .y = 138, .z = 20,
		.width = 8, .height = 4, .depth = 6,
		.mat = 1,
	},
	{
		.x = 51, .y = 289, .z = 156,
		.width = 11, .height = 2, .depth = 3,
		.mat = 2,
	},
	{
		.x = 90, .y = 166, .z = 80,
		.width = 10, .height = 3, .depth = 5,
		.mat = 3,
	},
	{
		.x = 205, .y = 29, .z = 226,
		.width = 4, .height = 3, .depth = 2,
		.mat = 2,
	},
	{
		.x = 5, .y = 223, .z = 185,
		.width = 3, .height = 3, .depth = 2,
		.mat = 1,
	},
	{
		.x = 135, .y = 294, .z = 338,
		.width = 11, .height = 4, .depth = 6,
		.mat = 2,
	},
	{
		.x = 222, .y = 105, .z = 312,
		.width = 11, .height = 2, .depth = 3,
		.mat = 1,
	},
	{
		.x = 200, .y = 205, .z = 379,
		.width = 8, .height = 4, .depth = 2,
		.mat = 2,
	},
	{
		.x = 212, .y = 230, .z = 416,
		.width = 5, .height = 2, .depth = 3,
		.mat = 3,
	},
	{
		.x = 148, .y = 171, .z = 277,
		.width = 3, .height = 2, .depth = 5,
		.mat = 2,
	},
	{
		.x = 225, .y = 258, .z = 52,
		.width = 5, .height = 2, .depth = 2,
		.mat = 1,
	},
	{
		.x = 102, .y = 252, .z = 286,
		.width = 6, .height = 4, .depth = 3,
		.mat = 2,
	},
	{
		.x = 234, .y = 39, .z = 61,
		.width = 5, .height = 3, .depth = 6,
		.mat = 1,
	},
	{
		.x = 239, .y = 73, .z = 473,
		.width = 11, .height = 2, .depth = 2,
		.mat = 2,
	},
	{
		.x = 226, .y = 297, .z = 49,
		.width = 10, .height = 4, .depth = 4,
		.mat = 3,
	},
};
static struct chunk good06_chunks[] = {
	{
		.x = 72, .y = 2373, .z = 2380,
		.width = 2380, .height = 2383, .depth = 6,
		.mat = 15,
	},
};
static struct model good_model[NUM_GOOD_MODELS] = {
	{
		.title = "ennui",
		.width = 0, .height = 0, .depth = 0,
		.nchunks = 0,
		.chunks = NULL,
	},
	{
		.title = "PrintJob1",
		.width = 15, .height = 15, .depth = 16,
		.nchunks = 1,
		.chunks = good01_chunks,
	},
	{
		.title = "PrintJob2",
		.width = 105, .height = 105, .depth = 106,
		.nchunks = 5,
		.chunks = good02_chunks,
	},
	{
		.title = "PrintJob3",
		.width = 264, .height = 301, .depth = 506,
		.nchunks = 100,
		.chunks = good03_chunks,
	},
	{
		.title = "garbage?",
		.width = 105, .height = 105, .depth = 106,
		.nchunks = 5,
		.chunks = good02_chunks,
	},
	{
		.title = "cart before the horse",
		.width = 105, .height = 105, .depth = 106,
		.nchunks = 5,
		.chunks = good02_chunks,
	},
	{
		.title = "H\tE\tL\tL\tO",
		.width = 2452, .height = 4756, .depth = 2386,
		.nchunks = 1,
		.chunks = good06_chunks,
	},
	{
		.title = "wheeeeee!",
		.width = 105, .height = 105, .depth = 106,
		.nchunks = 5,
		.chunks = good02_chunks,
	}
};

#endif
