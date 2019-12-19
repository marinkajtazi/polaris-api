//
//  PolarisAPI.cpp
//  Polaris
//
//  Created by Marin Kajtazi on 04/02/2019.
//  Copyright © 2019 Marin Kajtazi. All rights reserved.
//

#include "PolarisAPI.h"
#include "ndicapi.h"

ndicapi* device = NULL;
int numOfHandles = 0;
int portHandle[NDI_MAX_HANDLES];
const char* reply;
const char* errPrefix = "NDI Polaris error";

/**
 * Method that checks if current state has any errors. After sending
 * any command to NDI Polaris, its state is updated. If last command
 * produced an error it will be written within its new state.
 *
 *	@return true if has errors, false otherwise
 */
bool ndiHasErrors()
{
	if(!ndiIsConnected()) return true;
	
	if(ndiGetError(device) != NDI_OKAY)
	{
		fprintf(stderr, "%s: %s\n", errPrefix, ndiErrorString(ndiGetError(device)));
		return true;
	}
	
	return false;
}

bool ndiIsConnected()
{
	if(device == NULL || device->SerialDevice == NDI_INVALID_HANDLE)
	{
		fprintf(stderr, "%s: NDI Polaris disconnected\n", errPrefix);
		return false;
	}
	
	return true;
}

int ndiConnect()
{
	// Initialize variables
	numOfHandles = 0;
	const char* name = NULL;
	bool foundDevice = false;
	
	// Find NDI device
	for(int i = 0; i < 20; i++)
	{
		name = ndiSerialDeviceName(i);
		if(ndiSerialProbe(name, false) == NDI_OKAY)
		{
			// Open serial communication with the found NDI device
			device = ndiOpenSerial(name);
			if(device == NULL)
			{
				fprintf(stderr, "%s: Could not open serial communication with the found NDI device\n", errPrefix);
				continue;
			}
			else
			{
				foundDevice = true;
				break;
			}
		}
	}
	
	if(foundDevice == false)
	{
		fprintf(stderr, "%s: No successful connection to NDI device was made\n", errPrefix);
		return -1;
	}
	
	// Reset NDI device
	reply = ndiRESET(device);
	if(ndiHasErrors()) return -2;
	
	// Initialize NDI device
	reply = ndiINIT(device);
	if(ndiHasErrors()) return -3;
	
	return 0;
}

void ndiDisconnect()
{
	if(device != NULL)
	{
		if(device->SerialDevice != NDI_INVALID_HANDLE)
		{
			ndiCloseSerial(device);
		}
		device = NULL;
	}
}

int ndiLoadROMFile(int *toolNum, unsigned char dataROM[1024])
{
	if(!ndiIsConnected()) return -1;
	
	if(device->IsTracking)
	{
		ndiTSTOP(device);
		if(ndiHasErrors()) return -2;
	}
	
	char hexdata[128];
	
	// Get port handles that need to be freed
	ndiPHSR(device, NDI_STALE_HANDLES);
	if(ndiHasErrors()) return -3;
	
	// Free port handles that need to be freed
	for(int i = 0; i < ndiGetPHSRNumberOfHandles(device); i++)
	{
		ndiPHF(device, ndiGetPHSRHandle(device, i));
		if(ndiHasErrors()) return -4;
	}
		
	// Request a port handle
	ndiCommand(device, "PHRQ:*********1****");
	if(ndiHasErrors()) return -5;
	
	portHandle[numOfHandles] = ndiGetPHRQHandle(device);
	
	// Load tool definition file
	for(int addr = 0; addr < 1024; addr += 64)
	{
		ndiPVWR(device, portHandle[numOfHandles], addr, ndiHexEncode(hexdata, &dataROM[addr], 64));
		if(ndiHasErrors()) return -6;
	}
	
	// Get all port handles that need to be initialized
	ndiPHSR(device, NDI_UNINITIALIZED_HANDLES);
	if(ndiHasErrors()) return -7;
	
	// Initialize all handles that need to be initialized
	for(int i = 0; i < ndiGetPHSRNumberOfHandles(device); i++)
	{
		reply = ndiPINIT(device, ndiGetPHSRHandle(device, i));
		if(ndiHasErrors()) return -8;
	}
		
	// Get all port handles that need to be enabled
	ndiPHSR(device, NDI_UNENABLED_HANDLES);
	if(ndiHasErrors()) return -9;
	
	// Enable all handles that need to be enabled
	for(int i = 0; i < ndiGetPHSRNumberOfHandles(device); i++)
	{
		reply = ndiPENA(device, ndiGetPHSRHandle(device, i), NDI_DYNAMIC);
		if(ndiHasErrors()) return -10;
	}
	
	*toolNum = numOfHandles;
	numOfHandles++;
	
	return 0;
}

int ndiCapture()
{
	if(!ndiIsConnected()) return -1;
	
	// Start tracking mode
	if(!device->IsTracking)
	{
		ndiTSTART(device);
		if(ndiHasErrors()) return -2;
	}
	
	ndiTX(device, NDI_PASSIVE_STRAY | NDI_XFORMS_AND_STATUS);
	if(ndiHasErrors()) return -3;
	
	return 0;
}

int ndiGetToolTransformation(int toolNum, double toolTransformation[8])
{
	if(!ndiIsConnected()) return -1;
	
	if(toolNum >= numOfHandles)
	{
		fprintf(stderr, "%s: Invalid tool number\n", errPrefix);
		return -2;
	}
	
	if(ndiGetTXTransform(device, portHandle[toolNum], toolTransformation) != NDI_OKAY)
	{
		return -3;
	}
	
	return 0;
}

int ndiGetNumOfStrayMarkers()
{
	return ndiGetTXNumberOfPassiveStrays(device);
}

int ndiGetStrayMarkersPos(int strayMarkerNum, double markerCoord[3])
{
	if(!ndiIsConnected()) return -1;
	
	if(ndiGetTXPassiveStray(device, strayMarkerNum, markerCoord) != NDI_OKAY)
	{
		return -2;
	}
	
	return 0;
}
