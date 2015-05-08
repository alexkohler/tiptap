/*
 * Source file for the TipTap driver.  This is where you will implement the
 * user-facing API (tiptap_*) for the TipTap, using the tapctl() hardware
 * interface to communicate with the device.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "tiptap.h"
#include "driver.h"

//opcode - 5 bits (31-27)
//material - 4 bits (26-23)
//distance - 9 bits (signed 8 bit number, distance to move) (22-14)
//flags - 4 bits (13-10)
//sequence number - 10 bits (9-0)
uint32_t pack_bytes_correctorder(uint32_t opcode, uint32_t material, int32_t distance, uint32_t flags, uint32_t sequence_number ) { 
 // Setup some local values 
 uint32_t retval = 0x0, tempa, tempb, tempc, tempd, tempe; 
 //Since most significant bits are written on left, that means our opcode should come first in our final result
 //this being said, we want to first grab our sequence number since it will be the LAST thing we want  

 tempa = sequence_number&0x3ff; // Make sure you are only getting the bottom 10 bits - 0x3ff comes from wanting to and with 11 1111 1111 (10 ones) 
 tempb = (flags&0xf) << 10; //Since flags are 4 bit long, we want to and with 1111 (4 ones), then shift value over 10 bits (to account for sequence number)
 tempc = (distance&0x1ff) << 14; //shift over (10 + 4) bits	
 tempd = (material&0xf) << 23;//shift over (10 + 4 + 9) 23 bits
 tempe = (opcode&0x1f) << 27;//shift (10 + 4 + 9 + 4) bits

 retval = tempa|tempb|tempc|tempd|tempe; // Now combine all of the values 
 // Print out all of the values 
 // Return the computed value 
 return( retval ); 
}

/*increment seq - takes care of incrementing sequence and gets us an error message if possible if something goes awry*/
int increment_seq(int retVal, struct tiptap *tt)
{
	int incrementedSequenceNum = tt->sequence_number + 1;
	if (retVal != 0) {
		tt_error(retVal);
		return 1;
		}
	tt->sequence_number = incrementedSequenceNum; 
	return 0;
}

