#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "driver.h"
#include "model.h"
#include "tiptap.h"

// Initializes an empty model with the given title
int model_init(struct model *md, const char *title)
{
/*
Initializes all of the fields in the given model. The title should be set to the given value, and the model should have 0 dimensions and no chunks. If the title
parameter is too long to fit in the allotted space, it should be truncated to fit. This function should return 0 for success and nonzero if anything fails.
*/
	unsigned int i = 0;
	while (title[i] != '\0' && i < (TITLE_MAX)) //iterate through array until we hit a null character, add our own bounds check to avoid buffer overflow.
	{
		md->title[i] = title[i];
		i++;
	}
	md->title[i] = '\0';//then add our own null character
	md->width    = 0;
	md->height   = 0;
	md->depth    = 0;
	md->nchunks  = 0;
		
	return 0;
}
/*hasOverlap - self explanatory, checks if two chunks overlap based on their starting and ending coordinates (of the same dimension, of course.) */
int hasOverlap(int prevChunkStart, int prevChunkEnd, int currChunkStart, int currChunkEnd)
{
	//printf("hasoverlap: prevChunkStart/end = %d/%d currChunkStart/end = %d/%d\n", prevChunkStart, prevChunkEnd, currChunkStart, currChunkEnd);
	return  (prevChunkEnd  > currChunkStart && currChunkStart >= prevChunkStart) || (currChunkEnd  > prevChunkStart && currChunkStart < prevChunkStart);
}

// Adds a chunk to the model
int model_add_chunk(struct model *md, const struct chunk *ck)
{
	/*printf("(left, top, lowest layer) =  x: %d y: %d z: %d \n", ck->x, ck->y, ck->z);
	printf("Dimensions of chunk  width: %d height: %d depth: %d \n", ck->width, ck->height, ck->depth);
	printf("Material id: %d \n", ck->mat);
	printf("MD_WIDTH is %d\n", md->width);
	printf("numchunks is %d\n", md->nchunks);*/
	printf("adding chunk of material %d  width %d height %d depth %d to (%d %d %d)\n", ck->mat, ck->width, ck->height, ck->depth, ck->x, ck->y, ck->z);
	//reallocate memory to extend array of chunks in model (reallocate for exactly one instead of growing array for now.. perhaps talk to Devin about improving performance
	int chunk_size = sizeof(struct chunk);
	int overlap_case = 0;
	if (md->nchunks == 0 && ck != NULL) {
		md->chunks = (struct chunk*) malloc((md->nchunks + 1) * chunk_size);//allocate at least one
	}
	else {
		md->chunks = realloc(md->chunks, (md->nchunks + 1) * chunk_size);
	}
	if (md->chunks == NULL) {
		printf("model_add_chunk: Error allocating more memory!\n");
		return 1;
		}
	//If we've successfully reallocated, add our new chunk 
	md->chunks[md->nchunks] = *ck; 
	//See if there's an overlapping chunk we must take care of (may have to iterate eventually?)
	struct chunk previousChunk;//which means this may be better named overlappinChunk
	struct chunk currentChunk = md->chunks[md->nchunks];
	if (md->nchunks > 0) {
		for (unsigned int i = 0; i < md->nchunks; i++) {
			previousChunk = md->chunks[i];
			//Check overlaps 
			int previousChunkStartingX = previousChunk.x;
			int previousChunkEndingX = previousChunk.x + previousChunk.width;
			int currentChunkStartingX = currentChunk.x;
			int currentChunkEndingX = currentChunk.x + currentChunk.width;	
			overlap_case = hasOverlap(previousChunkStartingX, previousChunkEndingX, currentChunkStartingX, currentChunkEndingX); 
			//previous overlap current on x
			if (overlap_case == 1) {
			//	printf("@@@@ We have an overlap case with chunk %d!\n", i);	
				break;
			}

		}
	}
	if (ck->width != 0) { 
		if (overlap_case) {
			if (ck->x + ck->width > previousChunk.x + previousChunk.width) {
				if (md->width < ck->x + ck->width) //can only get bigger  
					md->width =  ck->x + ck->width;
			}
			else {
				if (md->width < previousChunk.x + previousChunk.width)  
					md->width = previousChunk.x + previousChunk.width;
			}
		}
		else	{
			if (md->nchunks <= 1)
				md->width = ck->x + ck->width;
			else if (ck->x + ck->width > md->width)
				md->width = ck->x + ck->width;
			}
		//printf("Model width is now %d\n", md->width);
	}
	else {
		printf("model_add_chunk: Invalid width passed!\n");
		free(md->chunks);
		md->chunks = NULL;
		return 1;
	}
	if (ck->height != 0) {
		if (overlap_case) {
			if (ck->y + ck->height > previousChunk.y + previousChunk.height) {
				if (ck->y + ck->height > md->height)//can only get taller
					md->height = ck->y + ck->height;
			}
			else {
				if (previousChunk.y + previousChunk.height > md->height)//can only get taller
					md->height = previousChunk.y + previousChunk.height;
			}
		}
		else 
			md->height = ck->y + ck->height;
	}
	else {
		printf("model_add_chunk: Invalid height passed!\n");
		free(md->chunks);
		md->chunks = NULL;
		return 1;
	}
	//printf("Height is now %d\n", md->height);
	if (ck->depth != 0) {
		if (overlap_case) {
			if (ck->z + ck->depth > previousChunk.z + previousChunk.depth) {
				if (ck->z + ck->depth > md->depth)
					md->depth = ck->z + ck->depth;
			}
			else {
				if (previousChunk.z + previousChunk.depth > md->depth)
					md->depth = previousChunk.z + previousChunk.depth;
			}
		}
		else {	
			if (md->depth < ck->z + ck->depth)
				md->depth = ck->z + ck->depth;//plus equals or just add?
		}
	}
	else {
		printf("model_add_chunk: Invalid depth passed!\n");
		free(md->chunks);
		md->chunks = NULL;
		return 1;
	}
	
	//If we've successfully added a chunk, increase nchunks and see if we need to model model width/depth
	md->nchunks++;
	//printf("numberofchunks is now %d\n", md->nchunks);
	//printf("width is now %d\n", md->width);
	
	return 0;
}

