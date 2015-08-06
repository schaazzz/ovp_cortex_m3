/**
 * @file
 * Test OVP simulation platform which contains a Cortex-M3 processor, a single memory
 * for code and data, and an ARM PrimeCell PL011 UART.
 *
 * @author Shahzeb Ihsan
 * @version 1.0
 */

/* Include standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Include OVP headers */
#include <icm/icmCpuManager.h>

/* Include local headers */
#include "cortex_m3_test_platform.h"

/** @brief Platform executable name. */
const char * const platform = "cortex_m3_test_platform.Windows.exe";

/** @brief Platform command line options. */
const char * const usage = "\r\nOptions..."
                           "\r\n-e<application>             Application to be executed by the platform.\r\n"
                           "\r\n-c<processor>               Optional argument, default processor variant is set to \""DEFAULT_CPU_VAR_STR"\".\r\n"
                           "\r\n-d<port (optional)>         Debug the application using GDB. Port number is optional, default is set to \""DEFAULT_GDB_PORT_STR"\".\r\n"
                           "\r\n-s                          Use \"stdout\" for UART output.\r\n"
                           "\r\n-p<port (optional)>         Use a network connection for UART output. Port number is optional, default is"
                           "\r\n                            set to\""DEFAULT_UART_PORT_STR"\".\r\n"
                           "\r\n-f<filename (optional)>     Use a log file for UART output. File name is optional, by default \""DEFAULT_UART_LOGFILE_STR"\""
                           "\r\n                            is created in the current directory.\r\n";

/** @brief Platform options structure initialization. */
static options_t opts = { .application          = NULL,
                          .cpu_var              = DEFAULT_CPU_VAR_STR,
                          .app_found            = FALSE,
                          .enable_uart_stdout   = FALSE,
                          .enable_uart_port     = FALSE,
                          .enable_uart_logfile  = FALSE,
                          .enable_gdb_debug     = FALSE,
                          .uart_logfile         = ".\\"DEFAULT_UART_LOGFILE_STR,
                          .uart_portnum         = DEFAULT_UART_PORT_NUM,
                          .gdb_portnum          = DEFAULT_GDB_PORT_NUM };

/** @brief Debug hook structure initialization. */
static dbg_hook_t dbg_msg = {.index = 0, .begin = FALSE};

/**
 * @brief Simulation entry point.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @return Simulation status.
 */
int
main(int argc, char **argv)
{
   icmProcessorP cpu;

   /* Parse command-line arguments. */
   parse_args(argc, argv);

   /* Initialize OVPsim, enabling verbose mode to get statistics at
    * the end of execution. */
   if(opts.enable_gdb_debug)
   {
      icmInit((ICM_VERBOSE | ICM_STOP_ON_CTRLC), "rsp", opts.gdb_portnum);
   }
   else
   {
      icmInit((ICM_VERBOSE | ICM_STOP_ON_CTRLC), NULL, 0);
   }
   /* Create the platform. */
   create_platform(&cpu);

   /* Load the application executable file into processor memory space. */
   if(!icmLoadProcessorMemory(cpu, opts.application, False, False, False))
   {
      return (-1);
   }

   /* Simulate the platform. */
   icmPrintf("Info ====================================================\n");
   icmPrintf("Info >>> Simulation: STARTED\n");
   icmPrintf("Info ====================================================\n");
   icmProcessorP final = icmSimulatePlatform();

   /* Check simulation status. */
   if(final &&
      (icmGetStopReason(final) == ICM_SR_INTERRUPT))
   {
      icmPrintf("Info ====================================================\n");
      icmPrintf("Info >>> Simulation: INTERRUPTED\n");
      icmPrintf("Info ====================================================\n");
   }
   else
   {
      icmPrintf("Info ====================================================\n");
      icmPrintf("Info >>> Simulation: FINISHED\n");
      icmPrintf("Info ====================================================\n");
   }

   /* Free the processor. */
   icmFreeProcessor(cpu);

   return (0);
}

/**
 * @brief Parse command-line arguments.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 */
