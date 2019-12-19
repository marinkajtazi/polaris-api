//
//  PolarisAPI.hpp
//  Polaris
//
//  Created by Marin Kajtazi on 04/02/2019.
//  Copyright © 2019 Marin Kajtazi. All rights reserved.
//

#ifndef PolarisAPI_hpp
#define PolarisAPI_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	/**
	 * Method that checks if NDI Polaris device is currently connected
	 * (i.e. has an active serial communication).
	 *
	 * @return <code>true</code> if connected, false otherwise
	 */
	bool ndiIsConnected(void);
	
	/**
	 * Method that tries to connect to NDI Polaris device. If it's already connected,
	 * return with no error message. 
	 *
	 * @return error code if error was produced, 0 otherwise
	 */
	int ndiConnect(void);
	
	/**
	 * Method that disconnects from NDI Polaris device, if it's connected.
	 */
	void ndiDisconnect(void);
	
	/**
	 * Method that loads a tool definition file (.rom extension) data, initializes
	 * it and enables that tool handle.
	 *
	 * @param toolNum - number of loaded tool is returned through this parameter
	 * @param dataROM - 1024 byte long tool definition file data
	 *
	 * @return error code if error was produced, 0 otherwise
	 */
	int ndiLoadROMFile(int *toolNum, unsigned char dataROM[1024]);
	
	/**
	 * Method that captures current tools and stray markers position seen by the
	 * NDI Polaris camera.
	 * Use methods ndiGetToolTransformation, ndiGetNumOfStrayMarkers and
	 * ndiGetStrayMarkersPos to get those captured informations.
	 *
	 * @return error code if error was produced, 0 otherwise
	 */
	int ndiCapture(void);
	
	/**
	 * Method that gets last captured transformation of a toolNum tool.
	 *
	 * @param toolNum - tool number of a tool whose transformation is wanted
	 * @param toolTransformation - tool transformation is returned through this
	 *        parameter (first 4 numbers - quaternion, next 3 numbers - coordinates
	 *        in milimeters, last number - unitless error estimate)
	 *
	 * @return error code if error was produced, 0 otherwise
	 */
	int ndiGetToolTransformation(int toolNum, double toolTransformation[8]);
	
	/**
	 * Method that returns number of stray markers from the last capture.
	 *
	 * @return number of stray markers from last capture
	 */
	int ndiGetNumOfStrayMarkers(void);
	
	/**
	 * Method that gets stray marker 3D coordinates in milimeters of a strayMarkerNum
	 * marker from the last capture.
	 *
	 * @param strayMarkerNum - stray marker number of a marker whose coordinates are wanted
	 * @param markerCoord - marker coordinates are returned through this parameter
	 *
	 * @return error code if error was produced, 0 otherwise
	 */
	int ndiGetStrayMarkersPos(int strayMarkerNum, double markerCoord[3]);
	
#ifdef __cplusplus
}
#endif

#endif /* PolarisAPI_hpp */
