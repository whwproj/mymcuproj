#include "../Bsp/bsp_apds9930.h"

int wireReadDataByte( uint8_t data, uint8_t *val ) {
	if ( HAL_I2C_Mem_Read(&hi2c1, APDS9930_I2C_ADDR, data|AUTO_INCREMENT, 1, val, 1, 0x100) != HAL_OK ) {
		return -1;
	}
	//HAL_I2C_Mem_Write(&hi2c1,	OLED_ADDRESS,0x00,I2C_MEMADD_SIZE_8BIT,pData,1,100)
	/*HAL_I2C_Master_Transmit( &hi2c1, APDS9930_I2C_ADDR, &data, 1, 0xFFF );
	HAL_I2C_Master_Receive( &hi2c1, APDS9930_I2C_ADDR, &r_data, 1, 0xFFF );*/
	return 0;
}

int wireWriteDataByte(uint8_t reg, uint8_t val) {
	if ( HAL_I2C_Mem_Write(&hi2c1, APDS9930_I2C_ADDR, reg|AUTO_INCREMENT, 1, &val, 1, 0x100) != HAL_OK ) {
		return -1;
	}
	return 0;
}	

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
int wireWriteByte(uint8_t val) {
	if ( HAL_I2C_Master_Transmit(&hi2c1, APDS9930_I2C_ADDR, &val, 1, 0x100) != HAL_OK ) {
		return -1;
	}
	return 0;
}

/**
 * @brief Reads and returns the contents of the ENABLE register
 *
 * @return Contents of the ENABLE register. 0 if error.
 */
uint8_t getMode( void ) {
    uint8_t enable_value;
    
    /* Read current ENABLE register */
    if( wireReadDataByte(APDS9930_ENABLE, &enable_value) == -1 ) {
        return 0xFF;
    }
    
    return enable_value;
}

/**
 * @brief Enables or disables a feature in the APDS-9930
 *
 * @param[in] mode which feature to enable
 * @param[in] enable ON (1) or OFF (0)
 * @return 0 if operation success. -1 otherwise.
 */
