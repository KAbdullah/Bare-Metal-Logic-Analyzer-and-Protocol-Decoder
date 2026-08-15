#include <stdint.h>

/* Test variables to check memory sections */
volatile uint32_t val_data = 0xDEADBEEF; // Goes to .data
volatile uint32_t val_bss;               // Goes to .bss

int main(void) {
    val_bss = 0x12345678;
    
    while (1) {
        val_data++;
    }
}