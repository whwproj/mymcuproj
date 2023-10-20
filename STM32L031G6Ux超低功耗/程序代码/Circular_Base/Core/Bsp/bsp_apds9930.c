/**
 * @file    APDS-9930.cpp
 * @brief   Library for the SparkFun APDS-9930 breakout board
 * @author  Shawn Hymel (SparkFun Electronics)
 *
 * @copyright	This code is public domain but you buy me a beer if you use
 * this and we meet someday (Beerware license).
 *
 * This library interfaces the Avago APDS-9930 to Arduino over I2C. The library
 * relies on the Arduino Wire (I2C) library. to use the library, instantiate an
 * APDS9930 object, call init(), and call the appropriate functions.
 *
 * APDS-9930 current draw tests (default parameters):
 *   Off:                   1mA
 *   Waiting for gesture:   14mA
 *   Gesture in progress:   35mA
 */
 
#include "../Bsp/bsp_apds9930.h"
 

/*******************************************************************************
 * Public methods for controlling the APDS-9930
 ******************************************************************************/

/**
 * @brief Reads and returns the contents of the ENABLE register
 *
 * @return Contents of the ENABLE register. 0xFF if error.
 */
uint8_t getMode( void ) {
    uint8_t enable_value;
    
    /* Read current ENABLE register */
    if( !wireReadDataByte(APDS9930_ENABLE, &enable_value) ) {
        return 0xFF;
    }
    
    return enable_value;
}

/**
 * @brief Enables or disables a feature in the APDS-9930
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 * @return True if operation success. False otherwise.
 */
uint8_t setMode(uint8_t mode, uint8_t enable) {
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if( reg_val == 0xFF ) {
        return 0;
    }
    
    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( mode <= 6 ) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }
        
    /* Write value back to ENABLE register */
    if( !wireWriteDataByte(APDS9930_ENABLE, reg_val) ) {
        return 0;
    }
        
    return 1;
}

/**
 * @brief Starts the light (Ambient/IR) sensor on the APDS-9930
 *
 * @param[in] interrupts true to enable hardware interrupt on high or low light
 * @return True if sensor enabled correctly. False on error.
 */
uint8_t enableLightSensor(uint8_t interrupts) {
    
    /* Set default gain, interrupts, enable power, and enable sensor */
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return 0;
    }
    if( interrupts ) {
        if( !setAmbientLightIntEnable(1) ) {
            return 0;
        }
    } else {
        if( !setAmbientLightIntEnable(0) ) {
            return 0;
        }
    }
    if( !enablePower() ){
        return 0;
    }
    if( !setMode(AMBIENT_LIGHT, 1) ) {
        return 0;
    }
    
    return 1;

}

/**
 * @brief Ends the light sensor on the APDS-9930
 *
 * @return True if sensor disabled correctly. False on error.
 */
