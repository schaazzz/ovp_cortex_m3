/**
 * Startup assembler file for the Cortex-M3 test platform.
 *
 * Author: Shahzeb Ihsan
 * Version: 1.0
 * See: cortex_m3_test_app.c
 */

   /* Thumb-2 instructions are only supported in unified syntax mode. */
   .syntax unified

   /* Vector table definition. */
   .section "__cs3_int_vectors"
.long __cs3_stack                   /* Top of the Stack */
.long  reset_handler                /* Reset handler */
.long  nmi_handler                  /* Non-maskable interrupt handler */
.long  hard_fault_handler           /* Hard fault handler */
.long  mpu_fault_handler            /* Memory protection unit fault handler */
.long  bus_fault_handler            /* Bus fault handler */
.long  usage_fault_handler          /* Usage fault handler */
.long  0                            /* Reserved */
.long  0                            /* Reserved */
.long  0                            /* Reserved */
.long  0                            /* Reserved */
.long  svc_handler                  /* Supervisor call handler */
.long  debug_mon_handler            /* Debug monitor handler */
.long  0                            /* Reserved */
.long  pendsv_handler               /* PendSV handler */
.long  sys_tick_handler             /* System tick handler */

   /* Vendor specific interrupts. */
   /*  ---- Not implemented --- */

   /* Program section. */
   .section ".text"

   /* Declare as thumb function, otherwise it won't be linked correctly. */
   .thumb_func

   /* Export the symbol so the linker can see this. */
   .global reset_handler
reset_handler:
   /* Jump to main(), a thumb function. */
   ldr      r0, =main
   blx      r0

   /* If main() ever exits, the "_exit" label will end the simulation. */
 .global _exit
_exit:
   B        .

   /* Dummy interrupt handlers. */
   .thumb_func
nmi_handler:
   .thumb_func
hard_fault_handler:
   .thumb_func
mpu_fault_handler:
   .thumb_func
bus_fault_handler:
   .thumb_func
usage_fault_handler:
   .thumb_func
svc_handler:
   .thumb_func
debug_mon_handler:
   .thumb_func
pendsv_handler:
   .thumb_func
sys_tick_handler:
   /* Loop here forever. */
   B        .
