/*
 * appMain.cpp
 *
 *  Author: xdzmkus
 */

#include "appMain.h"
#include "main.h"
#include "DebounceButton.h"

void handleButtonEvent(const DebounceButton* button, BUTTON_EVENT eventType)
{
	switch (eventType)
	{
	case BUTTON_EVENT::Pressed:
		if (button->getGPIOx() == KEY1_GPIO_Port && button->getPin() == KEY1_Pin)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		}
		if (button->getGPIOx() == KEY2_GPIO_Port && button->getPin() == KEY2_Pin)
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		}
		break;
	case BUTTON_EVENT::Released:
		if (button->getGPIOx() == KEY1_GPIO_Port && button->getPin() == KEY1_Pin)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		}
		if (button->getGPIOx() == KEY2_GPIO_Port && button->getPin() == KEY2_Pin)
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		}
		break;
	default:
		break;
	}
}

void appMain()
{
	DebounceButton key1(KEY1_GPIO_Port, KEY1_Pin, BUTTON_CONNECTED::GND);
	DebounceButton key2(KEY2_GPIO_Port, KEY2_Pin, BUTTON_CONNECTED::GND);

	key1.setEventHandler(handleButtonEvent);
	key2.setEventHandler(handleButtonEvent);

	/* Infinite loop */
	while (1)
	{
		  key1.check();
		  key2.check();
	}
}