uint8_t disableLightSensor( void ) {
    if( !setAmbientLightIntEnable(0) ) {
        return 0;
    }
    if( !setMode(AMBIENT_LIGHT, 0) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Starts the proximity sensor on the APDS-9930
 *
 * @param[in] interrupts true to enable hardware external interrupt on proximity
 * @return True if sensor enabled correctly. False on error.
 */
uint8_t enableProximitySensor(uint8_t interrupts) {
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return 0;
    }
    if( !setLEDDrive(DEFAULT_PDRIVE) ) {
        return 0;
    }
    if( interrupts ) {
        if( !setProximityIntEnable(1) ) {
            return 0;
        }
    } else {
        if( !setProximityIntEnable(0) ) {
            return 0;
        }
    }
    if( !enablePower() ){
        return 0;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Ends the proximity sensor on the APDS-9930
 *
 * @return True if sensor disabled correctly. False on error.
 */
uint8_t disableProximitySensor( void ) {
	if( !setProximityIntEnable(0) ) {
		return 0;
	}
	if( !setMode(PROXIMITY, 0) ) {
		return 0;
	}

	return 1;
}

/**
 * Turn the APDS-9930 on
 *
 * @return True if operation successful. False otherwise.
 */
uint8_t enablePower( void ) {
    if( !setMode(POWER, 1) ) {
        return 0;
    }
    
    return 1;
}

/**
 * Turn the APDS-9930 off
 *
 * @return True if operation successful. False otherwise.
 */
uint8_t disablePower( void ) {
    if( !setMode(POWER, 0) ) {
        return 0;
    }
    
    return 1;
}

/*******************************************************************************
 * Ambient light sensor controls
 ******************************************************************************/

/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
uint8_t readAmbientLightLux(float *val) {
    uint16_t Ch0;
    uint16_t Ch1;
    
    /* Read value from channel 0 */
    if( !readCh0Light(&Ch0) ) {
        return 0;
    }

    /* Read value from channel 1 */
    if( !readCh1Light(&Ch1) ) {
        return 0;
    }

    *val = floatAmbientToLux(Ch0, Ch1);
		return 0;
}

uint8_t ureadAmbientLightLux(unsigned long *val) {
    uint16_t Ch0;
    uint16_t Ch1;
    
    /* Read value from channel 0 */
    if( !readCh0Light(&Ch0) ) {
        return 0;
    }

    /* Read value from channel 1 */
    if( !readCh1Light(&Ch1) ) {
        return 0;
    }

    *val = ulongAmbientToLux(Ch0, Ch1);
		return 0;
}

float floatAmbientToLux(uint16_t Ch0, uint16_t Ch1) {
    float ALSIT = 2.73 * (256 - DEFAULT_ATIME);
    float iac  = (Ch0-B*Ch1) > (C*Ch0-D*Ch1) ? (Ch0-B*Ch1) : (C*Ch0-D*Ch1);//max(Ch0 - B * Ch1, C * Ch0 - D * Ch1); 
    float lpc  = GA * DF / (ALSIT * getAmbientLightGain());
    return iac * lpc;
}

unsigned long ulongAmbientToLux(uint16_t Ch0, uint16_t Ch1) {
    unsigned long ALSIT = 2.73 * (256 - DEFAULT_ATIME);
    unsigned long iac  = (Ch0-B*Ch1) > (C*Ch0-D*Ch1) ? (Ch0-B*Ch1) : (C*Ch0-D*Ch1);//max(Ch0 - B * Ch1, C * Ch0 - D * Ch1);
    unsigned long lpc  = GA * DF / (ALSIT * getAmbientLightGain());
    return iac * lpc;
}

uint8_t readCh0Light(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from channel 0 */
    if( !wireReadDataByte(APDS9930_Ch0DATAL, &val_byte) ) {
        return 0;
    }
    *val = val_byte;
    if( !wireReadDataByte(APDS9930_Ch0DATAH, &val_byte) ) {
        return 0;
    }
    *val += ((uint16_t)val_byte << 8);
    return 1;
}

uint8_t readCh1Light(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from channel 0 */
    if( !wireReadDataByte(APDS9930_Ch1DATAL, &val_byte) ) {
        return 0;
    }
    *val = val_byte;
    if( !wireReadDataByte(APDS9930_Ch1DATAH, &val_byte) ) {
        return 0;
    }
    *val += ((uint16_t)val_byte << 8);
    return 1;
}

/*******************************************************************************
 * Proximity sensor controls
 ******************************************************************************/

/**
 * @brief Reads the proximity level as an 8-bit value
 *
 * @param[out] val value of the proximity sensor.
 * @return True if operation successful. False otherwise.
 */
uint8_t readProximity(uint16_t *val) {
    *val = 0;
    uint8_t val_byte;

    /* Read value from proximity data register */
    if( !wireReadDataByte(APDS9930_PDATAL, &val_byte) ) {
        return 0;
    }
    *val = val_byte;
    if( !wireReadDataByte(APDS9930_PDATAH, &val_byte) ) {
        return 0;
    }
    *val += ((uint16_t)val_byte << 8);
    
    return 1;
}

/*******************************************************************************
 * Getters and setters for register values
 ******************************************************************************/

/**
 * @brief Returns the lower threshold for proximity detection
 *
 * @return lower threshold
 */
uint8_t getProximityIntLowThreshold( void ) {
    uint16_t val;
    uint8_t val_byte;
    
    /* Read value from PILT register */
    if( !wireReadDataByte(APDS9930_PILTL, &val_byte) ) {
        val = 0;
    }
    val = val_byte;
    if( !wireReadDataByte(APDS9930_PILTH, &val_byte) ) {
        val = 0;
    }
    val += ((uint16_t)val_byte << 8);    
    
    return val;
}

/**
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 * @return True if operation successful. False otherwise.
 */
uint8_t setProximityIntLowThreshold(uint16_t threshold) {
    uint8_t lo;
    uint8_t hi;
    hi = threshold >> 8;
    lo = threshold & 0x00FF;

    if( !wireWriteDataByte(APDS9930_PILTL, lo) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_PILTH, hi) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Returns the high threshold for proximity detection
 *
 * @return high threshold
 */
uint8_t getProximityIntHighThreshold( void ) {
    uint16_t val;
    uint8_t val_byte;
    
    /* Read value from PILT register */
    if( !wireReadDataByte(APDS9930_PIHTL, &val_byte) ) {
        val = 0;
    }
    val = val_byte;
    if( !wireReadDataByte(APDS9930_PIHTH, &val_byte) ) {
        val = 0;
    }
    val += ((uint16_t)val_byte << 8);    
    
    return val;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 * @return True if operation successful. False otherwise.
 */
uint8_t setProximityIntHighThreshold(uint16_t threshold) {
    uint8_t lo;
    uint8_t hi;
    hi = threshold >> 8;
    lo = threshold & 0x00FF;

    if( !wireWriteDataByte(APDS9930_PIHTL, lo) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_PIHTH, hi) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Returns LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @return the value of the LED drive strength. 0xFF on failure.
 */
uint8_t getLEDDrive( void ) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out LED drive bits */
    val = (val >> 6) & 0x03;
    
    return val;
}

/**
 * @brief Sets the LED drive strength for proximity and ALS
 *
 * Value    LED Current
 *   0        100 mA
 *   1         50 mA
 *   2         25 mA
 *   3         12.5 mA
 *
 * @param[in] drive the value (0-3) for the LED drive strength
 * @return True if operation successful. False otherwise.
 */
uint8_t setLEDDrive(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 6;
    val &= 0x3F;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9930_CONTROL, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Returns receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @return the value of the proximity gain. 0xFF on failure.
 */
uint8_t getProximityGain( void ) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out PDRIVE bits */
    val = (val >> 2) & 0x03;
    
    return val;
}

/**
 * @brief Sets the receiver gain for proximity detection
 *
 * Value    Gain
 *   0       1x
 *   1       2x
 *   2       4x
 *   3       8x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
uint8_t setProximityGain(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 2;
    val &= 0xF3;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9930_CONTROL, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Returns the proximity diode
 *
 * Value    Diode selection
 *   0       Reserved
 *   1       Reserved
 *   2       Use Ch1 diode
 *   3       Reserved
 *
 * @return the selected diode. 0xFF on failure.
 */
uint8_t getProximityDiode( void ) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out PDRIVE bits */
    val = (val >> 4) & 0x03;
    
    return val;
}

/**
 * @brief Selects the proximity diode
 *
 * Value    Diode selection
 *   0       Reserved
 *   1       Reserved
 *   2       Use Ch1 diode
 *   3       Reserved
 *
 * @param[in] drive the value (0-3) for the diode
 * @return True if operation successful. False otherwise.
 */
uint8_t setProximityDiode(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 4;
    val &= 0xCF;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9930_CONTROL, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Returns receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @return the value of the ALS gain. 0xFF on failure.
 */
uint8_t getAmbientLightGain( void ) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out ADRIVE bits */
    val &= 0x03;
    
    return val;
}

/**
 * @brief Sets the receiver gain for the ambient light sensor (ALS)
 *
 * Value    Gain
 *   0        1x
 *   1        4x
 *   2       16x
 *   3       64x
 *
 * @param[in] drive the value (0-3) for the gain
 * @return True if operation successful. False otherwise.
 */
uint8_t setAmbientLightGain(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !wireReadDataByte(APDS9930_CONTROL, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    val &= 0xFC;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !wireWriteDataByte(APDS9930_CONTROL, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Gets the low threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9930
 * @return True if operation successful. False otherwise.
 */
uint8_t getLightIntLowThreshold(uint16_t *threshold) {
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light low threshold, low byte register */
    if( !wireReadDataByte(APDS9930_AILTL, &val_byte) ) {
        return 0;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light low threshold, high byte register */
    if( !wireReadDataByte(APDS9930_AILTH, &val_byte) ) {
        return 0;
    }
    *threshold = *threshold + ((uint16_t)val_byte << 8);
    
    return 1;
}

/**
 * @brief Sets the low threshold for ambient light interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
uint8_t setLightIntLowThreshold(uint16_t threshold) {
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( !wireWriteDataByte(APDS9930_AILTL, val_low) ) {
        return 0;
    }
    
    /* Write high byte */
    if( !wireWriteDataByte(APDS9930_AILTH, val_high) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Gets the high threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9930
 * @return True if operation successful. False otherwise.
 */
uint8_t getLightIntHighThreshold(uint16_t *threshold) {
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light high threshold, low byte register */
    if( !wireReadDataByte(APDS9930_AIHTL, &val_byte) ) {
        return 0;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light high threshold, high byte register */
    if( !wireReadDataByte(APDS9930_AIHTH, &val_byte) ) {
        return 0;
    }
    *threshold = *threshold + ((uint16_t)val_byte << 8);
    
    return 1;
}

/**
 * @brief Sets the high threshold for ambient light interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return True if operation successful. False otherwise.
 */
uint8_t setLightIntHighThreshold(uint16_t threshold) {
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( !wireWriteDataByte(APDS9930_AIHTL, val_low) ) {
        return 0;
    }
    
    /* Write high byte */
    if( !wireWriteDataByte(APDS9930_AIHTH, val_high) ) {
        return 0;
    }
    
    return 1;
}


/**
 * @brief Gets if ambient light interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getAmbientLightIntEnable( void ) {
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9930_ENABLE, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out AIEN bit */
    val = (val >> 4) & 0x01;
    
    return val;
}

/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
uint8_t setAmbientLightIntEnable(uint8_t enable) {
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9930_ENABLE, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    enable &= 0x01;
    enable = enable << 4;
    val &= 0xEF;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9930_ENABLE, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Gets if proximity interrupts are enabled or not
 *
 * @return 1 if interrupts are enabled, 0 if not. 0xFF on error.
 */
uint8_t getProximityIntEnable( void ) {
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9930_ENABLE, &val) ) {
        return 0xFF;
    }
    
    /* Shift and mask out PIEN bit */
    val = (val >> 5) & 0x01;
    
    return val;
}

/**
 * @brief Turns proximity interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return True if operation successful. False otherwise.
 */
uint8_t setProximityIntEnable(uint8_t enable) {
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !wireReadDataByte(APDS9930_ENABLE, &val) ) {
        return 0;
    }
    
    /* Set bits in register to given value */
    enable &= 0x01;
    enable = enable << 5;
    val &= 0xDF;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( !wireWriteDataByte(APDS9930_ENABLE, val) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Clears the ambient light interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
uint8_t clearAmbientLightInt( void ) {
    if( !wireWriteByte(CLEAR_ALS_INT) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Clears the proximity interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
uint8_t clearProximityInt( void ) {
    if( !wireWriteByte(CLEAR_PROX_INT) ) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Clears all interrupts
 *
 * @return True if operation completed successfully. False otherwise.
 */
uint8_t clearAllInts( void ) {
    if( !wireWriteByte(CLEAR_ALL_INTS) ) {
        return 0;
    }
    
    return 1;
}

/*******************************************************************************
 * Raw I2C Reads and Writes
 ******************************************************************************/

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
uint8_t wireWriteByte(uint8_t val) {
	if ( HAL_I2C_Master_Transmit(&hi2c1, APDS9930_I2C_ADDR, &val, 1, 0x100) != HAL_OK ) {
		return 0;
	}
	return 1;
	
}

/**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
uint8_t wireWriteDataByte(uint8_t reg, uint8_t val) {
    if ( HAL_I2C_Mem_Write(&hi2c1, APDS9930_I2C_ADDR, reg|AUTO_INCREMENT, 1, &val, 1, 0x100) != HAL_OK ) {
		return 0;
	}
	return 1;
}

/**
 * @brief Writes a block (array) of bytes to the I2C device and register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val pointer to the beginning of the data byte array
 * @param[in] len the length (in bytes) of the data to write
 * @return True if successful write operation. False otherwise.
 */
uint8_t wireWriteDataBlock(  uint8_t reg, uint8_t *val, unsigned int len) {
	if ( HAL_I2C_Mem_Write(&hi2c1, APDS9930_I2C_ADDR, reg|AUTO_INCREMENT, 1, val, len, 0x100) != HAL_OK ) {
		return 0;
	}
	return 1;
}

/**
 * @brief Reads a single byte from the I2C device and specified register
 *
 * @param[in] reg the register to read from
 * @param[out] the value returned from the register
 * @return True if successful read operation. False otherwise.
 */
uint8_t wireReadDataByte(uint8_t reg, uint8_t *val) {
   if ( HAL_I2C_Mem_Read(&hi2c1, APDS9930_I2C_ADDR, reg|AUTO_INCREMENT, 1, val, 1, 0x100) != HAL_OK ) {
		return 0;
	}
	return 1;
}

/**
 * @brief Reads a block (array) of bytes from the I2C device and register
 *
 * @param[in] reg the register to read from
 * @param[out] val pointer to the beginning of the data
 * @param[in] len number of bytes to read
 * @return Number of bytes read. -1 on read error.
 */
int wireReadDataBlock( uint8_t reg, uint8_t *val, unsigned int len) {
	if ( HAL_I2C_Mem_Read(&hi2c1, APDS9930_I2C_ADDR, reg|AUTO_INCREMENT, 1, val, len, 0x100) != HAL_OK ) {
		return -1;
	}
	return len;
}


/**
 * @brief Configures I2C communications and initializes registers to defaults
 *
 * @return True if initialized successfully. False otherwise.
 */
uint8_t APDS9930_init( void ) {
    uint8_t id;
		uint16_t threshold = 0;
     
    /* Read ID register and check against known values for APDS-9930 */
    if( !wireReadDataByte(APDS9930_ID, &id) ) {
        //printf("ID read err!\r\n");
        return 0;
    }
    if( !(id == APDS9930_ID_1 || id == APDS9930_ID_2) ) {
        //printf("ID check err\r\n");
        return 0;
    } else {
		//printf("ID check is %d\r\n", id);
	}
     
    /* Set ENABLE register to 0 (disable all features) */
    if( !setMode(ALL, OFF) ) {
        //printf("Regs off\r\n");
        return 0;
    }
    
    /* Set default values for ambient light and proximity registers */
    if( !wireWriteDataByte(APDS9930_ATIME, DEFAULT_ATIME) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_WTIME, DEFAULT_WTIME) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_PPULSE, DEFAULT_PPULSE) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_POFFSET, DEFAULT_POFFSET) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_CONFIG, DEFAULT_CONFIG) ) {
        return 0;
    }
    if( !setLEDDrive(DEFAULT_PDRIVE) ) {
        return 0;
    }
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return 0;
    }
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return 0;
    }
    if( !setProximityDiode(DEFAULT_PDIODE) ) {
        return 0;
    }
    if( !setProximityIntLowThreshold(DEFAULT_PILT) ) {
        return 0;
    }
    if( !setProximityIntHighThreshold(DEFAULT_PIHT) ) {
        return 0;
    }
    if( !setLightIntLowThreshold(DEFAULT_AILT) ) {
        return 0;
    }
    if( !setLightIntHighThreshold(DEFAULT_AIHT) ) {
        return 0;
    }
    if( !wireWriteDataByte(APDS9930_PERS, DEFAULT_PERS) ) {
        return 0;
    }
		
		// Set high and low interrupt thresholds
		if ( !setLightIntLowThreshold(LIGHT_INT_LOW) ) {
		 //printf("Error writing low threshold\r\n");
			return 0;
		}
		if ( !setLightIntHighThreshold(LIGHT_INT_HIGH) ) {
			//printf("Error writing high threshold\r\n");
			return 0;
		}
		
		// Start running the APDS-9930 light sensor (no interrupts)
		if ( !enableLightSensor(0) ) {
			//printf("Light sensor is now running\r\n");
			return 0;
		} else {
			//printf("Something went wrong during light sensor init!\r\n");
		}
	 
		// Read high and low interrupt thresholds
		if ( !getLightIntLowThreshold(&threshold) ) {
			//printf("Error reading low threshold\r\n");
			return 0;
		} else {
			//printf("Low Threshold: %d\r\n", threshold);
		}
		if ( !getLightIntHighThreshold(&threshold) ) {
			//printf("Error reading high threshold\r\n");
			return 0;
		} else {
			//printf("High Threshold: %d\r\n", threshold);
		}
		
		// Enable interrupts
		if ( !setAmbientLightIntEnable(1) ) {
			//printf("Error enabling interrupts\r\n");
			return 0;
		}
	
    return 1;
}


void APDS9930_interrupt( void ) {
	float ambient_light = 0;
	uint16_t ch0 = 0;
	uint16_t ch1 = 1;
	
// Read the light levels (ambient, red, green, blue) and print
	if ( !readAmbientLightLux(&ambient_light) ||
			 !readCh0Light(&ch0) || 
			 !readCh1Light(&ch1) ) {
		//printf("Error reading light values\r\n");
	} else {
		//printf("Interrupt! Ambient:%f  ", ambient_light);
		//printf("Ch0:%d  ", ch0 );
		//printf("Ch1:%d\r\n", ch1 );
	}
	
	// Turn on LED for a half a second
	//LED_ON();
	osDelay(500);
	//LED_OFF();
	
	// Reset flag and clear APDS-9930 interrupt (IMPORTANT!)
	if ( !clearAmbientLightInt() ) {
		//printf("Error clearing interrupt\r\n");
	}
	//HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

