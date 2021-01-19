/*
 * lcd1602.h based on lcd control from Controllerstech https://controllerstech.com/interface-lcd-16x2-with-stm32-without-i2c/
 *
 *
 *  Created on: Jan 09, 2021
 *      Authors: Krzysztof Nowicki, Szymon Kwasiborski
 */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_


void lcd_init (void);   // Function initializing lcd

void lcd_send_cmd (char cmd);  // Funtion that sends commands in hexadecimal numbers

void lcd_send_data (char data);  // Funtion sending data to lcd display

void lcd_send_string (char *str);  // Funtion that allows to send char arrays or strings

void lcd_put_cur(int row, int col);  // Function that translates simple cursor location in rows and columns to one readeable by lcd display

void lcd_clear (void); //Funtion that clear the screen from any characters

#endif /* INC_LCD1602_H_ */