// Releases any resources allocated to the model
void model_destroy(struct model *md)
{
	if (md->nchunks != 0) { //meaning we actually have some memory allocated...
		free(md->chunks);	
		md->chunks = NULL;
		md->nchunks = 0; //and reset our number of chunks to zero to reflect our deallocation
	}
}

// Loads model info from a 3DM file, overwriting any existing information
int model_load(struct model *md, const char *filename)
{
	//You may assume the model has been just initialized with model_init
	
	//Open the model file using low level I/O
	int fd =  open(filename, O_RDONLY, 0);//readonly
	printf("loading file %s...\n", filename);
	if (fd < 0) {
		printf("model_load: error with open!\n");
		perror("open");
		return -1;
	}
	//Read file header, title, and chunkdata from their respective offsets
	//Each 3DM file begins with a 17-byte header at offset 0 in the file, and its structure is as follows
	char buf[17];
	int ret = read(fd, buf, 17);//should be no short reads here
	//printf("DEBUG: reading from %s yielded a %d\n", filename, ret);
	if (ret < 17)
	{
		if (ret >= 0)
			printf("model_load: error in short read for 17-byte header!\n");
		if (ret < 0) {
			printf("model_load: error with read\n");
			perror("read");
			}
		close (fd);//close our open file :) 
		return 1;
	}
	//Fill fields(You need to allocate and initialize any memory needed by the struct	)
	//Grab our magic number
	uint32_t magicNumber = buf[0] //simply grab first byte
			       | ((int) buf[1] << 8)//shift over our second byte
			       | ((int) buf[2] << 16) //etc..
			       | ((int) buf[3] << 24);
	magicNumber = ntohl(magicNumber);

	//Verify we have proper file format
	if (magicNumber != 0x03113d6d)
	{
		printf("Invalid file format specified\n");
		close(fd);//Close our open file :) 
		return 1;
	}
		

	//Grab our chunk list file offset
	uint32_t chunkOffset  = buf[4] //simply grab first byte
			       | ((int) buf[5] << 8)//shift over our second byte
			       | ((int) buf[6] << 16) 
			       | ((int) buf[7] << 24);
	chunkOffset = ntohl(chunkOffset);

	//Grab our title offset
	uint32_t titleOffset = buf[8] //simply grab first byte
			       | ((int) buf[9] << 8)//shift over our second byte
			       | ((int) buf[10] << 16) 
			       | ((int) buf[11] << 24);
	titleOffset = ntohl(titleOffset);
	
	//Grab our number of chunks 
	uint32_t numberOfChunks = buf[12] //simply grab first byte
			       | ((int) buf[13] << 8)//shift over our second byte
			       | ((int) buf[14] << 16) 
			       | ((int) buf[15] << 24);
	numberOfChunks = ntohl(numberOfChunks);
	
	//Grab our title length - since it is only one byte, no shifting is required.
	uint8_t titleLength = buf[16];

	/*printf("***chunkOffset is %d\n", chunkOffset);
	printf("***titleOffset is %d\n", titleOffset);
	printf("***numberOfChunks is %d\n", numberOfChunks);
	printf("***titleLength is %d\n", titleLength);*/

	//Fill out our required model information - title
	char title[256];
	int retVal = pread(fd, title, titleLength, titleOffset);
	//printf("DEBUG: pread return value for title is %d\n", retVal);
	if (retVal != titleLength) {
		if (retVal < 0) { 
			printf("model_load: pread error!(title)\n");
			perror("pread");
		}
		else 
			printf("short read error! (title)\n");
			
		close(fd); //Close our open file :) 
		return 1;
	}

	for (int i = 0; i < titleLength; i++)//assign our string
		md->title[i] = title[i];

	md->title[titleLength] = '\0';//add our null terminator
	//printf("Title is %s\n", md->title);




	//Fill out our required model information - number of chunks
	md->nchunks = numberOfChunks;


	//Take care of chunk data..
	if (md->nchunks != 0) {
	md->chunks = malloc(md->nchunks * sizeof(struct chunk));
	}


	for (unsigned int i = 0; i < numberOfChunks; i++) {
		const int chunkSize = 13;
		char chunkData[chunkSize];
		retVal = pread(fd, chunkData, chunkSize, chunkOffset);//reading list of chunks
		//printf("DEBUG: pread return value for chunk data is %d\n", retVal);
		if (retVal < 13 && numberOfChunks > 0) 
		{
			printf("retVal is %d\n", retVal);
			if (retVal < 0){
				printf("model_load: pread error! (chunk_list)\n");
				perror("pread");
			}
			else if (retVal == 0){
			printf("model_load: pread error. Reached EOF \n");	
			}
			close(fd); //We encountered an error, close our open file :) 
			free(md->chunks); //We encountered an error, free our memory!
			return 1;
			
		}
		uint16_t ckx 	= ntohs(chunkData[0] | ((int) chunkData[1] << 8));
		uint16_t cky 	= ntohs(chunkData[2] | ((int) chunkData[3] << 8));
		uint16_t ckz 	= ntohs(chunkData[4] | ((int) chunkData[5] << 8));
		uint16_t width 	= ntohs(chunkData[6] | ((int) chunkData[7] << 8));
		uint16_t height = ntohs(chunkData[8] | ((int) chunkData[9] << 8));
		uint16_t depth 	= ntohs(chunkData[10] | ((int) chunkData[11] << 8));
		if (numberOfChunks == 0){
			width = 0;
			height = 0;
			depth = 0;
		}
		uint8_t materialID = chunkData[12];//1 byte is 8 bits, which can hold up to 255, this is flimsy and incorrect
		if (materialID > 15) { //tiptap can only take 1 byte materialID, so max value is 15
			printf("Model_load error: invalid material id! (%d)\n", materialID); 
			close(fd);
			free(md->chunks);
			return 1;
		}
		
		/*printf("Chunk[%d]\n", i);
		printf("***xyz is (%d, %d, %d)\n", ckx,cky,ckz); 
		printf("***width is %x\n", width); 
		printf("***height is %d\n", height); 
		printf("***depth is %d\n", depth); 
		printf("***material ID is %d\n", materialID); */
		if (width == 0 || height == 0 || depth == 0) {
			printf("Model_load error: invalid coordinate given in height/width/depth dimension: %d/%d/%d\n", width, height, depth);
			close(fd); //We had a file open, better close it
			free(md->chunks);//We should also free our allocated memory..
			return 1;
		}
		if (numberOfChunks == 0) {
			md->width = 0;
			md->height = 0;
		}
		else {
			if (ckx + width > md->width)//width can only grow
				md->width = ckx + width;
			if (cky + height > md->height)//height can only grow
				md->height = cky + height;
			if (ckz + depth > md->depth)//depth can only grow
				md->depth  = ckz + depth;	
			//printf("width is now %d\n", md->width);
		}

		//Allocate/initialie memory needed for chunk struct 	
		//printf("number of chunks is %d\n", md->nchunks);
		md->chunks[i].x = ckx;	
		md->chunks[i].y = cky;	
		md->chunks[i].z = ckz;	
		md->chunks[i].width = width;
		md->chunks[i].height = height;
		md->chunks[i].depth = depth;
		md->chunks[i].mat = materialID;
		//Increment chunk offset so we aren't reading same chunk over and over again	
 		chunkOffset = chunkOffset + 13;
	}
	
	//After data has been loaded, the file should be closed.
	retVal = close(fd);
	if (retVal < -1) {
		printf("model_load error with close!\n");
		perror("close");
		return -1;
	}
	fd = -1;//Good practice
	return 0;
}

