#include <stdint.h>

typedef struct {
  uint32_t GPIOx_MODER;
  uint32_t GPIOx_OTYPER;
  uint32_t GPIOx_OSPEEDR;
  uint32_t GPIOx_PUPDR;
  uint32_t GPIOx_IDR;
  uint32_t GPIOx_ODR;
  uint32_t GPIOx_BSRR;
  uint32_t GPIOx_LCKR;
  uint32_t GPIOx_AFRL;
  uint32_t GPIOx_AFRH;
} GPIO_Struct;

//RCC - over here when we put the outer *, we're saying that I want to immediately start working in the memory
// and not work with a pointer that points to a memory, so in subsequent uses of RCC, I'm working with memory 
#define RCC 0x40023800

#define RCC_CR (*((volatile uint32_t *)(RCC + 0x00)))
#define RCC_CFGR (*((volatile uint32_t *)(RCC + 0x08)))
#define RCC_AHB1ENR (*((volatile uint32_t *)(RCC + 0x30)))

//Probably not this, because it's GPIOB, but I'll check
#define GPIOB ((volatile GPIO_Struct *) 0x40020400)

typedef struct {
  uint32_t I2C_CR1;
  uint32_t I2C_CR2;
  uint32_t I2C_OAR1;
  uint32_t I2C_OAR2;
  uint32_t I2C_DR;
  uint32_t I2C_SR1;
  uint32_t I2C_SR2;
  uint32_t I2C_CCR;
  uint32_t I2C_TRISE;
  uint32_t I2C_FLTR;
} I2C_Struct;

#define I2C1 ((volatile I2C_Struct *) 0x40005400)

int main (void) {

  //Got to get the clock started before touching the pins, otherwise you exceptions flags called with interrupt 
  //Reset
  RCC_CR &= ~((0b111111 << 24) | (0b1111 << 16) | (0b10000 << 3) | (0b11 << 0));

  //Turn HSIO clock on
  RCC_CR |= (1 << 0);

  //Blocking until HSI starts
  while (!(RCC_CR & (1 << 1)));

  //Choose system clock to use HSI
  RCC_CFGR |= (0b00 << 0);

  //Blocking until sys switched to HSI
  while (1) {
    uint32_t address = RCC_CFGR;
    uint32_t extractsws = (address >> 2) & 0x3;
    if (extractsws == 0x00) {
      break;
    }
  }

  //Turn on GPIOB clock
  RCC_AHB1ENR |= (1 << 1);

  //Configuring the GPIOB Pins 5, 6, and 7, because those are the I2C AF ones
  //Reset the pins first to 00 each
  GPIOB->GPIOx_MODER &= ~((3 << 10) | (3 << 12) | (3 << 14));

  //Now set them to AF here
  GPIOB->GPIOx_MODER |= ((2 << 10) | (2 << 12) | (2 << 14));

  //Set to Open Drain
  GPIOB->GPIOx_OTYPER |= (1 << 5);
  GPIOB->GPIOx_OTYPER |= (1 << 6);
  GPIOB->GPIOx_OTYPER |= (1 << 7);

  //Output speed register, control the output speed
  //This speed doesn't matter, because the lowest it goes is 2 MHz, and the fastest I can set my I2C is 1MHz,
  //so doesn't matter how slow the GPIO pins are at outputting data to the I2C interface
  //Reset
  GPIOB->GPIOx_OSPEEDR &= ~((0b11 << 10) | (0b11 << 12) | (0b11 << 14));
  //Then Set
  GPIOB->GPIOx_OSPEEDR |= ((0b00 << 10) | (0b00 << 12) | (0b00 << 14));


  GPIOB->GPIOx_PUPDR &= ~((3 << 10) | (3 << 12) | (3 << 14));
  GPIOB->GPIOx_PUPDR |= ((1 << 10) | (1 << 12) | (1 << 14));

  GPIOB->GPIOx_AFRL |= ((0b0100 << 20) | (0b0100 << 24) | (0b0100 << 28));

  //Lock key write sequence
  GPIOB->GPIOx_LCKR = (1 << 16) | (1 << 5) | (1 << 6) | (1 << 7);
  GPIOB->GPIOx_LCKR = (0 << 16) | (1 << 5) | (1 << 6) | (1 << 7);
  GPIOB->GPIOx_LCKR = (1 << 16) | (1 << 5) | (1 << 6) | (1 << 7);
  (void)GPIOB->GPIOx_LCKR;


  //STARTING THE I2C
  //enable peripheral
  // I2C1->I2C_CR1 |= (1 << 0);
  // //set start condition to go into controller mode
  // I2C1->I2C_CR1 |= (1 << 8);



  //Start condition to get into controller mode

  while(1) {

  }

}