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

//Probably not this, because it's GPIOB, but I'll check
#define GPIOB ((volatile GPIO_Struct *) 40020400)

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



  //Starting the I2C
  //enable peripheral
  I2C1->I2C_CR1 |= (1 << 0);
  //set start condition to go into controller mode
  I2C1->I2C_CR1 |= (1 << 8);



  //Start condition to get into controller mode



}