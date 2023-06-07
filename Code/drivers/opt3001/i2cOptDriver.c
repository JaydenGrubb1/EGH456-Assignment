/**************************************************************************************************
*  Filename:       i2cOptDriver.c
*  By:             Jesse Haviland
*  Created:        1 February 2019
*  Revised:        23 March 2019
*  Revision:       2.0
*
*  Description:    i2c Driver for use with opt3001.c and the TI OP3001 Optical Sensor
*************************************************************************************************/



// ----------------------- Includes -----------------------
#include "i2cOptDriver.h"
#include "utils/uartstdio.h"

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/drivers/i2c.h>

#include "Board.h"

static I2C_Handle g_handle = 0;
static uint32_t g_device = ~0;

bool openI2C(uint32_t device)
{
    if (g_device == device)
        return true;

    if (g_handle != 0)
    {
        I2C_close(g_handle);
        g_handle = 0;
        g_device = ~0;
    }

    I2C_Handle i2c;
    I2C_Params i2cParams;

    /* Create and Open I2C port*/
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2c = I2C_open(Board_I2C_OPT3001, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");
        return false;
    }

    g_handle = i2c;
    g_device = device;

    System_printf("I2C Initialized!\n");
    return true;
}

bool doTransaction(uint8_t ui8Addr, uint8_t *pRX, int rxSize, uint8_t *pTX, int txSize)
{
    I2C_Transaction i2cTransaction = {};
    i2cTransaction.slaveAddress = ui8Addr;
    i2cTransaction.writeBuf     = pTX;
    i2cTransaction.writeCount   = txSize;
    i2cTransaction.readBuf      = pRX;
    i2cTransaction.readCount    = rxSize;
    return I2C_transfer(g_handle, &i2cTransaction);
}

/*
 * Sets slave address to ui8Addr
 * Puts ui8Reg followed by two data bytes in *data and transfers
 * over i2c
 */
bool writeI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{
    // Concat data to single buffer
    uint8_t txBuffer[3];
    memcpy(txBuffer, &ui8Reg, sizeof(ui8Reg));
    memcpy(txBuffer + sizeof(ui8Reg), data, 2);

    return doTransaction(ui8Addr, 0, 0, txBuffer, sizeof(txBuffer));
}

/*
 * Sets slave address to ui8Addr
 * Writes ui8Reg over i2c to specify register being read from
 * Reads three bytes from i2c slave. The third is redundant but
 * helps to flush the i2c register
 * Stores first two received bytes into *data
 */
bool readI2C(uint8_t ui8Addr, uint8_t ui8Reg, uint8_t *data)
{
    if (!doTransaction(ui8Addr, 0, 0, &ui8Reg, sizeof(ui8Reg)))
        return false;

    uint8_t readBuffer[3] = {};

    if (!doTransaction(ui8Addr, readBuffer, 3, 0, 0))
        return false;

    memcpy(data, readBuffer, 2);

    return true;
}


