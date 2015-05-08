#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h> //for errno
#include <string.h>//for strerror
#include <stdlib.h>//for malloc/abs

#include "network.h"
#include "tiptap.h"


// Variable to keep track of the socket file descriptor
static int sock = -1;

// Convenience function to sign-extend a 9-bit two's complement number into
// something bigger.  Takes the 9-bit version and returns a value suitable for
// storing in a signed integer of any sort.
int signextend_9bit(int nine_bit)
{
	if (nine_bit & (1L << 8))
		nine_bit |= ~((1L << 9) - 1);
	return nine_bit;
}

//packs instruction number and buffer size into a 6 byte character array
void initial_pack(unsigned char * arr, uint32_t insn, uint8_t bufSize)
{
	arr[0] = (insn >> 24) & 0xFF;
	arr[1] = (insn >> 16) & 0xFF;
	arr[2] = (insn >> 8) & 0xFF;
	arr[3] = (insn & 0xFF);
	arr[4] = (bufSize >> 8) & 0xFF;
	arr[5] = (bufSize & 0xFF);
}

int handle_power(int opcode, int insn) 
{
	if (opcode == (int) TT_POWERON) { 
		//Start by creating our socket and getting a file descriptor
		sock = socket(AF_INET, SOCK_STREAM, 0);	//ipv4, stream(tcp),d w protocol

		//Setup address information
		struct sockaddr_in caddr;
		caddr.sin_family = AF_INET;
		caddr.sin_port = htons(TIPTAP_PORT);
	
		if (inet_aton(TIPTAP_IP, &caddr.sin_addr) == 0){ //0 - address invalid
			printf("error with inet_aton: invalid address!\n");
			return -1;
		}
		
		//Attempt to connection with the information we set up
		if (connect(sock, (struct sockaddr *) &caddr, sizeof(caddr)) == -1 ){
			printf("Error with connect!(): [%s]\n", strerror(errno));
 			return -1;
		} 
		else 
			printf("Connection successful!\n");

	}
	//If we have a TT_POWEROFF instruction, time to tear down!
	else if (opcode == (int) TT_POWEROFF) {
		printf("Destroy called, sending message and reading response\n");

		unsigned char send_buffer[6];
		initial_pack(send_buffer, insn, 0);

		if (write(sock, send_buffer, 6) != 6 ) {
			perror("write");
 			printf( "Error writing network data [%s]\n", strerror(errno) );
 			return -1;
 		} 

		unsigned char response[4];
		int retVal;
		if ( (retVal = read(sock, &response, sizeof response )) == -1 ) {
 			printf( "Error reading network data [%s]\n", strerror(errno) );
 			return -1 ;
 		}
		
		uint16_t returnValue = ntohs((response[1] << 8) | response[0]);
		uint16_t additionalBytes = ntohs((response[3] << 8) | response[2]);
		if (additionalBytes == 0)
			return returnValue;
		else {
			printf("Unexpected additional bytes received after reading response to  destroy\n");
			return 1;
		}
	}
	return 0;
}