/*handle_move - similar wrapper to handle_print - all big moves and other things that can go wrong are handled in here*/
int handle_move(struct tiptap *tt, int opcode, int xyz, int xyz_indicator)
{
	int distance;
	tt->material = 0; //moving, no need to be making a mess
 	if (xyz_indicator == 0){
		distance = xyz - tt->current_x;		
		}
 	else if (xyz_indicator == 1){
		distance = xyz - tt->current_y;		
		}
 	else if (xyz_indicator == 2){
		distance = xyz - tt->current_z;		
		}
	if (distance == 0)
		return 0;
	tt->opcode = opcode;
	tt->flag = 0;
	tt->distance = distance;
	if (abs(distance) < 255)
	{
		int32_t movex_instruction = pack_bytes_correctorder(
			tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
		int retVal = tapctl(movex_instruction, tt->data);
		if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
			printf("error in handle_move when distance < 255!\n");
			printf("distance is %d\n", distance);
			return 1;
		}
	}	
	else 
	{
	int max_movexyz_instructions;
	int remainder_movexyz_instruction;
	int notPositive = (distance < 0);
	//use all the bits we're given!
		if (notPositive) {
			max_movexyz_instructions = abs(distance / 256);
			remainder_movexyz_instruction= distance % 256;
		}
		else {//we are positive
			max_movexyz_instructions = abs(distance / 255);
			remainder_movexyz_instruction= distance % 255;
		}
		//printf("distacnce is greater than 255/256, max move instructions: %d, remainder move instructions %d\n", max_movexyz_instructions, remainder_movexyz_instruction);
		if (notPositive)
			tt->distance = -256;
		else
			tt->distance = 255;
		//printf("distance is %d\n", tt->distance);
	        for (int i = 0; i < max_movexyz_instructions; i++)
	       	{
	        	int32_t movexyz_instruction = pack_bytes_correctorder(
	        		tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
	        	int retVal = tapctl(movexyz_instruction, tt->data);
			if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl		
				printf("error in handle_move when attempting to print maximally!\n");
				return 1;
			}
	        }
		tt->distance = remainder_movexyz_instruction;
	       	int32_t movexyz_instruction = pack_bytes_correctorder(
	       		tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
	        int retVal = tapctl(movexyz_instruction, tt->data);
		if (increment_seq(retVal, tt) != 0)//increment after each call to tapctl		
				return 1;
	
	}
 	if (xyz_indicator == 0){
		tt->current_x = xyz;
		//printf("current x is now %d\n", tt->current_x);
		}
 	else if (xyz_indicator == 1){
		tt->current_y = xyz;
		//printf("current y is now %d\n", tt->current_y);
		}
 	else if (xyz_indicator == 2){
		tt->current_z = xyz;
		//printf("current z is now %d\n", tt->current_z);
		}
	return 0;
}

/*validate_coordinates - simulate the moves we are going to make before we make them to make sure they're actually possible. This may burn some CPU but it's better than burning our precious commands!*/
int validate_coordinates(struct tiptap *tt, uint16_t x, uint16_t y,
		uint16_t w, uint16_t h)
{

	//printf("validate_coordinates: passed values of x=%d, y=%d, w=%d, w=%d, tt->max_x=%d, tt->max_y=%d\n", x, y, w, h, tt->max_x, tt->max_y);
	int currentx = x;
	int currenty = y;	
	for (int k = 0; k < h; k++) {
		for (int i = 0; i < w; i++)
		{
			if (currentx > tt->max_x){
				printf("Error in validation of coordinates! currentx > tt->max_x (%d > %d)\n", currentx, tt->max_x);
				return 1;
			}
			if (currenty + k > tt->max_y){
				printf("Error in validation of coordinates! currenty + k > tt->max_y (%d > %d)\n", currenty + k, tt->max_y);
				return 1;
			}
			currentx++;	
		}
	currentx = x;
	}		
	//printf("Verification successful!\n");
	return 0;

}

//uint32_t pack_bytes_modified_noprint(uint32_t opcode, uint32_t material, int32_t distance, uint32_t flags, uint32_t sequence_number ) { 
//opcode - 5 bits
//material - 4 bits
//distance - 9 bits (signed 8 bit number, distance to move)
//flags - 4 bits
//sequence number - 10 bits
int tiptap_init(struct tiptap *tt)
{
	tt->opcode 	    = TT_POWERON;
	tt->material 	    = 0;
	tt->distance        = 0;
	tt->flag    	    = 0;
	tt->sequence_number = 0;
	uint16_t my_pointer[] = {1, 2};
	tt->data = my_pointer;
	uint32_t init_instruction = pack_bytes_correctorder(
		tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
	
	int retVal = tapctl(init_instruction, tt->data);	
	if (retVal != 0) {
		tt_error(retVal);
		return 1;
	}
	else {
	    int incrementedSequenceNum = tt->sequence_number + 1;
	    tt->sequence_number = incrementedSequenceNum;
	}	
	//get our max dimensions
	uint16_t *xy_pair_pointer = (uint16_t*) tt->data;
	//printf("max x element is %d\n", *xy_pair_pointer);
	tt->max_x = *xy_pair_pointer - 1;//0 indexed
	//printf("max y element is %d\n", *(xy_pair_pointer +1));
	tt->max_y = *(xy_pair_pointer + 1) - 1;//0 indexed


	
	//after initializing, we should be at (0, 0, d)
	tt->current_x = 0;
	tt->current_y = 0;
	uint16_t somex = 11, somey = 11, somez = 11;
	tiptap_getpos(tt, &somex, &somey, &somez);
	tt->current_z =  tt->max_z;
	tt->home_zpos = tt->max_z;
	//printf("After initialization, x is %d, y is %d, and z is %d\n", tt->current_x, tt->current_y, tt->current_z);

	return 0;
}

void tiptap_destroy(struct tiptap *tt)
{
	tt->opcode = TT_POWEROFF;
	tt->flag = 0;
	tt->material = 0;
	tt->distance = 0;
	uint32_t destroy_instruction = pack_bytes_correctorder(
		tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
	int retVal = tapctl(destroy_instruction, tt->data);	
	increment_seq(retVal, tt);
}

int tiptap_moveto(struct tiptap *tt, uint16_t x, uint16_t y, uint16_t z)
{
	//printf("we want to move to (%d, %d, %d)\n", x, y, z);
	//printf("our current position is (%d, %d, %d)\n", tt->current_x, tt->current_y, tt->current_z);
	if (x > tt-> max_x || y > tt->max_y || z > tt->home_zpos)
	{
		printf("error in tiptap_moveto:");
		if (x > tt->max_x)
			printf(" x > tt->max_x : %d > %d", x, tt->max_x);	
		if (y > tt->max_y)
			printf(" y > tt->max_y : %d > %d", y, tt->max_y);	
		if (z > tt->home_zpos)
			printf(" z > tt->home_zpos: %d > %d", z, tt->home_zpos);
		printf("\n");
		return 1;
	}
	if (handle_move(tt, TT_MOVEX, x, 0) != 0){
		//printf("handle_move failed in moving x!\n");
		return 1;
		}
	if (handle_move(tt, TT_MOVEY, y, 1) != 0) {
		//printf("handle_move failed in moving x!\n");
		return 1;
		}
	if (handle_move(tt, TT_MOVEZ, z, 2) != 0) {
		//printf("handle_move failed in moving x!\n");
		return 1;
		}	
	//printf("our position is now (%d %d %d)\n", tt->current_x, tt->current_y, tt->current_z);
	//reset distance to 0 after our moves
	tt->distance = 0;
	return 0;
}

void tiptap_getpos(struct tiptap *tt, uint16_t *x, uint16_t *y, uint16_t *z)
{
	*x = tt->current_x;
	*y = tt->current_y;
	tt->flag = TTFLAG_PROBE;
	tt->opcode = TT_MOVEZ; //need to pass one of these three flags, doesn't matter as we are just probing
	uint32_t getpos_instruction = pack_bytes_correctorder(
		tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
	int retVal = tapctl(getpos_instruction, tt->data);
	increment_seq(retVal, tt);

	uint16_t *zPointer = (uint16_t *) tt->data; //this is the space between the nozzle and the nearest material directly beneath it (or bed if no material)
	tt->max_z = *zPointer;
	*z = tt->current_z;
		
}

/*createSubArray - creates an array between startingIndex and endingIndex*/
void createSubArray(char *baseArray, char *subArray, int startingIndex, int endingIndex)
{
	int subArrayIndex = 0;
	for (int i = startingIndex; i < endingIndex; i++)
	{
		subArray[subArrayIndex] = baseArray[i];	
		subArrayIndex++;	
	}		
}

/* handle_print - wrapper for print function, actually handles the nitty-gritty of printing (big prints and edge cases are sorted out here*/
int handle_print(struct tiptap *tt, int opcode, int material, char *dataParam, int distance)
{
	if (distance > 255) {
		int numberOfIterations = distance / 255;
		int remainder = distance % 255;
		//First, do our biggest prints...
		int i;
		for (i = 0; i < numberOfIterations; i++)
		{
			tt->distance = 255;//distance will be maxed here as a tradiitonal print
			char subArray[255];
			createSubArray(dataParam, subArray, i * 255, i * 255 + 255); //data will be a sub array (0 - 255, then 256 - (256 + 255)
			tt->data = subArray; 
			tt->material = material; //material will be the same as a tradiitonal print
			tt->flag = TTFLAG_EXTRUDE;
			tt->opcode = opcode;//opcode and flag will also be the same as a tradiitonal print
			uint32_t printx_instruction = pack_bytes_correctorder(
					tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
			int retVal = tapctl(printx_instruction, tt->data);
			if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
				printf("handle_print error while attempting to do max distance print!\n");
				return 1;
			}
			if (opcode == TT_MOVEX)
				tt->current_x = tt->current_x + tt->distance;	
			else if (opcode == TT_MOVEY)
				tt->current_y = tt->current_y + tt->distance;	
			//Don't need to worry about TT_MOVEZ, my code does not call the handle_print instruction with the TT_MOVEZ flag.

		}
		//Then take care of the remainder
		tt->distance = remainder;
		char subArray[remainder];
		createSubArray(dataParam, subArray, i * 255, distance);//our upper bound will be distance (the total distance, because this is our remainder)
		tt->data = subArray;	
		int edgeCase = 1;
		if (opcode == TT_MOVEX) { 
			if (tt->current_x + remainder > tt->max_x) { //if it's an edge case, we possibly can fix it
				if (tt->current_x + remainder - tt->max_x == 1)	{
					edgeCase = 0;
					//printf("WE HAVE A FUCKING EDGE CASE\n");
					tt->distance = remainder - 1;
	
				}
		}
		else if (opcode == TT_MOVEY)
			tt->current_y = tt->current_y + tt->distance;	
		}
		uint32_t printx_instruction = pack_bytes_correctorder(
				tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
		int retVal = tapctl(printx_instruction, tt->data);
		if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
			printf("handle_print error when attempting to do remainder print\n");
				return 1;
		}
		tt->current_x = tt->current_x + tt->distance;//increment after calling to tapctl

		if (dataParam[distance - 1] > 0 && !edgeCase) {//edge cases -- can occur in either remainder OR a general distance less than 256
			
			tt->distance = 0;
			int arr[] = {1};
			tt->data = arr;
			uint32_t printx_instruction = pack_bytes_correctorder(
				tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
			int retVal = tapctl(printx_instruction, tt->data);
			if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
				printf("handle_print error when attempting to take care of an edge\n");
				return 1;
			}
		}
	}
	else //otherwise we have a distance less than 256
	{
		tt->distance = distance; //int arr[] = {1}; 
		tt->data = dataParam;	
		tt->material = material;
		tt->flag = TTFLAG_EXTRUDE;
		tt->opcode = opcode;
		int edgeCase = 1;
		if (opcode == TT_MOVEX) {
			if (tt->current_x + distance > tt->max_x) { //if it's an edge case, we possibly can fix it
				if (tt->current_x + distance - tt->max_x == 1)	{
					//print up to distance - 1, then use special case of distance = 0
					edgeCase = 0;
					tt->distance = distance - 1;					
					/*uint32_t printx_instruction = pack_bytes_correctorder(
						tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);*/
					}
				else {
					printf("printing edge case was greater than 1! I can't recover from this! tt->currentx + distance - tt->max_x = %d \n", tt->current_x + distance - tt->max_x);
					return 1;
				}
			}
		}
		
		uint32_t printx_instruction = pack_bytes_correctorder(
			tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
		int retVal = tapctl(printx_instruction, tt->data);
		if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
			printf("handle_print error when attempting to do traditional print! (Not a big print or its remainder)\n");
			return 1;
		}
			
		tt->current_x = tt->current_x + tt->distance;	

		if (dataParam[distance - 1] > 0 && !edgeCase) {//print the edge value
			
			tt->distance = 0;
			int arr[] = {1};
			tt->data = arr;
			uint32_t printx_instruction = pack_bytes_correctorder(
				tt->opcode, tt->material, tt->distance, tt->flag,  tt->sequence_number);
			int retVal = tapctl(printx_instruction, tt->data);
			if (increment_seq(retVal, tt) != 0){//increment after each call to tapctl	
				printf("handle_print error when attempting to take care of an edge\n");
				return 1;
			}
		}
	}//end else
		
	return 0;

}

/*checkForDuplicates - creates a frequency array. duplicatesArray is size 16 (to represent all hex values), keep track of number of occurences. (hex value is essentially "key", and its frequency is the "value")*/
void checkForDuplicates(const uint8_t *materials, int startingIndex, int endingIndex, int *duplicatesArray)
{
	for (int i = startingIndex; i < endingIndex; i++)	
	{
		duplicatesArray[materials[i]]++;
	}
}

int tiptap_printlayer(struct tiptap *tt, uint16_t x, uint16_t y,uint16_t w, uint16_t h, const uint8_t *materials)
{
	int retVal = validate_coordinates(tt, x, y, w, h);

	//printf("validating coordinates..xy = (%d, %d), wh = (%d, %d)\n", x, y, w, h);
	//printf("current coordinates are (%d, %d, %d)\n", tt->current_x, tt->current_y, tt->current_z);
	if (retVal != 0)//we've encountered an error :(
	{		
		printf("A validation problem has occured!\n");
		return 1;	
	
	}
		
	int currentRow = 0;
		for (int rowIterator = currentRow; currentRow < h; currentRow++, rowIterator++)
		{ 
			/*printf("[#######CURRENT ROW: %d] [DEPTH: %d]  [ROW WIDTH: %d] MATERIALS {", currentRow, tt->current_z, w);
			for (int g = currentRow * w; g < currentRow * w + w; g++)
				printf("%x", materials[g]);
			printf("} \n");
			printf("	checking for frequency of materials between %d - %d \n", currentRow * w, currentRow * w + w);*/
			int duplicatesArray[15];
			for (int i = 0; i < 15; i++) 
				duplicatesArray[i] = 0; //0 out dup array

			checkForDuplicates(materials, currentRow * w, currentRow * w + w, duplicatesArray);

			for (int materialVal = currentRow * w; materialVal < currentRow * w + w; materialVal++)
			{
				//move to our initial spot
				//printf("	%d ", materials[materialVal]);

				//printf("efficiency array: ");
				int noop = 5;
				if (materials[materialVal] == 0)
					printf(" ");
				else if (duplicatesArray[materials[materialVal]] > 0) { //if there is an occurence of current material 
					//printf("value here is %d\n", duplicatesArray[materials[materialVal]]);
					//printf("(moving to beginning of row) \n");
						
					if (w != 1)
					retVal = tiptap_moveto(tt, x, y + currentRow, tt->current_z); //only move if there is a material in the current row
						
					if (retVal != 0)//we've encountered an error :(
					{		
						printf("A moveto problem has occured in tiptap_printlayer!\n");
						return 1;	
					
					}
					char efficiencyArray[w];
					uint8_t currentMaterial = materials[materialVal];
					int beginningOfRow = currentRow * w;//form efficiency array at beg. of row
					int efficiencyArrayIndex = 0;
					for (int i = beginningOfRow; i < beginningOfRow + w; i++)
					{
						if (currentMaterial == materials[i])
							efficiencyArray[efficiencyArrayIndex] = 1;
						else
							efficiencyArray[efficiencyArrayIndex] = 0;
					efficiencyArrayIndex++;
					} 
					/*for (int i = 0; i < w; i++)
					{
						printf("[%d] ", efficiencyArray[i]);
					}*/
					if (w != 1)
						retVal = handle_print(tt, TT_MOVEX, currentMaterial, efficiencyArray,  w);  
					else	
						retVal = handle_print(tt, TT_MOVEY, currentMaterial, efficiencyArray,  1);  
					if (retVal != 0)//we've encountered an error :(
					{		
						printf("A printing problem has occured in tiptap_printlayer!!\n");
						printf("current width is %d\n", w);//to determine which statement bombed out
						return 1;	
					
					}
					//tt_log_layer(tt->current_z - 1);

					/*printf("EFFICIENCY ARRAY FOR %d IS ", currentMaterial);
					for (int i = 0; i < w; i++)
						printf("%d ", efficiencyArray[i]);	
					printf("\n");*/

						
					//put negative ("garbage") value so we don't try to print here again! (the if statement will evaluate to false for this material, which is what w want because we have already taken care of it for this row!
					duplicatesArray[currentMaterial] = -1;		
				}
				else{ ///less than 1, meaning already taken care of!
					//printf("Already taken care of! ");	
					noop = 10;
					noop++;
				}

				//printf("\n");

			}	 
			//printf("\n");
		}
		//do another for loop just for sake of clarity
		retVal = tiptap_moveto(tt, tt->current_x, tt->current_y, tt->current_z + 1);
                if (retVal != 0)//we've encountered an error :(
                {
                printf("A moving problem has occured in tip_printlayer when attempting to move the nozzle one unit up in the z direction!\n");
                return 1;
                }

		//tt_log_layer(tt->current_z - 2);
	return 0;
}


/* Bonus */
int tiptap_scan(struct tiptap *tt, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint16_t *output)
{
	if (x || y || w || h || *output || tt)//to stop those pesky unused warnings!
		return 1;
	return 0;
} 
