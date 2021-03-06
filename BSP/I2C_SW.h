#ifndef __I2C_SW_H
#define __I2C_SW_H

#include "user_common.h"

/*配置SDA,SCL端口和引脚*/
#define I2C_SW_GPIO_CLK          RCC_AHB1Periph_GPIOF

#define I2C_SW_GPIO_PORT         GPIOF
#define I2C_SW_SCL_Pin           GPIO_Pin_7
#define I2C_SW_SDA_Pin           GPIO_Pin_9


#define I2C_WR_ADDR		0		/* 写地址（原7位地址<<1|WR）*/
#define I2C_RD_ADDR		1		/* 写地址（原7位地址<<1|RD）*/

/*写SCL和SDA电平*/
#define SCL_H			GPIO_SetBits(I2C_SW_GPIO_PORT, I2C_SW_SCL_Pin)
#define SDA_H			GPIO_SetBits(I2C_SW_GPIO_PORT, I2C_SW_SDA_Pin)
#define SCL_L			GPIO_ResetBits(I2C_SW_GPIO_PORT, I2C_SW_SCL_Pin)
#define SDA_L			GPIO_ResetBits(I2C_SW_GPIO_PORT, I2C_SW_SDA_Pin)

/*读SDA电平*/
#define SDA_READ  GPIO_ReadInputDataBit(I2C_SW_GPIO_PORT, I2C_SW_SDA_Pin)


void I2C_SW_Init(void);

uint8_t I2C_CheckDevice(uint8_t D_Address);

uint8_t I2C_WriteBuff(uint8_t D_Address, uint8_t R_Address, uint16_t len, uint8_t *data_buf);
uint8_t I2C_ReadBuff(uint8_t D_Address, uint8_t R_Address, uint16_t len, uint8_t *data_buf);

// Write 8-bit to register
uint8_t I2C_writeRegister8(uint8_t D_Address, uint8_t R_Address, uint8_t data);
// Write 16-bit to register
uint8_t I2C_writeRegister16(uint8_t D_Address, uint8_t R_Address, int16_t value);
// Read 8-bit from register
uint8_t I2C_readRegister8(uint8_t D_Address, uint8_t R_Address);
// Read 16-bit from register
int16_t I2C_readRegister16(uint8_t D_Address, uint8_t R_Address);

// Read register bit
bool I2C_readRegisterBit(uint8_t D_Address, uint8_t R_Address, uint8_t pos);
// Write register bit
uint8_t I2C_writeRegisterBit(uint8_t D_Address, uint8_t R_Address, uint8_t pos, bool state);


/*===========================MPU DMP 接口================================*/
int Sensors_I2C_ReadRegister(unsigned char slave_addr,
                                       unsigned char reg_addr,
                                       unsigned short len,
                                       unsigned char *data_ptr);
int Sensors_I2C_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len,
                                        const unsigned char *data_ptr);

#endif /* __I2C_SW_H */
