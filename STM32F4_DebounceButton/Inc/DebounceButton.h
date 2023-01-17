#ifndef _DEBOUNCEBUTTON_H_
#define _DEBOUNCEBUTTON_H_

#include <stdint.h>

#include "stm32f4xx_hal.h"

class DebounceButton;

/**
* @param Released - button pulse Pressed->Released
* @param Pressed - button pulse Released->Pressed
* @param Clicked - button Pressed+Released(or hold)
* @param DoubleClicked - button Pressed+Released(or hold) twice
* @param RepeatClicked - button Pressed+Released continiously
* @param LongPressed - button hold
*/
enum class BUTTON_EVENT { Released, Pressed, Clicked, DoubleClicked, RepeatClicked, LongPressed };

/**
* @param VCC - button connected to VCC, mcu pin connected to GND, pinMode - INPUT
* @param GND - button connected to GND, mcu pin connected to VCC, pinMode - INPUT_PULLUP
*/
enum class BUTTON_CONNECTED { GND = 0, VCC = 1 };

/**
* @param OPEN - button normally open
* @param CLOSE - button normally closed
*/
enum class BUTTON_NORMAL { CLOSE = 0, OPEN = 1 };

/**
* The event handler procedure.
*
* @param button - pointer to the Button that generated the event
* @param eventType - the event type which trigger the call
*/
typedef void (*BtnEventHandler)(const DebounceButton* button, BUTTON_EVENT eventType);

class DebounceButton
{
public:

    // Default value of debounce delay
    static const uint16_t delayDebounce = 50;

    // Default value of double click delay
    static const uint16_t delayRepeatedClick = 600;

    // Default value of long press delay
    static const uint16_t delayLongPress = 1000;

    /**
      * @brief  Create debounce button connected specified input port pin to.
      * @param  GPIOx: where x can be (A..G depending on device used) to select the GPIO peripheral
      * @param  GPIO_Pin: specifies the port bit to read.
      *         This parameter can be GPIO_PIN_x where x can be (0..15).
      * @param  c: specifies the connection type.
      * 		This parameter can be VCC or GND.
      * @param  n: specifies the normal button state.
      * 		This parameter can be OPEN or CLOSE.
      * @retval None.
      */
    DebounceButton(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BUTTON_CONNECTED c = BUTTON_CONNECTED::VCC, BUTTON_NORMAL n = BUTTON_NORMAL::OPEN);

    ~DebounceButton();

    /**
    * Initialize the button's port pin.
    *
    * @return None.
    */
     void initPin();

    /**
    * Get the button's port.
    *
    * @return the GPIO peripheral
    */
    GPIO_TypeDef* getGPIOx() const;

    /**
    * Get the button's pin number.
    * 
    * @return specifies the port bit to read
    */
    uint16_t getPin() const;

    /**
    * Check current state of button and apply debouncing algorithm.
    * This method should be called from the code about every 50 ms.
    * If function is called less often, the debouncing algorithm may not work correctly,
    * which may cause other event detection algorithms to fail.
    *
    * @return true if button pressed (debounced)
    */
    bool check();

    /**
    * Check current state of button. May be not debounced.
    *
    * @return true if button pressed
    */
    bool getCurrentState();

    /**
    * Install the BtnEventHandler function pointer
    * 
    * @param eventHandler - the event handler function 
    */
    void setEventHandler(BtnEventHandler eventHandler);

    /**
    * Remove the BtnEventHandler function pointer
    * 
    */
    void clearEventHandler();

protected:

    /**
     * Check current state of button's PIN.
     *
     * @return PIN state (not debounced)
    */
    bool getPinState();

    /**
     * Get current processor time.
     *
     * @return timestamp
    */
    uint32_t getTicks();

    // GPIO peripheral
    GPIO_TypeDef* const gpioX;

    // The port bit to read
    const uint16_t pin;

    // Button level connected to
    const BUTTON_CONNECTED connectedTo;

    // Button normal state
    const BUTTON_NORMAL normalState;

    // The event handler for button
    BtnEventHandler eventHandler = nullptr;

    uint32_t lastDebounceTime = 0;
    uint32_t lastPressedTime = 0;
    uint32_t lastReleasedTime = 0;

    // Internal flags
    struct ButtonFlags
    {
        bool debounced      : 1;
        bool currrentState  : 1;
        bool previousState  : 1;
        bool debouncedState : 1;
        uint8_t clicks      : 4;
    }
    buttonFlags = { true, false, false, false, 0 };

    bool debounce(uint32_t now);

    void processPressed(uint32_t now);
    void processReleased(uint32_t now);

private:

    // Disable copy-constructor and assignment operator
    DebounceButton(const DebounceButton&) = delete;
    DebounceButton& operator=(const DebounceButton&) = delete;
};

#endif
