#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

// System includes
#include <stm32f410rx.h>
#include <stm32f4xx.h>

// User includes
#include <user_types.h>
#include <user_functions.h>
#include <delay.h>
#include <timer.h>
#include <ultrasonic_sensor.h>
#include <stdint.h>
// DEBUG
#include <debug_functons.h>
#include <stdio.h>

pin_struct_TypeDef trig_pin;
pin_struct_TypeDef echo_pin;

void clock_init()
{
  // Port B clock enable
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
}

void pin_init()
{
  trig_pin = pin_setup(GPIOB, PIN12, OUTPUT);
  echo_pin = pin_setup(GPIOB, PIN11, ALTERNATE);
  // set pb12 alternate function type to TIM5_CH1 (AF2 = 0010)
  SET_BIT(echo_pin.GPIOx->AFR[1], GPIO_AFRH_AFSEL11_1);
}

int main(void)
{
  clock_init();
  pin_init();
  tim5_pb11_input_capture();

  int dist_cm = 0;
  /* Loop forever */
  do
  {
    dist_cm = us_read_pb11_distance(trig_pin);
  } while (true);
}
