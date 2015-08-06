/**
 * @file
 * Demo application for the Cortex-M3 test platform.
 *
 * @author Shahzeb Ihsan
 * @version 1.0
 * @see platform.c
 */

/* Standard headers */
#include <stdint.h>

/* Log register definitions */
#define REG_LOG_TRIG       ((volatile unsigned char *) (0x00200001))
#define REG_LOG_BUFF       ((volatile unsigned char *) (0x00200000))

/* UART register definitions */
#define REG_UART_BASE      ((unsigned char *) 0x00100000)
#define REG_UARTDR         ((volatile unsigned char *) (REG_UART_BASE + 0))
#define REG_UARTFR         ((volatile unsigned char *) (REG_UART_BASE + 0x18))
#define REG_UARTLCR_H      ((volatile unsigned char *) (REG_UART_BASE + 0x2C))
#define REG_UARTCR         ((volatile unsigned char *) (REG_UART_BASE + 0x30))
#define REG_UARTRIS        ((volatile unsigned char *) (REG_UART_BASE + 0x3C))
#define REG_UARTIMSC       ((volatile unsigned char *) (REG_UART_BASE + 0x38))
#define REG_UARTMIS        ((volatile unsigned char *) (REG_UART_BASE + 0x40))

/* UARTFR bit definitions */
#define REG_BIT_RXFE       (0x01 << 4)

/* Program definitions */
#define CHAR_CTRL_D        (0x04)

/* Local constants */
const char * const str_out = "Hello, OVP!!!";


/**
 * @brief Uses the platform's debug memory region to print
 *        debug messages
 *
 * @param str String to print out.
 */
void dbg_print(char * str)
{
   unsigned char index = 0;

   /* Set the debug trigger. */
   *REG_LOG_TRIG = 0x00;
   do
   {
     *REG_LOG_BUFF = *(str + index++);
   } while(*(str + index) != '\0');

   /* Terminate the string. */
   *REG_LOG_BUFF = '\0';

   /* Clear the trigger. */
   *REG_LOG_TRIG = 0xFF;
}

/**
 * @brief Concat string 1 and string 2 and copy to buff.
 *
 * @param buff Target buffer.
 * @str1 First string.
 * @str2 Second string.
 */
void
concat_str(char * buff, char * str1, char * str2)
{
   uint32_t i = 0, j;

   /* Copy the first string. */
   j = 0;
   while(str1[j] != '\0')
   {
      buff[i++] = str1[j++];
   }

   /* Copy the second string. */
   j = 0;
   while(str2[j] != '\0')
   {
      buff[i++] = str2[j++];
   }

   /* Terminate the string. */
   buff[i] = '\0';
}

/**
 * @brief Convert integer to string.
 *
 * @param buff Buffer where to copy the string.
 * @param i The integer to convert.
 */
void
itos(char * buff, uint32_t i)
{
   uint8_t j, k = 0;

   do
   {
      j = i % 10;
      i = i / 10;
      buff[k++] = j + 0x30;
   } while(i / 10);

   /* Terminate the string. */
   buff[k] = '\0';
}

/**
 * @brief Application entry point, initializes the UART and echoes
 *        any data it receives through the UART.
 *
 * @return Exit status.
 */
int
main(void)
{
   char c;
   uint32_t i;
   uint32_t char_cnt = 0;;
   uint8_t temp[32], buff[128];

   /* UART initialization. */
   *REG_UARTLCR_H = 0x10;
   *REG_UARTCR |= 0x11;

   /* Program loop, terminates when its receives Ctrl-D. */
   dbg_print(">>> Starting echo program...");
   while(1)
   {
      /* Wait to receive a character. */
      while((*REG_UARTFR & REG_BIT_RXFE) != 0)
      {
         i = 100;
         while(--i);
      }

      /* Read the character. */
      c = *REG_UARTDR;

      /* Break out of the loop if the received character
       * is Ctrl-D. */
      if(c == CHAR_CTRL_D)
      {
         dbg_print(">>> Ctrl-D recieved, terminating program...");
         break;
      }

      /* Echo the character. */
      *REG_UARTDR = c;
      char_cnt++;
   }

   itos(temp, char_cnt);
   concat_str(buff, ">>> Number of characters received: ", temp);
   dbg_print(buff);
   return (0);
}