// Network interface for the TipTap 3-D printer
int tapctl(uint32_t insn, void *buf)
{
	//to grab bits 31-27, we shift them to the lowest position and mask with 0x1f =11111
	int opcode   = (insn >> 27) & 0x1f;//first 5 bits
	int material = (insn >> 23) & 0xf;
	int distance = signextend_9bit((insn >> 14) & 0x1ff); //extend our 2scomp dist
	int flag     = (insn >> 10) & 0xf;
	//int seqNum   = (insn >>  0) & 0x3ff;
	


	//Handle poweron/poweroff
	if (opcode == (int) TT_POWERON || opcode == (int) TT_POWEROFF) { 
			int retVal;
			if ( (retVal = handle_power(opcode, insn)) != 0) {
				printf("nonzero return value in handle_power\n");
				return 1;
			}
	}
	
	//send our instruction number to the server!
	//form our message - 6 + n bytes long where n is number of bytes in data buf
	
	//write our instruction number to offset 0
	if (flag != TTFLAG_EXTRUDE) {
		//pack a 6 byte instruction into character array (our instruction with a request for 0 bytes)
		unsigned char send_buffer[6];
		initial_pack(send_buffer, insn, 0);
		//single write bytes brah
		if (write(sock, send_buffer, 6) != 6 ) {
			perror("write");
 			printf( "Error writing network data [%s]\n", strerror(errno) );
 			return -1;
 		} 


		char initial_buffer[4];
		int bytesRead =  read(sock, initial_buffer, sizeof initial_buffer); 	
		if (bytesRead != 4) {
			printf("We've encountered a short read. 4 bytes not read... (%d received)\n", bytesRead);
			if (bytesRead == -1) {
 				printf( "Error reading network data [%s]\n", strerror(errno) );
				return 1;
			}
		}

		//uint16_t returnValue = ntohs((initial_buffer[1] << 8) | initial_buffer[0]);
		uint16_t additionalBytes = ntohs((initial_buffer[3] << 8) | initial_buffer[2]);

		if (additionalBytes > 0)  {
			//printf("we have an additional %d bytes to fetch\n", additionalBytes);
			unsigned char* additional_buffer = malloc(additionalBytes * sizeof(char));
			int bytesRead = read(sock, additional_buffer, sizeof additional_buffer);	        
			if (opcode == TT_POWERON) {//two additional bytes
				uint16_t *xy_pair = (uint16_t*) buf;
				xy_pair[0] = (additional_buffer[1] << 8 | additional_buffer[0]);
				xy_pair[1] = (additional_buffer[3] << 8 | additional_buffer[2]);
				printf("assigned xy pair: (%d, %d)\n", xy_pair[0], xy_pair[1]);
			}
			else if (flag == TTFLAG_PROBE && distance == 0) {//one additional byte
				uint16_t *max_z = (uint16_t*) buf;
				max_z[0] = (additional_buffer[1] << 8 | additional_buffer[0]);
			}
			
			//free our memory after we're done using it like good little systems programmers
			free(additional_buffer);
		}
	}
	
	else if (flag == TTFLAG_EXTRUDE) {//then we ourselves have some bytes to send over (regardless of whether bytes received is > 0)
			//printf("we've encountered a wild extrude flag! (distance is %d)\n", distance);  
		//write our instruction to server (as always)
		int sendBufSize = 6 + abs(distance);

		unsigned char* send_buffer = malloc ( (sendBufSize) * sizeof(char));
		uint8_t matArrSize = abs(distance);
		initial_pack(send_buffer, insn, matArrSize);
		unsigned char *materials = (unsigned char*) buf;
		for (int i = 0; i < matArrSize; i++)
			send_buffer[6 + i] = materials[i];

		int numBytes = 0;
		if ((numBytes = write(sock, send_buffer, sendBufSize)) != sendBufSize ) {
			perror("write");
 			printf( "Error writing network data [%s]\n", strerror(errno) );
 			return -1;
 		} 
		//after it's been written, free up our memory like good systems programmers
		free(send_buffer);	

		unsigned char initial_buffer[4];
		int bytesRead =  read(sock, initial_buffer, sizeof initial_buffer); 	
		if (bytesRead != 4) {
			if (bytesRead != -1)
			printf("We've encountered a short read. 4 bytes not read... (%d received)\n", bytesRead);
			else {
 				printf( "Error reading network data [%s]\n", strerror(errno) );
			}
			return 1;
		}

		int returnValue = ntohs((initial_buffer[1] << 8) | initial_buffer[0]);
		if (returnValue != 0) {
			printf("nonzero ret val (%d) \n", returnValue);
			return 1;
		}

		int additionalBytes = ntohs((initial_buffer[3] << 8) | initial_buffer[2]);
		if (additionalBytes > 0) {
			printf("Unexpected additional bytes after extrude command\n"); 
			return 1;
		}

	}

	return 0;
}
