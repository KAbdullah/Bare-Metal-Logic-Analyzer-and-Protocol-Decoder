#include <stdint.h>

typedef struct {
  volatile uint32_t GPIOx_MODER;
  volatile uint32_t GPIOx_OTYPER;
  volatile uint32_t GPIOx_OSPEEDR;
  volatile uint32_t GPIOx_PUPDR;
  volatile uint32_t GPIOx_IDR;
  volatile uint32_t GPIOx_ODR;
  volatile uint32_t GPIOx_BSRR;
  volatile uint32_t GPIOx_LCKR;
  volatile uint32_t GPIOx_AFRH;
  volatile uint32_t GPIOx_AFRL;
} GPIO_Struct;


int main (void) {

}