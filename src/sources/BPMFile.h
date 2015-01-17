/**********************************************************************
	BPM.cpp:	BePhotoMagic file reader class
				This is NOT a translator explicitly to allow for Layers
				Copyright 2001 BPM Dev Team
	version 1.0
**********************************************************************/
#ifndef _BPM_FILE_H_

#define _BPM_FILE_H_

#include <Bitmap.h>
#include <File.h>
#include <string.h>
#include <NodeInfo.h>

#include "bzlib.h"		// bzlib.h tweaked to include stdio.h file for FILE descriptor
#include "ErrorCodes.h"
#include "BPMImage.h"	// defs from main app

#define BPM_FILE_UNINITIALIZED 100
#define BPM_FILE_BAD_VERSION 101
#define BPM_FILE_BAD_HEADER	102
#define BPM_FILE_BAD_LAYER	103
#define BPM_FILE_MISSING_LAYER_TAG 104


class BPMFileHeader
{
public:
	BPMFileHeader(void);
	char signature[20];	// string 'BePhotoMagic x.xx' plus buffer
	uint32 compression;	// 0 = Uncompressed
						// 1 = BZip2 compressed
	uint32 width;
	uint32 height;
	uint32 layers;
	uint32 guides;		// also currently unimplemented - BPM doesn't use guides yet
	uint32 mask;		// ==1 if the first "layer" is actually the selection
};

class BPMFile
{
public:
	BPMFile(BPMImage *image);
	BPMFile(void);

	BFile file;

	BPMFileHeader header;
	bpm_status WriteProject(const char *path, uint32 flags=0);
	BPMImage *ReadProject(const char *path);
	void SetImage(BPMImage *image);

	bpm_status ReadHeader(void);
	bpm_status WriteHeader(void);

	bpm_status ReadLayer(uint32 index,uint32 compression);
	bpm_status WriteLayer(uint32 index,uint32 compression);

	bpm_status ReadMask(void);
	bpm_status WriteMask(void);
	
	BPMImage *data;
	bool initialized;
};

#endif