//need to validate coordinates here as well because of the nature of how I am printing - always passing 0, 0 to xy for printlayer. Is this abusing the abstraction? I don't think so, just a little outside of the box thinking. :)
int validate_chunk_coordinates(struct tiptap *tt, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

	int currentx = x;
	int currenty = y;	
	for (int k = 0; k < h; k++) {
		for (int i = 0; i < w; i++) {
			if (currentx > tt->max_x) {
				printf("Error in validation of coordinates! currentx > tt->max_x (%d > %d)\n", currentx, tt->max_x);
				return 1;
			}	

			if (currenty + k > tt->max_y) {
				printf("Error in validation of coordinates! currenty + k > tt->max_y (%d > %d)\n", currenty + k, tt->max_y);
				return 1;
			}
			currentx++;	
		}
	currentx = x;
	}		
	return 0;

}

// Prints model at layer Z=0 on an attached and powered-on TipTap
int model_print(struct model *md, struct tiptap *tt, uint16_t x, uint16_t y)
{
	/*This function should position the nozzle to begin printing,*/
	int largestWidth = 0;
	int largestHeight = 0;
		
	for (unsigned int i = 0; i < md->nchunks; i++) {
		struct chunk currentChunk = md->chunks[i];
		if (currentChunk.width > largestWidth) 
			largestWidth = currentChunk.width;
		if (currentChunk.height > largestHeight) 
			largestHeight = currentChunk.height;
	}

 	int retVal = tiptap_moveto(tt, x, y, /*current_z_layer +*/ 1);//initially start at 1
	if (retVal != 0) {
		printf("model_print error: tiptap_moveto error\n");
		return 1;
	}

	uint8_t* materials_ptr = NULL;//allocating and freeing each time we iterate through chunk 
	uint8_t* materials = NULL;//set our pointers to null initially to check for possible errors further down the road

	int maxX = tt->max_x + 1;
	int maxY = tt->max_y + 1;
	for (int currentDepth = 0; currentDepth < tt->max_z - 1; currentDepth++) {
		if (maxX == largestWidth) {
			printf("We have exact printer dimensions\n");
			materials = malloc(largestWidth * largestHeight * sizeof(uint8_t)); //allocate some memory for our materials array (for each layer)
			for (int i = 0; i < largestWidth * largestHeight; i++) 
				materials[i] = 0;//initially zero out our array
		}
		else {
			materials = malloc(maxX * maxY * sizeof(uint8_t)); //allocate some memory for our materials array (for each layer)
			for (int i = 0; i < maxX * maxY; i++) 
				materials[i] = 0;//initially zero out our array
			//printf("allocating array of size %d\n", maxX * maxY);
		}

		materials_ptr = materials; //to avoid worrying about pointer-y/array funkiness, this is a separate variable to keep track of the beginning of the array
		struct chunk currentChunk;
		struct chunk previousChunk;//might not be correct way but will push us where we need to go 
		for (unsigned int i = 0; i < md->nchunks; i++) {
				currentChunk = md->chunks[i];
				//Since we're printing with 0, 0, we actually need to validate our coordinates beforehand...
				retVal = validate_chunk_coordinates(tt, x + currentChunk.x, y + currentChunk.y, currentChunk.width, currentChunk.height);	
				if (retVal != 0) {
					printf("model_print: invalid coordinates passed!\n");
					free(materials_ptr);
					return 1;
				}
					retVal = tiptap_moveto(tt, currentChunk.x, currentChunk.y, currentDepth + 1);
				if (retVal != 0) {
					printf("model_print error: tiptap_moveto error\n");
					printf("memory freed\n");
					if (materials_ptr != NULL) {
						free(materials_ptr);
						materials_ptr = NULL;
					}
					else
						printf("why u do this\n");
					return 1;
					}
				if (materials == NULL)
					return 1;	
				//printf("bed dimensions are %dx%dx%d\n", tt->max_x, tt->max_y, tt->max_z);
				for (int heightIter = currentChunk.y + y; heightIter < currentChunk.y + y + currentChunk.height; heightIter++) {
					//printf("DEBUG: j starts at %d, its upper bound is %d\n", currentChunk + heightIter * tt->max_x, currentChunk.x * heightIter
					//at every depth, begin at currentChunk.x and go for the width of the current chunk
					for (int j = currentChunk.x + x + heightIter * maxX; j < currentChunk.x + x + heightIter * maxX + currentChunk.width ; j++) { 						

						if (currentDepth >= currentChunk.z && currentDepth < currentChunk.z + currentChunk.depth) 
							materials[j] = currentChunk.mat;
					}
				}
			previousChunk = currentChunk;
			}//end chunk iteration

			/*printf("DEBUG: array is \n");	
			int iter = 1;
				for (int i = 0; i <  maxX * maxY; i++) {
					 printf("%x", materials[i]);	
					 if (iter % maxX == 0 && i != 0)
						printf("\n");
					iter++;
				}
					printf("\n");*/
				
			retVal = tiptap_printlayer(tt, 0, 0, maxX, maxY, materials);
			if (retVal != 0) {
				printf("model_print error: tiptap_printlayer error\n");
				return 1;
			}

			//tt_log_layer(currentDepth);
			free(materials_ptr);//free our materials after we're done with them!
		}
	return 0;
}

// BONUS: Saves model info to a 3DM file, overwriting it if it exists
int model_save(struct model *md, const char *filename)
{
	//This function creates a new 3DM file with the given filename and writes the data from the given model struct into it using low-level I/O
	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);	
	if (fd < 0) {
		printf("model_load: error with open!\n");
		perror("open");
		return -1;
	}
	//each 3dm file begins with a 17-byte header at offset 0 in the file
	/* 
	0 4 Magic number 0x03113d6d to identify file format
	4 4 File offset where chunk list begins
	8 4 File offset where title begins
	12 4 Number of chunks
	16 1 Length of title
	*/
	printf("DEBUG: model info: title: %s | whd=(%d, %d, %d), nchunks=(%d)\n", md->title, md->width, md->height, md->depth, md->nchunks);

	uint32_t magicNumber = 0x03113d6d;
	printf("proper magic number is %x\n", ntohl(magicNumber));
	//close the file after we are done having our way with it	
	int retVal = close(fd);
	if (retVal != 0) {
		printf("error closing file!\n");
		perror("close");
		return -1;	
	}
	return 0;
}
