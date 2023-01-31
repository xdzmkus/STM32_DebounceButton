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
		HAL_GPIO_WritePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin, GPIO_PIN_RESET);
		break;
	case BUTTON_EVENT::Released:
		HAL_GPIO_WritePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin, GPIO_PIN_SET);
		break;
	default:
		break;
	}
}

void appMain()
{
	DebounceButton key(GPIOA, GPIO_PIN_0, BUTTON_CONNECTED::GND);

	key.initPin();
	key.setEventHandler(handleButtonEvent);

	/* Infinite loop */
	while (1)
	{
		key.check();
	}
}
