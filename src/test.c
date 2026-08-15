#include <stdint.h>

/* Forward declaration of main */
int main(void);

/* Dummy Reset Handler that calls main */
void __attribute__((naked, noreturn)) Reset_Handler(void) {
    main();
    while (1);
}

/* Force this into the .isr_vector section so the linker finds it */
__attribute__((section(".isr_vector"), used))
const uint32_t vector_table[] = {
    0x20002000, 
    (uint32_t)&Reset_Handler // Stack pointer, then Reset Handler entry
};

/* Test variables to check memory sections */
volatile uint32_t val_data = 0xDEADBEEF; // Goes to .data
volatile uint32_t val_bss;               // Goes to .bss

int main(void) {
    val_bss = 0x12345678;
    
    while (1) {
        val_data++;
    }
}