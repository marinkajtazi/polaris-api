//
//  main.c
//  Test
//
//  Created by Marin Kajtazi on 05/02/2019.
//  Copyright © 2019 Marin Kajtazi. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include "PolarisAPI.h"

#define ROM_1_PATH "path/to/your/first/rom/file"
#define ROM_2_PATH "path/to/your/second/rom/file"

int main(int argc, const char * argv[])
{
	unsigned char dataROM[2][1024];
	FILE *file;
	
	// 8700339.rom
	file = fopen(ROM_1_PATH, "rb");
	if(file == NULL) return -1;
	memset(dataROM[0], 0, 1024);
	fread(dataROM[0], 1, 1024, file);
	if(ferror(file))
	{
		fclose(file);
		return -2;
	}
	fclose(file);
	
	// pacijent.rom
	file = fopen(ROM_2_PATH, "rb");
	if(file == NULL) return -1;
	memset(dataROM[1], 0, 1024);
	fread(dataROM[1], 1, 1024, file);
	if(ferror(file))
	{
		fclose(file);
		return -2;
	}
	fclose(file);
	
	int tool0, tool1;
	
	if(ndiConnect()) return -1;
	if(ndiLoadROMFile(&tool0, dataROM[0])) return -2;
	if(ndiLoadROMFile(&tool1, dataROM[1])) return -3;
	
	while(true)
	{
		double tool0Trans[8];
		double tool1Trans[8];
		
		ndiCapture();
		int tool0Found = ndiGetToolTransformation(tool0, tool0Trans);
		int tool1Found = ndiGetToolTransformation(tool1, tool1Trans);
		
		if(tool0Found == 0)
			printf("Tool 0: %f %f %f %f %f %f %f %f\n", tool0Trans[0], tool0Trans[1], tool0Trans[2], tool0Trans[3], tool0Trans[4], tool0Trans[5], tool0Trans[6], tool0Trans[7]);
		else printf("Tool 0: NOT FOUND\n");
		
		if(tool1Found == 0)
			printf("Tool 1: %f %f %f %f %f %f %f %f\n", tool1Trans[0], tool1Trans[1], tool1Trans[2], tool1Trans[3], tool1Trans[4], tool1Trans[5], tool1Trans[6], tool1Trans[7]);
		else printf("Tool 1: NOT FOUND\n");
	}
	
	return 0;
}

