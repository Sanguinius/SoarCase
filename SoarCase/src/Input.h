/*
 **************************************************************************
 * @file    buttoncontrol.h
 * @author  Ivo Infanger
 * @version V01_01
 * @date    03.01.2012
 * @brief   Button Configuration & Setup
 **************************************************************************
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <avr/io.h>
#include <string.h>
#include "BoardInit.h"
#include "nmea/nmea.h"
#include "USB.h"


#define FS1 		0
#define FS2 		1
#define FS3 		2
#define FS4 		3
#define FS5 		4
#define FS6 		5
#define FS7 		6
#define FS8 		7
#define FS7OCW		8
#define FS7OCCW		9
#define FS7ICW		10
#define FS7ICCW		11
#define FS7ICWAlt	12
#define FS7ICCWAlt	13
#define FS8OCW		14
#define FS8OCCW		15
#define FS8ICW		16
#define FS8ICCW		17
#define FS8ICWAlt	18
#define FS8ICCWAlt	19


void Input_Init ( void );
void Input_AssignButtons ( void );
void Input_CheckButton ( uint8_t PSx, uint8_t FSx, uint8_t edgePos );
void Input_CheckRotary ( uint8_t PSx, uint8_t PSxAO, uint8_t PSxBO, uint8_t PSxAI, uint8_t PSxBI, uint8_t FSx, uint8_t FSxOCW, uint8_t FSxOCCW, uint8_t FSxICW, uint8_t FSxICCW, uint8_t FSxICWAlt, uint8_t FSxICCWAlt );
void Input_ClearStateASCII ( void );
void Input_ClearStateHID ( void );
void Input_ReadInputs ( void );
uint8_t Input_IsButtonPressedASCII ( uint8_t button );
uint8_t Input_IsButtonPressedHID ( uint8_t button );
bool Input_HasHIDPriority ( uint8_t FSx );
void Input_SendNMEA ( nmeaINFO *info );
uint8_t Input_getHIDCode ( uint8_t FSx );
void Input_ClearDoubleclick ( void );
void Input_CheckDoubleclick ( void );

#endif /* INPUT_H_ */