#if !defined(USER_FUNCTIONS_H)
#define USER_FUNCTIONS_H

#include <stm32f410rx.h>
#include <stm32f4xx.h>

#include <user_types.h>
#include <delay.h>
#include <adc.h>

bool read_pin(pin_struct_TypeDef &pin, pin_state_TypeDef state_bit)
{
    uint32_t bit_mask = 0x1U << pin.pinx;
    if (state_bit == LOW)
    {
        bit_mask = ~bit_mask;
    }

    return READ_BIT(pin.GPIOx->IDR, bit_mask);
}

pin_struct_TypeDef pin_setup(GPIO_TypeDef *GPIOx, pin_TypeDef pinx, pin_mode_TypeDef mode)
{
    pin_struct_TypeDef pin_struct;
    pin_struct.GPIOx = GPIOx;
    pin_struct.pinx = pinx;
    pin_struct.mode = mode;

    uint32_t mode_pin = 2 * pinx;

    switch (mode)
    {
    case OUTPUT:
        SET_BIT(pin_struct.GPIOx->MODER, mode << mode_pin);
        break;
    case INPUT:
        CLEAR_BIT(pin_struct.GPIOx->MODER, mode << mode_pin);
        break;
    case ANALOG:
        // If pin is for analog
        if (check_analog_pin(pin_struct))
        {
            SET_BIT(pin_struct.GPIOx->MODER, mode << mode_pin);
        }
        break;
    case ALTERNATE:
        SET_BIT(pin_struct.GPIOx->MODER, mode << mode_pin);
        break;
    default:
        break;
    }

    return pin_struct;
}

void set_input_pull_mode(pin_struct_TypeDef &pin, pupd_mode_TypeDef input_mode)
{
    if (pin.mode != INPUT)
    {
        return;
    }
    switch (input_mode)
    {
    case NONE:
        CLEAR_BIT(pin.GPIOx->PUPDR, !NONE << 2 * pin.pinx);
        break;
    case PULLDOWN:
        SET_BIT(pin.GPIOx->PUPDR, PULLDOWN << 2 * pin.pinx);
        break;
    case PULLUP:
        SET_BIT(pin.GPIOx->PUPDR, PULLUP << 2 * pin.pinx);
        break;
    case RESERVED:
        SET_BIT(pin.GPIOx->PUPDR, RESERVED << 2 * pin.pinx);
        break;
    default:
        break;
    }
}

void digital_write(pin_struct_TypeDef &pin, pin_state_TypeDef mode)
{
    if (mode == HIGH)
    {
        uint32_t set_msk = 0x1U << pin.pinx;
        WRITE_REG(pin.GPIOx->BSRR, set_msk); // set pin
        return;
    }

    uint32_t reset_pin = (0x1F - (0xF - pin.pinx));
    uint32_t reset_msk = 0x1U << reset_pin;

    WRITE_REG(pin.GPIOx->BSRR, reset_msk); // reset pin
}

void led_array_decrement(pin_struct_TypeDef LED_pins[9], int &starting_position, int &delay)
{
    int i = starting_position;

    digital_write(LED_pins[i], HIGH);
    delay_ms(delay);
    digital_write(LED_pins[i], LOW);

    starting_position--;

    if (starting_position == -1)
    {
        starting_position = 8;
    }
}

void led_array_increment(pin_struct_TypeDef LED_pins[9], int &starting_position, int &delay)
{
    int i = starting_position;

    digital_write(LED_pins[i], HIGH);
    delay_ms(delay);
    digital_write(LED_pins[i], LOW);

    starting_position++;

    if (starting_position == 9)
    {
        starting_position = 0;
    }
}

uint32_t linear_distribution_12_bit(uint32_t input_value, uint32_t max_value)
{ // Takes 12bit input value and converts it linearly
    if (input_value == 5000)
    {
        return 0;
    }

    uint32_t linear_value = (input_value * max_value) / 4096;

    if (linear_value <= 9)
    {
        return 9; // Hard limit, if value aproaches 0
    }
    return linear_value;
}

// TODO: Implement pin toogle function

#endif // USER_FUNCTIONS_H