static void
parse_args(int argc, char **argv)
{
   /* Check if the minimum number of arguments has
    * been specified. */
   if(argc < ARGS_NUM_MIN)
   {
      icmPrintf("\r\nWrong number of arguments.");
      print_usage();
      exit(1);
   }

   /* Parse command line arguments. */
   while ((argc > 1) && (argv[1][0] == '-'))
   {
      switch (argv[1][1])
      {
         /* Parse the application name. */
         case 'e':
            opts.app_found = TRUE;
            opts.application = &argv[1][2];
            break;

         /* Parse the CPU variant. */
         case 'c':
            opts.cpu_var = &argv[1][2];
            break;

         /* Parse GDB debugging options. */
         case 'd':
            opts.enable_gdb_debug = TRUE;

            if(argv[1][2] != '\0')
            {
               opts.gdb_portnum = atoi(&argv[1][2]);
            }
            break;

         /* Parse UART output options. */
         case 's':
            opts.enable_uart_stdout = TRUE;
            break;

         case 'p':
            opts.enable_uart_port = TRUE;

            if(argv[1][2] != '\0')
            {
               opts.uart_portnum = atoi(&argv[1][2]);
            }
            break;

         case 'f':
            opts.enable_uart_logfile = TRUE;

            if(argv[1][2] != '\0')
            {
               opts.uart_logfile = &argv[1][2];
            }
            break;

         default:
            icmPrintf("\r\nUnknown argument: %s", argv[1]);
            print_usage();
            exit(1);
            break;
      }

      ++argv;
      --argc;
   }

   /* Check if the application was specified. */
   if(!opts.app_found)
   {
      icmPrintf("\r\nNo application specified.");
      print_usage();
      exit(1);
   }
}

/**
 * @brief Print usage string.
 */
static void
print_usage(void)
{
   icmPrintf("\r\n\r\nUsage: %s -e<application> ...optional arguments... %s", platform, usage);
}

/**
 * @brief Debug memory region write callback.
 *
 * @param cpu Processor which triggered the write.
 * @param start_addr Start address of the write.
 * @param num_bytes Number of bytes written.
 * @param value Value written to the memory.
 * @param user_data User data for the callback.
 * @param end_addr End address of the write.
 */
static void
dbg_write_hook(icmProcessorP cpu, Addr start_addr, uint32_t num_bytes,
               const void * value, void * user_data, Addr end_addr)
{
   uint8_t * data_ptr;
   uint8_t index = 0;

   /* Initialize the data pointer. */
   data_ptr = (uint8_t *)value;

   /* Check if a write was triggered. */
   if((MEM_LOG_TRIG == (uint32_t)start_addr) &&
      (*data_ptr == 0x00))
   {
      dbg_msg.index = 0;
      dbg_msg.begin = TRUE;
   }
   /* Print out the debug string if the trigger was cleared. */
   else if((MEM_LOG_TRIG == (uint32_t)start_addr) &&
           (*data_ptr == 0xFF))
   {
      dbg_msg.begin = FALSE;
      icmPrintf("\r\nInfo (DBG_STR) ");
      do
      {
         icmPrintf("%c", dbg_msg.buff[index++]);
      } while(dbg_msg.buff[index] != '\0');
      icmPrintf("\r\n");
   }
   /* If a write has been triggered fill-up the buffer. */
   else if(dbg_msg.begin)
   {
      dbg_msg.buff[dbg_msg.index++] = *data_ptr;
   }
}

/**
 * @brief Create the platform.
 *
 * @param cpu Pointer to the processor.
 */
