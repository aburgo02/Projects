/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                   *
 *                                 um                                *
 *                                                                   *
 *                File: um.c                                         *
 *             Authors: Andrew Burgos and Lucy Qin                   *
 *       Date Modified: November 13, 2014                            *
 *             Purpose: Initializes the UM and interacts with        *
 *                      various modules to execute                   *
 *                      UM instructions as well as interact with     *
 *                      the segmented memory.                        *
 *                                                                   *
 *                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "uarray.h"
#include "bitpack.h"

/*   U M   M O D U L E S   */
#include "managemem.h"
#include "alu.h"
#include "io.h"
#include "decoder.h"
#define C codeword & 7
#define B (codeword >> 3) & 7
#define A (codeword >> 6) & 7


struct Memory {
        Seq_T segments;
        Seq_T unused_ids;
};

/* * * * * * * * * * * * * * * * * * * * * * * * *
 *   F U N C T I O N   D E C L A R A T I O N S   *
 * * * * * * * * * * * * * * * * * * * * * * * * */

//static void initialize_registers (UArray_T registers);

static void read_file(int argc, char *argv[], 
                      UArray_T registers, Memory mem);

/*static void execute_instruction(instruction decoded, UArray_T registers, 
                                Memory mem, uint32_t *program_counter);
*/
static void free_um_memory(UArray_T registers, Memory mem);


/* * * * * * * * * * * * * * * * * * 
 *   I M P L E M E N T A T I O N   *
 * * * * * * * * * * * * * * * * * */



int main(int argc, char *argv[]) 
{
        /* initialize segmented memory */
        Memory mem = initialize_memory();        
        
        /* create registers */
        UArray_T registers = UArray_new(8, sizeof(uint32_t));  
    //    uint32_t $r[8];
        /* initialize program counter */
        uint32_t pc_value = 0;
        uint32_t *program_counter = &pc_value;
        
    
        
        read_file(argc, argv, registers, mem);

//        initialize_registers(registers);

        /* load in 32-bit instructions */
	
	UArray_T segment_zero = Seq_get(mem->segments, 0);
	 
        Um_instruction codeword = *((Um_instruction*)UArray_at(segment_zero,*program_counter));
 //       instruction decoded = malloc(sizeof(*decoded));
	
	unsigned opcode = 0;
	
        
               


        while ( opcode != HALT) {
	 
	 //((Um_instruction*)program_counter = UArray_at(segment, *program_counter); 
//	 decode(codeword, decoded);
//	  fprintf(stderr, "opcode is: %u\n", opcode);
//	  fprintf(stderr, "orig rb is: %u\n", rb);
//	  uint32_t *program_counter = UArray_at(segment, pc_value);
	  unsigned raval, value;
	  opcode = (codeword >> 28) & 15;
	  if(opcode == 13){
	   value = codeword & 33554431;
 //          fprintf(stderr, "value is : %u\n", value);
	   raval = (codeword >> 25) & 7;
	   
	  } 
	  /*
	  else{
	    rc = codeword & 7;
	    rb = (codeword >> 3) & 7;
//	    fprintf(stderr, "rb is : %u\n", rb);
	    ra = (codeword >> 6) & 7;
	  }*/
	    
	
	
	  
          switch ( opcode ) {
                case 0:
		//  printf("in case 0");
			//if ($r[C]) {$r[A] = $r[B];}
			  
                        cond_move(A,B,C, registers);
			  
                        break;
                case 1:
		    
                        segmented_load(A, B, C,
                                       registers, mem);
                        break;
                case 2: 
                        segmented_store(A, B, C,
                                        registers, mem);
                        break;
                case 3:
                        addition(A, B, C, 
                                 registers); 
                        break;
                case 4:
                        multiply(A, B, C, 
                                 registers);
                        break;
                case 5: 
                        division(A, B, C,
                                 registers);
                        break;
                case 6: 
                        nand(A, B, C, registers);
                        break;
                case 7: 
                        return 0;
                case 8: 
		 
                        map_segment(B, C, registers, mem);
                        break;
                case 9: 
		   
                        unmap_segment(C, registers, mem);
                        break;
                case 10: 
                        output(C, registers);
                        break;
                case 11: 
                        input(C, registers);
                        break;
                case 12: 
                        load_program(B, C, registers, 
                                     mem, program_counter); 
                        break;
                case 13: 
                        load_value(raval, value, registers);
                        break;
        }
        
        if (opcode != 12) {
                *program_counter = *program_counter +1;
        
        }
	

            //   execute_instruction(decoded, registers, mem, program_counter);
        codeword = *((Um_instruction*)UArray_at(segment_zero,*program_counter));
              
              
   //             decode(codeword, decoded);
      }    
  
        free_um_memory(registers, mem);
   
        
  
}
/*
static void initialize_registers(UArray_T registers) 
{
        int i;
        for (i = 0; i < UArray_length(registers); i++) {
                int *value = UArray_at(registers, i);
                *value = 0; 
        }
}
*/

static void read_file(int argc, char *argv[], UArray_T registers, Memory mem) 
{
        if (argc != 2) {
                fprintf(stderr, "Error: please specify one file\n");
                free_um_memory(registers, mem);
		exit(EXIT_FAILURE);
        }


        FILE *file_ptr = fopen(argv[1], "r");

        struct stat file_stats;

        if(stat(argv[1], &file_stats) == -1 || file_stats.st_size % 4 != 0) {
                fprintf(stderr, "Error within file\n");
		exit(EXIT_FAILURE);
        }
        /*
        if (file_stats.st_size % 4 != 0) {
                fprintf(stderr, "Error: File does not contain");
                fprintf(stderr, "correctly formatted instruction\n");
                exit(EXIT_FAILURE);
                fclose(file_ptr);
                free_um_memory(registers, mem);
                
        }
        */
        load_value(0, (file_stats.st_size / 4), registers);
        map_segment(1, 0, registers, mem);

        uint32_t instruct = 0;
        uint32_t instruct_byte;
        int instruction_count = 0;
        unsigned lsb = 32;

        while((instruct_byte = getc(file_ptr)) != (unsigned)EOF) {
           
                lsb = lsb - 8;
                instruct = Bitpack_newu(instruct, 8, lsb, instruct_byte);
                
                if (lsb == 0) {
                        load_value(0, instruct, registers);
                        int *value2 = UArray_at(registers, 2);
                        *value2 = instruction_count++;
                        segmented_store(1, 2, 0, registers, mem);    
                        lsb = 32;
                }
        }
        fclose(file_ptr);
}


static void free_um_memory(UArray_T registers, Memory mem) 
{
        UArray_free(&registers);
        free_memory(mem);
        

        
        Seq_free(&(mem->segments));
        Seq_free(&(mem->unused_ids));
        
        free(mem);
}

