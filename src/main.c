#include <stdint.h>

void I2C1_EV_IRQHandler (void);

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
#define RCC_APB1ENR (*((volatile uint32_t *)(RCC + 0x40)))

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


  //Turn on I2C clock
  RCC_APB1ENR |= (1 << 21);

  //STARTING THE I2C
  //Program the peripheral input clock
  I2C1->I2C_CR2 &= ~(0b11111 << 0);
  I2C1->I2C_CR2 |= (0b10000 << 0);
  
  //Configure the clock control registers
  //reset
  I2C1->I2C_CCR &= ~(0xFFFF << 0);
  //Select FM mode
  I2C1->I2C_CCR |= (1 << 15);
  
  //Select DUTY, since 16MHz not multiple of 10Mhz, we do 2
  I2C1->I2C_CCR &= ~(1 << 14);
  I2C1->I2C_CCR |= (0 << 14);

  //Set CCR, how? A few things to clear up: 
  // 1) Tlow and Thigh indicate how many ticks aka clocks that they need to stay at their respective levels, so Tlow = 2Thigh means Tlow will be at that level for two PCLK (peripheral clock cycles)
  // 2) Ttotal = Tlow + Thigh 
  // 3) CCR = Ttotal / TPCLK => CCR = Total / 3 * TPCLK
  I2C1->I2C_CCR |= (14 << 0);

  //Set TRISE -> 300 / 62.5 = 4.8 => 4 + 1 => 5; 4 * 62.5  = 250ns or 4 SYSCLK ticks is the max safe limit to rise from 0 to 1
  I2C1->I2C_TRISE &= ~(0b111111 << 0);
  I2C1->I2C_TRISE |= (0b000101);

  //Enable interrupt ITEVTEN and ITBUFEN
  I2C1->I2C_CR2 |= (1 << 9) | (1 << 10);

  //Peripheral start
  I2C1->I2C_CR1 |= (1 << 0);

  __asm("cpsie i"); // Change Processor State to enable interrupt 
  //0xE000E100 is the NVIC base address 
  *((volatile uint32_t *)0xE000E100) |= (1 << 31); //I2C interrupt is number 31, so enable it to 1.

  //Start condition to get into controller mode
  I2C1->I2C_CR1 |= (1 << 8);

  while(1) {

  }

}

void I2C1_EV_IRQHandler (void) {
  //I do only 1, then everything else is set to 0, so 0b0000000000000001
  if (I2C1->I2C_SR1 & (1 << 0)) {
    (void)I2C1->I2C_SR1;
    //THE SGP30 uses 7 bits addressing, so we send the address starting at bit 1 and reserve the LSB as 0 (reset) to enter transmitter mode
    I2C1->I2C_DR = (0x58 << 1);
  }
}