static void
create_platform(icmProcessorP * cpu)
{
   const char * arm_model;
   const char * arm_semihost;
   const char * uart_pse;
   const char * uart_model;

   char cpu_name[64];

   icmPseP uart;
   icmBusP cpu_bus;
   icmMemoryP dbg_mem;
   icmMemoryP data_mem;
   icmMemoryP instr_mem;
   icmAttrListP icm_attr;
   icmAttrListP uart_attr;

   icmPrintf("Creating platform...");

   /* Setup processor's attributes. */
   /* TODO: [Shahzeb] Create a platform without semi-hosting. */
   arm_model = icmGetVlnvString(NULL, "arm.ovpworld.org", "processor", "armm", "1.0", "model");
   arm_semihost = icmGetVlnvString(NULL, "arm.ovpworld.org", "semihosting", "armNewlib", "1.0", "model");

   /* TODO: [Shahzeb] What is the significance of this attribute list? */
   icm_attr = icmNewAttrList();
   icmAddStringAttr(icm_attr, "endian", "little");
   icmAddStringAttr(icm_attr, "compatibility", "nopBKPT");
   icmAddStringAttr(icm_attr, "variant", opts.cpu_var);
   icmAddStringAttr(icm_attr, "showHiddenRegs", "1");
   icmAddStringAttr(icm_attr, "UAL", "1");

   /* Create a processor instance. */
   sprintf(cpu_name,"cpu-%s", opts.cpu_var);
   *cpu = icmNewProcessor(cpu_name,       /* CPU name */
                          "armm",         /* CPU type */
                          0,              /* CPU ID */
                          0,              /* CPU model flags */
                          32,             /* Address bits */
                          arm_model,      /* Model file */
                          "modelAttrs",   /* Morpher attributes */
                          SIM_ATTRS,      /* Simulation attributes */
                          icm_attr,       /* Custom attributes */
                          arm_semihost,   /* Semihosting file */
                          "modelAttrs"    /* Semihosting attributes */);

   /* Create processor bus. */
   cpu_bus = icmNewBus("bus", 32);

   /* Connect the processor to the bus. */
   icmConnectProcessorBusses(*cpu, cpu_bus, cpu_bus);

   /* Create instruction, data and debug memories. */
   instr_mem = icmNewMemory("instr_mem", ICM_PRIV_RX, MEM_INSTR_SIZE);
   data_mem = icmNewMemory("data_mem", ICM_PRIV_RWX, MEM_DATA_SIZE);
   dbg_mem = icmNewMemory("dbg_mem", ICM_PRIV_W, MEM_LOG_SIZE);

   /* Enable debugging for this processor. */
   if(opts.enable_gdb_debug)
   {
      icmDebugThisProcessor(*cpu);
   }

   /* Connect memories to the bus. */
   icmConnectMemoryToBus(cpu_bus, "port0", instr_mem, MEM_INSTR_START_ADDR);
   icmConnectMemoryToBus(cpu_bus, "port1", data_mem, MEM_DATA_START_ADDR);
   icmConnectMemoryToBus(cpu_bus, "port2", dbg_mem, MEM_LOG_START_ADDR);

   /* Get the PL011 UART model and PSE strings. */
   uart_pse = icmGetVlnvString(0,               /* Path (0 if from the product directory) */
                               0,               /* Vendor */
                               0,               /* Library */
                               "UartPL011",     /* Name */
                               0,               /* Version */
                               "pse"            /* Model */);

   uart_model = icmGetVlnvString(0,             /* Path (0 if from the product directory) */
                                 0,             /* Vendor */
                                 0,             /* Library */
                                 "UartPL011",   /* Name */
                                 0,             /* Version */
                                 "model"        /* Model */);

   /* Initialize UART attribues. */
   uart_attr = icmNewAttrList();

   /* Configure UART output mode(s). */
   if(opts.enable_uart_port)
   {
      icmAddDoubleAttr(uart_attr, "portnum", opts.uart_portnum);
      icmAddStringAttr(uart_attr, "finishOnDisconnect", "on");
   }

   if(opts.enable_uart_logfile)
   {
      icmAddStringAttr(uart_attr, "outfile", opts.uart_logfile);
      icmAddStringAttr(uart_attr, "infile", "in.txt");
   }

   if(opts.enable_uart_stdout)
   {
      icmAddUns64Attr(uart_attr, "log", 3);
   }

   icmAddStringAttr(uart_attr, "variant", "ARM");

   /* Create UART instance. */
   uart = icmNewPSE("uart",         /* Peripheral name */
                    uart_pse,       /* Peripheral model */
                    uart_attr,      /* Peripheral attribute list */
                    uart_model,     /* Semihosting file */
                    "modelAttrs"    /* Semihosting attributes */);

   /* Set UART peripheral diagnostic level. */
   icmSetPSEdiagnosticLevel(uart, 3);

   /* Connect the UART to the processor using the bus. */
   icmConnectPSEBus(uart, cpu_bus, "bport1", 0, PER_UART_START_ADDR, PER_UART_END_ADDR);

   /* Setup the callback for the debug memory region. */
   icmAddWriteCallback(*cpu, MEM_LOG_START_ADDR, MEM_LOG_END_ADDR, dbg_write_hook, 0);
}
