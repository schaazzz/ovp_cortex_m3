/**
 * Linker script, specifies the output format and entry point and
 * sets-up the memory layout.
 *
 * Author: Shahzeb Ihsan
 * Version: 1.0
 */

/* Set the default output format as little endian. */ 
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

/* Target architecture. */
OUTPUT_ARCH(arm)

/* Set the reset handler as the entry point. */
ENTRY(reset_handler)

/* Setup the memory layout for the platform. */
SECTIONS
{
   /* Flash at 0x0000000.*/
   .flash 0x00000000 :
   {
      /* Collect the entire "__cs3_int_vectors" section to the start of the flash */
      KEEP(*(__cs3_int_vectors));

      /* The rest of the executable parts go after the interrupt vectors. */
      *(.text .text.*);
 
      /* Finally, place all read-only (constant) data in the flash as well. */
      *(.rodata .rodata.*);
 
      /* Initialization data for variables will be placed after this. */
      __init_data = .;
   }

   /* This section refers to variables with init value. The initialization
    * values are stored on ROM, but addressed on RAM (otherwise the variables
    * cannot be manipulated).
    *
    * This section will be loaded at 0x10000000.
    * TODO: [Shahzeb] We probably need to copy data from flash to this address.
    */
   .data_at_ram 0x00040000: AT (__init_data)
   {
      __sdata = .;
      *(.data .data.*);
      __edata = .;
   }

   .ram :
   {
      __sbss = .;
      
      /* Collect global/static variables here (without init value here). */
      *(.bss .bss.*);
      __ebss = .;
   }

   /* Set __cs3_stack to point the end of memory. */
   __cs3_stack = 0x00040000 + (16 * 1024);
}
