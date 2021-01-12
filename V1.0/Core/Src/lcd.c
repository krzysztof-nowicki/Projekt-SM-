/**
  ******************************************************************************
  * @file    lcd.c
  * @author  Olivier Van den Eede [ https://github.com/4ilo/HD44780-Stm32HAL ]
  * @author  AW (adjustments for STM32F7)
  * @version V2.0
  * @date    10-Jun-2018
  * @brief   Simple HD44780 driver library for STM32F7.
  *          NOTE!: This code provides only WRITE features, no READ features.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* Typedef -------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/
#define LCD_NUMBER_BUF_SIZE 11
#define LCD_PRINTF_BUF_SIZE 64

/* Macro ---------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const uint8_t LCD_ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t LCD_ROW_20[] = {0x00, 0x40, 0x14, 0x54};

/* Public variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void lcd_write_command(LCD_HandleTypeDef* hlcd, uint8_t command);
void lcd_write_data(LCD_HandleTypeDef* hlcd, uint8_t data);
void lcd_write(LCD_HandleTypeDef* hlcd, uint8_t data, uint8_t len);

/* Private function ----------------------------------------------------------*/
/**
 * @brief Write a byte to the command register
 * @param[in] hlcd LCD handler
 * @param[in] data Display command @see lcd.h/Define
 * @return None
 */
void lcd_write_command(LCD_HandleTypeDef* hlcd, uint8_t command)
{
  HAL_GPIO_WritePin(hlcd->RS_Port, hlcd->RS_Pin, LCD_COMMAND_REG);    // Write to command register

  if(hlcd->Mode == LCD_4_BIT_MODE)
  {
    lcd_write(hlcd, (command >> 4), LCD_NIB);
    lcd_write(hlcd, command & 0x0F, LCD_NIB);
  }
  else
  {
     lcd_write(hlcd, command, LCD_BYTE);
  }
}

/**
 * @brief Write a byte to the data register
 * @param[in] hlcd LCD handler
 * @param[in] data Display data byte
 * @return None
 */
void lcd_write_data(LCD_HandleTypeDef* hlcd, uint8_t data)
{
  HAL_GPIO_WritePin(hlcd->RS_Port, hlcd->RS_Pin, LCD_DATA_REG);     // Write to data register

  if(hlcd->Mode == LCD_4_BIT_MODE)
  {
    lcd_write(hlcd, data >> 4, LCD_NIB);
    lcd_write(hlcd, data & 0x0F, LCD_NIB);
  }
  else
  {
    lcd_write(hlcd, data, LCD_BYTE);
  }
}

/**
 * @brief Set len bits on the bus and toggle the enable line
 * @param[in] hlcd LCD handler
 * @param[in] data Data byte
 * @param[in] len  Data port size (length): 4 ( LCD_NIB )or 8 ( LCD_BYTE ) bits
 * @return None
 */
void lcd_write(LCD_HandleTypeDef* hlcd, uint8_t data, uint8_t len)
{
  for(uint8_t i = 0; i < len; i++)
  {
    HAL_GPIO_WritePin(hlcd->Data_Port[i], hlcd->Data_Pin[i], (data >> i) & 0x01);
  }

  HAL_GPIO_WritePin(hlcd->EN_Port, hlcd->EN_Pin, GPIO_PIN_SET);
  __LCD_Delay(1);
  HAL_GPIO_WritePin(hlcd->EN_Port, hlcd->EN_Pin, GPIO_PIN_RESET); // Data receive on falling edge
}

/* Public function -----------------------------------------------------------*/

/**
 * @brief LCD initialization procedure.
 * @note Cursor off, Cursor increment on, No blink @see HD44780 technical note.
 * @param[in] hlcd LCD handler
 * @return None 
 */
