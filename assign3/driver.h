#ifndef DRIVER_H
#define DRIVER_H
#include <stdint.h> //to surpress unknown type errors
/*
 * Header file for the TipTap driver.  This file defines the types and functions
 * needed to implement a driver for the TipTap hardware.
 *
 * DO NOT put function implementations in this file, and DO NOT change the
 * function prototypes below!  The only thing you should need to change here is
 * to fill in the struct definition below with any state you want to keep track
 * of.
 */

struct tiptap {
	/* Store any needed driver state in this struct.  If you use global
	 * variables in a driver, it will stop working as soon as you connect a
	 * second device... so put them here instead!  This struct will be
	 * passed to each of the driver functions so you can read your state
	 * variables from it.
	 */
	int opcode          : 5;
	int material        : 4;
	int distance        : 9;//max value of distance is
	int flag            : 4;
	int sequence_number : 10;
	int home_zpos;
	void* data;   
	//current xyz coords
	int current_x;
	int current_y;
	int current_z;

	//max bed dimensions
	int max_x;
	int max_y;
	int max_z;
	
	int material_height;

};

int tiptap_init(struct tiptap *tt);
void tiptap_destroy(struct tiptap *tt);

int tiptap_moveto(struct tiptap *tt, uint16_t x, uint16_t y, uint16_t z);

void tiptap_getpos(struct tiptap *tt, uint16_t *x, uint16_t *y, uint16_t *z);

int tiptap_printlayer(struct tiptap *tt, uint16_t x, uint16_t y,
		uint16_t w, uint16_t h, const uint8_t *materials);

/* Bonus */
int tiptap_scan(struct tiptap *tt, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint16_t *output);

#endif
