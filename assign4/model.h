#ifndef MODEL_H_
#define MODEL_H_

#include <stdint.h>

#include "driver.h"

#define TITLE_MAX ((1UL << 8) - 1)

// Represents a rectangular chunk of material
struct chunk {
	// Coordinates (left, top, lowest layer) of chunk
	uint16_t x, y, z;
	// Dimensions of chunk
	uint16_t width, height, depth;
	// Material ID for chunk
	uint8_t mat;
};

// Represents a 3D model
struct model {
	// Title of the model (normal, terminated C string)
	char title[TITLE_MAX + 1];
	// Dimensions of the model, assuming it starts at (0, 0, 0)
	uint16_t width, height, depth;
	// Number of chunks in the model
	uint32_t nchunks;
	// Dynamically allocated array of chunks
	struct chunk *chunks;
};

// Initializes an empty model with the given title
int model_init(struct model *md, const char *title);

// Adds a chunk to the model
int model_add_chunk(struct model *md, const struct chunk *ck);

// Releases any resources allocated to the model
void model_destroy(struct model *md);

// Loads model info from a 3DM file, overwriting any existing information
int model_load(struct model *md, const char *filename);

// Prints model at layer Z=0 on an attached and powered-on TipTap
int model_print(struct model *md, struct tiptap *tt, uint16_t x, uint16_t y);

// BONUS: Saves model info to a 3DM file, overwriting it if it exists
int model_save(struct model *md, const char *filename);

#endif