void LCD_Init(LCD_HandleTypeDef* hlcd)
{
  if(hlcd->Mode == LCD_4_BIT_MODE)
  {
    lcd_write_command(hlcd, 0x33); // 0011 0011
    lcd_write_command(hlcd, 0x32); // 0011 0010
    lcd_write_command(hlcd, LCD_FUNCTION_SET | LCD_OPT_N);       // 4-bit mode
  }
  else
    lcd_write_command(hlcd, LCD_FUNCTION_SET | LCD_OPT_DL | LCD_OPT_N);

  lcd_write_command(hlcd, LCD_CLEAR_DISPLAY);                    // Clear screen
  lcd_write_command(hlcd, LCD_DISPLAY_ON_OFF_CONTROL | LCD_OPT_D);   // LCD on, Cursor off, No blink
  lcd_write_command(hlcd, LCD_ENTRY_MODE_SET | LCD_OPT_INC);         // Cursor increment on
}

/**
 * @brief Write a decimal number on the current position.
 * @param[in] hlcd   LCD handler
 * @param[in] number Decimal number, max. 10 digits
 * @return None 
 */
void LCD_printInt(LCD_HandleTypeDef* hlcd, int number)
{
  char buffer[LCD_NUMBER_BUF_SIZE];
  sprintf(buffer, "%d", number);

  LCD_printStr(hlcd, buffer);
}

/**
 * @brief Write a hexadecimal number on the current position.
 * @param[in] hlcd   LCD handler
 * @param[in] number Hexadecimal number, max. 10 digits
 * @return None 
 */
void LCD_printHex(LCD_HandleTypeDef* hlcd, int number)
{
  char buffer[LCD_NUMBER_BUF_SIZE];
  sprintf(buffer, "%x", number);

  LCD_printStr(hlcd, buffer);
}

/**
 * @brief Write a string on the current position.
 * @param[in] hlcd LCD handler
 * @param[in] str  Null-terminated string
 * @return None 
 */
void LCD_printStr(LCD_HandleTypeDef* hlcd, char* str)
{
  for(uint8_t i = 0; i < strlen(str); i++)
    lcd_write_data(hlcd, str[i]);
}

/**
 * @brief Set the cursor position.
 * @param[in] hlcd LCD handler
 * @param[in] row  Display row (line): 0 to N
 * @param[in] col  Display column: 0 to 15 (16 character display) or 19 (20 character display)
 * @return None 
 */
void LCD_SetCursor(LCD_HandleTypeDef* hlcd, uint8_t row, uint8_t col)
{
  #ifdef LCD20xN
  lcd_write_command(hlcd, LCD_SET_DDRAM_ADDR + LCD_ROW_20[row] + col);
  #endif

  #ifdef LCD16xN
  lcd_write_command(hlcd, LCD_SET_DDRAM_ADDR + LCD_ROW_16[row] + col);
  #endif
}

/**
 * @brief Clear the screen.
 * @param[in] hlcd LCD handler
 * @return None 
 */
void LCD_Clear(LCD_HandleTypeDef* hlcd) {
  lcd_write_command(hlcd, LCD_CLEAR_DISPLAY);
}

/**
 * @brief Clear the screen.
 * @param[in] hlcd   LCD handler
 * @param[in] code   Defined character code in display memory @see HD44780 technical note.
 * @param[in] bitmap Defined character array @see HD44780 technical note.
 * @return None 
 */
void LCD_DefineChar(LCD_HandleTypeDef* lcd, uint8_t code, uint8_t bitmap[]){
  lcd_write_command(lcd, LCD_SETCGRAM_ADDR + (code << 3));
  
  for(uint8_t i=0; i < 8; ++i)
    lcd_write_data(lcd, bitmap[i]);
}

#ifdef LCD_PRINTF_ENABLE
/**
 * @brief Write text in standard format on the current position.
 * @param[in] hlcd   LCD handler
 * @param[in] format Text format @see http://www.cplusplus.com/reference/cstdio/printf/
 * @param[in] ...    Variadic arguments
 * @return None
 */
void LCD_printf(LCD_HandleTypeDef * lcd, const char* format, ...)
{
  char buffer[LCD_PRINTF_BUF_SIZE];
  va_list args;
  va_start(args, format);
  vsprintf(buffer,format, args);
  LCD_printStr(lcd, buffer);
  va_end(args);
}
#endif
