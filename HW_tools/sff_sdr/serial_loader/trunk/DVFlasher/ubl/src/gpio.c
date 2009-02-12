/* gpio.c - GPIO functions */

#include "davinci.h"
#include "util.h"

struct gpio_controller *
gpio_to_controller(unsigned gpio)
{
  void *ptr;

  if (gpio < 32 * 1)
    ptr = (void *) DAVINCI_GPIO_BASE + 0x10;
  else if (gpio < 32 * 2)
    ptr = (void *) DAVINCI_GPIO_BASE + 0x38;
  else if (gpio < 32 * 3)
    ptr = (void *) DAVINCI_GPIO_BASE + 0x60;
  else if (gpio < 32 * 4)
    ptr = (void *) DAVINCI_GPIO_BASE + 0x88;
  else
    ptr = NULL;

  return ptr;
}

inline uint32_t gpio_mask(unsigned gpio)
{
  return 1 << (gpio % 32);
}

int gpio_direction_out(unsigned gpio, int value)
{
  volatile struct gpio_controller *g = gpio_to_controller(gpio);
  uint32_t temp;
  uint32_t mask = gpio_mask(gpio);

  temp = g->dir;
  temp &= ~mask;
  if (value)
    g->set_data = mask;
  else
    g->clr_data = mask;

  g->dir = temp;

  return 0;
}
