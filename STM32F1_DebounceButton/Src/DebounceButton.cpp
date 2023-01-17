#include "DebounceButton.h"

DebounceButton::DebounceButton(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BUTTON_CONNECTED c, BUTTON_NORMAL n)
	: gpioX(GPIOx), pin(GPIO_Pin), connectedTo(c), normalState(n)
{
}

DebounceButton::~DebounceButton()
{
}

void DebounceButton::initPin()
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	if(connectedTo == BUTTON_CONNECTED::VCC)
	{
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}
	else
	{
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}

	HAL_GPIO_Init(gpioX, &GPIO_InitStruct);
}

GPIO_TypeDef* DebounceButton::getGPIOx() const
{
	return gpioX;
}

uint16_t DebounceButton::getPin() const
{
	return pin;
}

void DebounceButton::setEventHandler(BtnEventHandler eventHandler)
{
	this->eventHandler = eventHandler;
}

void DebounceButton::clearEventHandler()
{
	this->eventHandler = nullptr;
}

bool DebounceButton::check()
{
	buttonFlags.currrentState = getCurrentState();

	uint32_t now = getTicks();

	// dispatch events if button is debounced
	if (debounce(now) && eventHandler)
	{
		processPressed(now);
		processReleased(now);
	}

	return buttonFlags.debouncedState;
}

bool DebounceButton::getCurrentState()
{
	return getPinState() ^ static_cast<bool>(connectedTo) ^ static_cast<bool>(normalState);
}

bool DebounceButton::getPinState()
{
	return static_cast<bool>(HAL_GPIO_ReadPin(gpioX, pin));
}

uint32_t DebounceButton::getTicks()
{
	return HAL_GetTick();
}

bool DebounceButton::debounce(uint32_t now)
{
	if (buttonFlags.currrentState == buttonFlags.previousState)
	{
		// check if button has the same state at least debounce delay 
		if (!buttonFlags.debounced && (now - lastDebounceTime) > delayDebounce)
		{
			buttonFlags.debounced = true;
			buttonFlags.debouncedState = buttonFlags.currrentState;
		}
	}
	else
	{
		// button state is changed - still bouncing
		buttonFlags.previousState = buttonFlags.currrentState;
		lastDebounceTime = now;
		buttonFlags.debounced = false;
	}

	return buttonFlags.debounced;
}

void DebounceButton::processPressed(uint32_t now)
{
	// check if button pressed
	if (buttonFlags.currrentState == false)
		return;

	// Released to Pressed pulse
	if (lastPressedTime <= lastReleasedTime)
	{
		lastPressedTime = now;
		lastReleasedTime = 0;

		eventHandler(this, BUTTON_EVENT::Pressed);

		buttonFlags.clicks++;

		return;
	}

	// check if button is long pressed
	if (buttonFlags.clicks > 0 && (now - lastPressedTime) > delayLongPress)
	{
		// decrement count of clicks
		if (buttonFlags.clicks == 2)
			eventHandler(this, BUTTON_EVENT::Clicked);
		else if (buttonFlags.clicks == 3)
			eventHandler(this, BUTTON_EVENT::DoubleClicked);

		eventHandler(this, BUTTON_EVENT::LongPressed);

		buttonFlags.clicks = 0; // reset clicks after long press
	}
}

void DebounceButton::processReleased(uint32_t now)
{
	// check if button released
	if (buttonFlags.currrentState == true)
		return;

	//  Pressed to Released pulse
	if (lastPressedTime > lastReleasedTime)
	{
		lastReleasedTime = now;
		eventHandler(this, BUTTON_EVENT::Released);

		// button is repeatedly pressed
		if (buttonFlags.clicks > 2)
		{
			eventHandler(this, BUTTON_EVENT::RepeatClicked);
		}

		return;
	}

	// process clicks
	if (buttonFlags.clicks == 0) return;

	// chek if repeated clicks are still possible
	if ((now - lastPressedTime) > delayRepeatedClick)
	{
		// no more repeated clicks possible
		if (buttonFlags.clicks == 1)
			eventHandler(this, BUTTON_EVENT::Clicked);
		else if (buttonFlags.clicks == 2)
			eventHandler(this, BUTTON_EVENT::DoubleClicked);

		buttonFlags.clicks = 0; // reset clicks
	}
}
