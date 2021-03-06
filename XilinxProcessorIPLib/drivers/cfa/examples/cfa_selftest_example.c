/******************************************************************************
*
* (c) Copyright 2010-14 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file cfa_selftest_example.c
*
* This file contains an example using the XCfa driver to do self test
* on the device.
*
* @note
*
* None
*
* MODIFICATION HISTORY:
* <pre>
* Ver	Who	Date	Changes
* ----- ------ -------- -----------------------------------------------
* 1.00a drg/jz	01/13/10 First Release
* 1.03a sg 	08/14/12 Updated the example for CR 666306. Modified
*			 the device ID to use the first Device Id
*			 Removed the printf at the start of the main
* 7.0	adk	01/15/14 Implimented main and XCfaSelfTestExample
*			 functions.
*			 Adherence to MISRA C 2012
*			 standard guidelines.
* </pre>
******************************************************************************/

/***************************** Include Files *********************************/

#include "xcfa.h"
#include "xparameters.h"
#include "xil_printf.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define CFA_DEVICE_ID		XPAR_CFA_0_DEVICE_ID	/**< CFA Device ID */

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


u32 XCfaSelfTestExample(u16 DeviceId);

/************************** Variable Definitions *****************************/

XCfa Cfa;		/**<Instance of the CFA Device */

/*****************************************************************************/
/**
*
* Main function to call the example.
*
* @return	XST_SUCCESS if succesful, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
int main(void)
{
	u32 Status;

	/*
	 * Run the selftest example
	 */
	Status = XCfaSelfTestExample((u16)CFA_DEVICE_ID);
	if (Status != (u32)XST_SUCCESS) {
		return (int)XST_FAILURE;
	}

	xil_printf("Successfully ran CFA Selftest Example\r\n");
	return (int)XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function does a minimal test on the XCfa driver.
*
*
* @param	DeviceId is the XPAR_<CFA_instance>_DEVICE_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if succesful, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
u32 XCfaSelfTestExample(u16 DeviceId)
{
	u32 Status;
	XCfa_Config *Config;
	u32 BeforeData = (u32)0x00; /**< Default value of the Control registers
					* Status Register and Error Register */
	u32 AfterData;

	/*
	 * Initialize the CFA driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XCfa_LookupConfig(DeviceId);
	if (NULL == Config) {
		return (u32)XST_FAILURE;
	}

	Status = XCfa_CfgInitialize(&Cfa, Config, Config->BaseAddress);
	if (Status != (u32)XST_SUCCESS) {
		return (u32)XST_FAILURE;
	}

	/*
	 * Perform a self-test to check hardware build.
	 */

	Status = XCfa_SelfTest(&Cfa);
	if (Status != (u32)XST_SUCCESS) {
		return (u32)XST_FAILURE;
	}

	/*
	 * Perform a self-test with Control register default values
	 */
	AfterData = XCfa_ReadReg(Config->BaseAddress, XCFA_CONTROL_OFFSET);
	if(BeforeData != AfterData){
		return (u32)XST_FAILURE;
	}

	/*
	 * Perform a self-test with Status register default values
	 */
	AfterData = XCfa_ReadReg(Config->BaseAddress, XCFA_STATUS_OFFSET);
	if(BeforeData != AfterData){
		return (u32)XST_FAILURE;
	}

	/*
	 * Perform a self-test with Error register default values
	 */

	AfterData = XCfa_ReadReg(Config->BaseAddress, XCFA_ERROR_OFFSET);
	if(BeforeData != AfterData){
		return (u32)XST_FAILURE;
	}
	return (u32)XST_SUCCESS;
}
