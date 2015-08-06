/**
 * @file
 * Header for the Cortex-M3 test platform.
 *
 * @author Shahzeb Ihsan
 * @version 1.0
 */
#ifndef __CORTEX_M3_TEST_PLATFORM_H
#define __CORTEX_M3_TEST_PLATFORM_H

/** @brief Configuration option for enabling instruction tracing. */
#define SIM_TRACE_INSTR     (0)

/* Boolean type definition */
typedef unsigned char         bool_t;
#define TRUE   (1 == 1)
#define FALSE  (1 == 0)

/* Integer type definitions */
typedef signed char           sint8_t;
typedef unsigned char         uint8_t;
typedef signed short int      sint16_t;
typedef unsigned short int    uint16_t;
typedef signed int            sint32_t;
typedef unsigned int          uint32_t;

/** @brief Command line options structure. */
struct __options
{
   char *   application;
   char *   cpu_var;
   bool_t   app_found;
   bool_t   enable_uart_stdout;
   bool_t   enable_uart_port;
   bool_t   enable_uart_logfile;
   bool_t   enable_gdb_debug;
   char *   uart_logfile;
   uint16_t uart_portnum;
   uint16_t gdb_portnum;
};

typedef struct __options options_t;

/** @brief Debug hook structure. */
struct __dbg_hook
{
   char buff[128];
   bool_t begin;
   uint8_t index;
};

typedef struct __dbg_hook dbg_hook_t;

/* Function prototypes */
static void print_usage(void);
static void parse_args(int argc, char **argv);
static void create_platform(icmProcessorP * cpu);
static void dbg_write_hook(icmProcessorP cpu, Addr start_addr, uint32_t num_bytes,
                           const void * value, void * user_data, Addr end_addr);

/* Enable relaxed scheduling for maximum performance. */
#if(SIM_TRACE_INSTR == 0)
/** @brief Default simulation options. */
#define SIM_ATTRS (/*ICM_ATTR_SIMEX |*/ ICM_ATTR_RELAXED_SCHED)
#else    /* (SIM_TRACE_INSTR == 0) */
/** @brief Simulation options for instruction tracing. */
#define SIM_ATTRS (/*ICM_ATTR_SIMEX |*/ ICM_ATTR_TRACE_REGS | ICM_ATTR_TRACE | ICM_ATTR_RELAXED_SCHED)
#endif   /* (SIM_TRACE_INSTR == 1) */

/* Memory and peripheral start addresses and sizes. */
#define MEM_INSTR_SIZE           (0x00040000 - 1)
#define MEM_INSTR_START_ADDR     (0x00000000)

#define MEM_DATA_SIZE            (0x00010000 - 1)
#define MEM_DATA_START_ADDR      (0x00040000)

#define PER_UART_START_ADDR      (0x00100000)
#define PER_UART_END_ADDR        (0x00100fff)

#define MEM_LOG_SIZE             (0x02 - 1)
#define MEM_LOG_START_ADDR       (0x00200000)
#define MEM_LOG_END_ADDR         (0x00200001)
#define MEM_LOG_TRIG             (MEM_LOG_END_ADDR)

/* Constants related to command-line arguments. */
#define ARGS_NUM_MIN                (2)
#define DEFAULT_UART_PORT_NUM       (17231)
#define DEFAULT_GDB_PORT_NUM        (9999)
#define DEFAULT_CPU_VAR_STR         "Cortex-M3"
#define DEFAULT_GDB_PORT_STR        "9999"
#define DEFAULT_UART_PORT_STR       "17321"
#define DEFAULT_UART_LOGFILE_STR    "uart.log"

#endif   /* __CORTEX_M3_TEST_PLATFORM_H */
