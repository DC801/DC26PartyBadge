/*
 * @file utility.h
 *
 * @date Jul 24, 2017
 * @author hamster
 *
 * Utility functions
 *
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#define BUTTON_PRESSED 	0
#define BUTTON_RELEASED 1
#define BUTTON_DEBOUNCE_MS		15
#define BUTTON_LONG_PRESS_MS	200

typedef enum{
    LEVEL0,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4
} LEVEL;

typedef enum{
    POWERUP_0,
    POWERUP_1,
    POWERUP_2,
    POWERUP_3,
    POWERUP_4
} POWERUP;


#define POLYNOM		0x0801

uint16_t calcCRC(uint8_t *data, uint8_t len);
uint16_t crc16(uint16_t crcValue, uint8_t newByte);


uint8_t getButton(bool waitForLongPress);
bool isButtonDown(int button);
void pauseUntilPress(int button);
void beep(int duration, int frequency);
void getString(char *string, uint8_t chars, bool showScroll);
void setLevelLEDs(LEVEL level);
void setPowerUpLEDs(POWERUP powerUp);

APP_TIMER_DEF(sysTickID);
void sysTickStart(void);
void sysTickHandler(void * p_context);
uint32_t getSystick(void);

uint8_t getFiles(char files[][9], char *path, uint8_t fileMax);

#endif /* UTILITY_H_ */