int setMode(uint8_t mode, uint8_t enable) {
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if( reg_val == 0xFF ) {
        return -1;
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
    if( wireWriteDataByte(APDS9930_ENABLE, reg_val) == -1 ) {
        return -1;
    }
        
    return 0;
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
 * @return 0 if operation successful. -1 otherwise.
 */
int setLEDDrive(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( wireReadDataByte(APDS9930_CONTROL, &val) == -1 ) {
        return -1;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 6;
    val &= 0x3F;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( wireWriteDataByte(APDS9930_CONTROL, val) == -1 ) {
        return -1;
    }
    
    return 0;
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
 * @return 0 if operation successful. -1 otherwise.
 */
int setProximityGain(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( wireReadDataByte(APDS9930_CONTROL, &val) == -1 ) {
        return -1;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 2;
    val &= 0xF3;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( wireWriteDataByte(APDS9930_CONTROL, val) == -1 ) {
        return -1;
    }
    
    return 0;
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
 * @return 0 if operation successful. -1 otherwise.
 */
int setAmbientLightGain(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( wireReadDataByte(APDS9930_CONTROL, &val) == -1 ) {
        return -1;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    val &= 0xFC;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( wireWriteDataByte(APDS9930_CONTROL, val) == -1 ) {
        return -1;
    }
    
    return 0;
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
 * @return 0 if operation successful. -1 otherwise.
 */
int setProximityDiode(uint8_t drive) {
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( wireReadDataByte(APDS9930_CONTROL, &val) == -1 ) {
        return -1;
    }
    
    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 4;
    val &= 0xCF;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( wireWriteDataByte(APDS9930_CONTROL, val) == -1 ) {
        return -1;
    }
    
    return 0;
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
int getAmbientLightGain( void ) {
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
 * @brief Sets the lower threshold for proximity detection
 *
 * @param[in] threshold the lower proximity threshold
 * @return 0 if operation successful. -1 otherwise.
 */
int setProximityIntLowThreshold(uint16_t threshold)
{
    uint8_t lo;
    uint8_t hi;
    hi = threshold >> 8;
    lo = threshold & 0x00FF;

    if( wireWriteDataByte(APDS9930_PILTL, lo) == -1 ) {
        return -1;
    }
    if( wireWriteDataByte(APDS9930_PILTH, hi) == -1 ) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Sets the high threshold for proximity detection
 *
 * @param[in] threshold the high proximity threshold
 * @return 0 if operation successful. -1 otherwise.
 */
int setProximityIntHighThreshold(uint16_t threshold)
{
    uint8_t lo;
    uint8_t hi;
    hi = threshold >> 8;
    lo = threshold & 0x00FF;

    if( wireWriteDataByte(APDS9930_PIHTL, lo) == -1 ) {
        return -1;
    }
    if( wireWriteDataByte(APDS9930_PIHTH, hi) == -1 ) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Sets the low threshold for ambient light interrupts
 *
 * @param[in] threshold low threshold value for interrupt to trigger
 * @return 0 if operation successful. -1 otherwise.
 */
int setLightIntLowThreshold(uint16_t threshold) {
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( wireWriteDataByte(APDS9930_AILTL, val_low) == -1 ) {
        return -1;
    }
    
    /* Write high byte */
    if( wireWriteDataByte(APDS9930_AILTH, val_high) == -1 ) {
        return -1;
    }
    
    return 0;
}


/**
 * @brief Sets the high threshold for ambient light interrupts
 *
 * @param[in] threshold high threshold value for interrupt to trigger
 * @return 0 if operation successful. -1 otherwise.
 */
int setLightIntHighThreshold(uint16_t threshold) {
    uint8_t val_low;
    uint8_t val_high;
    
    /* Break 16-bit threshold into 2 8-bit values */
    val_low = threshold & 0x00FF;
    val_high = (threshold & 0xFF00) >> 8;
    
    /* Write low byte */
    if( wireWriteDataByte(APDS9930_AIHTL, val_low) == -1 ) {
        return -1;
    }
    
    /* Write high byte */
    if( wireWriteDataByte(APDS9930_AIHTH, val_high) == -1 ) {
        return -1;
    }
    
    return 0;
}

int APDS9930_init( void ) {
	uint8_t id = 0;
	
	/* Read ID register and check against known values for APDS-9930 */
	if( wireReadDataByte(APDS9930_ID, &id) == -1 ) {
		printf("ID read err\r\n");
	}
	if ( (id == APDS9930_ID_1 || id == APDS9930_ID_2) ) {
		printf("ID check is %d\r\n", id);
	}
	
	/* Set ENABLE register to 0 (disable all features) */
	if( setMode(ALL, OFF) == -1 ) {
		printf( "Regs off\r\n" );
		return -1;
	}

	/* 设置环境光和邻近寄存器的默认值 */
	if( wireWriteDataByte(APDS9930_ATIME, DEFAULT_ATIME) == -1 ) {
			return -1;
	}
	if( wireWriteDataByte(APDS9930_WTIME, DEFAULT_WTIME) == -1  ) {
			return -1;
	}
	if( wireWriteDataByte(APDS9930_PPULSE, DEFAULT_PPULSE) == -1  ) {
			return -1;
	}
	if( wireWriteDataByte(APDS9930_POFFSET, DEFAULT_POFFSET) == -1  ) {
			return -1;
	}
	if( wireWriteDataByte(APDS9930_CONFIG, DEFAULT_CONFIG) == -1  ) {
			return -1;
	}
	if( setLEDDrive(DEFAULT_PDRIVE) == -1  ) {
			return -1;
	}
	if( setProximityGain(DEFAULT_PGAIN) == -1  ) {
			return -1;
	}
	if( setAmbientLightGain(DEFAULT_AGAIN) == -1  ) {
			return -1;
	}
	if( setProximityDiode(DEFAULT_PDIODE) == -1  ) {
			return -1;
	}
	if( setProximityIntLowThreshold(DEFAULT_PILT) == -1  ) {
			return -1;
	}
	if( setProximityIntHighThreshold(DEFAULT_PIHT) == -1  ) {
			return -1;
	}
	if( setLightIntLowThreshold(DEFAULT_AILT) == -1  ) {
			return -1;
	}
	if( setLightIntHighThreshold(DEFAULT_AIHT) == -1  ) {
			return -1;
	}
	if( wireWriteDataByte(APDS9930_PERS, DEFAULT_PERS) == -1  ) {
			return -1;
	}

	return 0;
}



/**
 * @brief Turns ambient light interrupts on or off
 *
 * @param[in] enable 1 to enable interrupts, 0 to turn them off
 * @return 0 if operation successful. -1 otherwise.
 */
int setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( wireReadDataByte(APDS9930_ENABLE, &val) == -1 ) {
        return -1;
    }
    
    /* Set bits in register to given value */
    enable &= 0x01;
    enable = enable << 4;
    val &= 0xEF;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( wireWriteDataByte(APDS9930_ENABLE, val) == -1 ) {
        return -1;
    }
    
    return 0;
}


/**
 * Turn the APDS-9930 on
 *
 * @return 0 if operation successful. -1 otherwise.
 */
int enablePower( void ) {
    if( setMode(POWER, 1) == -1 ) {
        return -1;
    }
    
    return 0;
}

/**
 * Turn the APDS-9930 off
 *
 * @return 0 if operation successful. -1 otherwise.
 */
int disablePower( void ) {
    if( setMode(POWER, 0) == -1 ) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Starts the light (Ambient/IR) sensor on the APDS-9930
 *
 * @param[in] interrupts 0 to enable hardware interrupt on high or low light
 * @return 0 if sensor enabled correctly. -1 on error.
 */
int enableLightSensor(uint8_t interrupts) {
    
    /* Set default gain, interrupts, enable power, and enable sensor */
    if( setAmbientLightGain(DEFAULT_AGAIN) == -1 ) {
        return -1;
    }
    if( interrupts ) {
        if( setAmbientLightIntEnable(1) == -1 ) {
            return -1;
        }
    } else {
        if( setAmbientLightIntEnable(0) == -1 ) {
            return -1;
        }
    }
    if( enablePower() == -1 ){
        return -1;
    }
    if( setMode(AMBIENT_LIGHT, 1) == -1 ) {
        return -1;
    }
    
    return 0;

}


/**
 * @brief Gets the low threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9930
 * @return 0 if operation successful. -1 otherwise.
 */
int getLightIntLowThreshold(uint16_t *threshold) {
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light low threshold, low byte register */
    if( wireReadDataByte(APDS9930_AILTL, &val_byte) == -1 ) {
        return -1;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light low threshold, high byte register */
    if( wireReadDataByte(APDS9930_AILTH, &val_byte) == -1 ) {
        return -1;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);
    
    return 0;
}

/**
 * @brief Gets the high threshold for ambient light interrupts
 *
 * @param[out] threshold current low threshold stored on the APDS-9930
 * @return 0 if operation successful. -1 otherwise.
 */
int getLightIntHighThreshold(uint16_t *threshold){
    uint8_t val_byte;
    *threshold = 0;
    
    /* Read value from ambient light high threshold, low byte register */
    if( wireReadDataByte(APDS9930_AIHTL, &val_byte) == -1 ) {
        return -1;
    }
    *threshold = val_byte;
    
    /* Read value from ambient light high threshold, high byte register */
    if( wireReadDataByte(APDS9930_AIHTH, &val_byte) == -1 ) {
        return -1;
    }
    threshold = threshold + ((uint16_t)val_byte << 8);
    
    return 0;
}



/*******************************************************************************
 * Ambient light sensor controls
 ******************************************************************************/

int readCh1Light(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from channel 0 */
    if( wireReadDataByte(APDS9930_Ch1DATAL, &val_byte) == -1 ) {
       return -1;
    }
    *val = val_byte;
    if( wireReadDataByte(APDS9930_Ch1DATAH, &val_byte) == -1 ) {
       return -1;
    }
    *val += ((uint16_t)val_byte << 8);
    return 0;
}

int floatAmbientToLux(uint16_t Ch0, uint16_t Ch1) {
    float ALSIT = 2.73 * (256 - DEFAULT_ATIME);
		float iac  = (Ch0-B*Ch1) > (C*Ch0-D*Ch1) ? (Ch0-B*Ch1) : (C*Ch0-D*Ch1);//max(Ch0 - B * Ch1, C * Ch0 - D * Ch1);
    float lpc  = GA * DF / (ALSIT * getAmbientLightGain());
    return iac * lpc;
}

int readCh0Light(uint16_t *val) {
    uint8_t val_byte;
    *val = 0;
    
    /* Read value from channel 0 */
    if( wireReadDataByte(APDS9930_Ch0DATAL, &val_byte) == -1 ) {
        return -1;
    }
    *val = val_byte;
    if( wireReadDataByte(APDS9930_Ch0DATAH, &val_byte) == -1 ) {
        return -1;
    }
    *val += ((uint16_t)val_byte << 8);
    return 0;
}

/**
 * @brief Reads the ambient (clear) light level as a 16-bit value
 *
 * @param[out] val value of the light sensor.
 * @return True if operation successful. False otherwise.
 */
int readAmbientLightLux(float *val) {
    uint16_t Ch0;
    uint16_t Ch1;
    
    /* Read value from channel 0 */
    if( readCh0Light(&Ch0) == -1 ) {
        return -1;
    }

    /* Read value from channel 1 */
    if( readCh1Light(&Ch1) == -1 ) {
        return -1;
    }

    *val = floatAmbientToLux(Ch0, Ch1);
		
		return 0;
}

unsigned long ulongAmbientToLux(uint16_t Ch0, uint16_t Ch1) {
    unsigned long ALSIT = 2.73 * (256 - DEFAULT_ATIME);
		unsigned long iac  = (Ch0-B*Ch1) > (C*Ch0-D*Ch1) ? (Ch0-B*Ch1) : (C*Ch0-D*Ch1);//max(Ch0 - B * Ch1, C * Ch0 - D * Ch1);
    unsigned long lpc  = GA * DF / (ALSIT * getAmbientLightGain());
    return iac * lpc;
}




/**
 * @brief Clears the ambient light interrupt
 *
 * @return True if operation completed successfully. False otherwise.
 */
int clearAmbientLightInt( void ) {
	
    if( wireWriteByte(CLEAR_ALS_INT) == -1 ) {
        return -1;
    }
    return 0;
}


void APDS9930_interrupt( void ) {
	float ambient_light = 0;
	uint16_t ch0 = 0;
	uint16_t ch1 = 1;
	
// Read the light levels (ambient, red, green, blue) and print
	if ( readAmbientLightLux(&ambient_light)==-1 ||
			 readCh0Light(&ch0)==-1 || 
			 readCh1Light(&ch1)==-1 ) {
		printf("Error reading light values\r\n");
	} else {
		printf("Interrupt! Ambient:%f  ", ambient_light);
		printf("Ch0:%d  ", ch0 );
		printf("Ch1:%d\r\n", ch1 );
	}
	
	// Turn on LED for a half a second
	LED_ON();
	osDelay(500);
	LED_OFF();
	
	// Reset flag and clear APDS-9930 interrupt (IMPORTANT!)
	if ( clearAmbientLightInt()==-1 ) {
		printf("Error clearing interrupt\r\n");
	}
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
}



