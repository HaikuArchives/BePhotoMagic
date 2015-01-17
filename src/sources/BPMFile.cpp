/**********************************************************************
	BPM.cpp:	BePhotoMagic file reader class
				This is explicitly NOT a translator to allow for Layers
				Copyright 2001 BPM Dev Team
	version 1.0
**********************************************************************/
#include "BPMFile.h"

/* Usage:	Create a BPMImage object and pass it to the constructor or use
			SetImage. Then either call ReadProject or WriteProject to open
			or save the thing.
*/

BPMFileHeader::BPMFileHeader(void)
{	sprintf(signature,"BePhotoMagic 1.00");
	layers=1;
	guides=0;
	width=0;
	height=0;

	// Compression is not tweaked elsewhere.
	// Set to 0=write all files uncompressed
	// Set to 1=write all files with BZip2 compression
	compression=1;
}

BPMFile::BPMFile(BPMImage *image)
{	data=image;
	initialized=true;
}

BPMFile::BPMFile(void)
{	initialized=false;
}

BPMImage * BPMFile::ReadProject(const char *path)
{	
	printf("\nReadProject()\n");
	uint32 stat=file.SetTo(path,B_READ_ONLY);
	if(stat != B_OK)
	{
//		printf("ReadProject: Couldn't set file %s\n",path);
		return NULL;
	}

	BPMImage *newimg=(BPMImage *) new BPMImage();
	data=newimg;
	stat=ReadHeader();	
	if(data->width==0 || data->height==0)
	{
//		printf("ReadProject: invalid dimensions\n");
		delete newimg;
		return NULL;
	}
//	if(header.compression==0)
//		printf("ReadProject: Uncompressed layers\n");
//	else
//		if(header.compression==1)
//			printf("ReadProject: BZ2ed  layers\n");
	for(uint32 i=0;i<data->number_layers;i++)
	{
//		printf("\nLayer %ld\n",i);
		stat=ReadLayer(i,header.compression);
		if(stat != BPM_OK)
		{
//			printf("ReadProject: ReadLayer returned NULL\n");
			delete newimg;
			return NULL;
		}
	}
	file.Unset();
	
	// Do some other initializations to prevent crashes
	data->display_bitmap=new BBitmap(BRect(0,0,data->width-1,data->height-1),B_RGBA32,true);
	data->work_bitmap=new BBitmap(BRect(0,0,data->width-1,data->height-1),B_RGBA32,true);
	data->selection=new BBitmap(data->work_bitmap);
	data->currentbrush=new BBitmap(BRect(0,0,31,31),B_RGBA32,true);
	data->MakeBrush(data->brush_width,data->brush_height,data->brush_softness,data->brushtype);
	data->bottomlayer=new BBitmap(BRect(0,0,1,1),B_RGB32);
	data->toplayer=new BBitmap(BRect(0,0,1,1),B_RGB32);
	return newimg;
}

bpm_status BPMFile::WriteProject(const char *path, uint32 flags=0)
{	
	if(initialized)
	{
//		printf("\nWriteProject()\n");
		uint32 stat=file.SetTo(path,B_WRITE_ONLY | flags);
		BNodeInfo fileinfo(&file);

		if(stat != B_OK)
			return stat;
	
		stat=WriteHeader();
		if(stat != BPM_OK)
			return stat;
		for(uint32 i=0;i<data->number_layers;i++)
		{
//			printf("\nLayer %ld\n",i);
			WriteLayer(i,header.compression);
		}
		fileinfo.SetType("image/bephotomagic");
		file.Unset();
		return BPM_OK;
	}

	return BPM_FILE_UNINITIALIZED;
}

bpm_status BPMFile::ReadHeader(void)
{	
//	printf("\nReadHeader\n");
	// get header data
	file.Seek(0,SEEK_SET);
	file.Read((void *)&header,sizeof(BPMFileHeader));
	
	// make assignments as according
	data->width=header.width;
//	printf("ReadHeader: Image Width- %ld\n",data->width);
	data->height=header.height;
//	printf("ReadHeader Image Height- %ld\n",data->height);
	data->number_layers=header.layers;
//	printf("ReadHeader Layer Count: %d\n",data->number_layers);
	return BPM_OK;
}

bpm_status BPMFile::WriteHeader(void)
{	
//	printf("\nWriteHeader\n");
	// set header data
	header.layers=data->number_layers;
//	printf("WriteHeader Layer Count: %ld\n",header.layers);
	header.width=data->width;
//	printf("WriteHeader Image Width: %ld\n",header.width);
	header.height=data->height;
//	printf("WriteHeader Image Height: %ld\n",header.height);
	header.guides=0;
	header.mask=false;

	// write data
	file.Seek(0,SEEK_SET);
	file.Write((void *)&header,sizeof(BPMFileHeader));
	return BPM_OK;
}

bpm_status BPMFile::ReadLayer(uint32 index,uint32 compression)
{
	int32 stat;
	
	if(index < 0 || index > uint16(data->number_layers-1))
		return BPM_BAD_PARAMETER;

	data->layers[index]=new Layer();
	Layer *layer=(Layer *)data->layers[index];

	if(layer==NULL)
	{	printf("Layer * is NULL\n");
		return false;
	}
//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->name,255);	// name is fixed array
//	printf("ReadLayer: Name %s\n",layer->name);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->width,sizeof(uint32));
//	printf("ReadLayer: Width %lu\n",layer->width);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->height,sizeof(uint32));
//	printf("ReadLayer: Height %lu\n",layer->height);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->opacity,sizeof(uint8));
//	printf("ReadLayer: Opacity %u\n",layer->opacity);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->blendmode,sizeof(uint8));
//	printf("ReadLayer: Blendmode %u\n",layer->blendmode);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->id,sizeof(int32));
//	printf("ReadLayer: ID %ld\n",layer->id);

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&layer->visible,sizeof(bool));
//	printf("ReadLayer: Visible %d\n",layer->visible);

	layer->bitmap=new BBitmap(BRect(0,0,layer->width-1,layer->height-1),B_RGBA32,true);

//	uint32 byteslength=layer->bitmap->BitsLength();
	uint32 byteslength;

//	printf("File position: %lx\n",(int32)file.Position());
	file.Read((void *)&byteslength,sizeof(uint32));
//	printf("ReadLayer: Byteslength %lu\n",byteslength);
	
	switch(compression)
	{
		case 0:
		{
			file.Read((void *)layer->bitmap->Bits(),byteslength);
			break;
		}
		case 1:	// BZip2 compression
		{
			if(byteslength>15000000L)
			{	// If the layer is bigger than 15MB, we won't have enough memory
				// to do this...
				delete layer;
				return BPM_BAD_ALLOC;
			}
			unsigned int cbytes;
//			printf("File position: %lx\n",(int32)file.Position());
			file.Read((void *)&cbytes,sizeof(unsigned int));
//			printf("ReadLayer: BZ2ed bytes to read: %u\n",cbytes);

			char cbuffer[cbytes];
			stat=file.Read((void *)cbuffer,cbytes);
//			printf("ReadLayer: BZ2ed bytes read: %ld\n",stat);
			
			char ucbuffer[byteslength],
				*bmpbits=(char *)layer->bitmap->Bits();
			if(ucbuffer==NULL)
			{	delete layer;
				return BPM_BAD_ALLOC;
			}
//printf("ReadLayer: byteslength before: %lu\n",byteslength);
			BZ2_bzBuffToBuffDecompress(ucbuffer,(unsigned int *)&byteslength,
						cbuffer,cbytes,0,4);
			memcpy (bmpbits,ucbuffer,byteslength);
//printf("ReadLayer: byteslength after: %lu\n",byteslength);
			break;
		}
		default:
		{	return BPM_BAD_PARAMETER;
			break;
		}
	}
	return BPM_OK;
}

bpm_status BPMFile::WriteLayer(uint32 index,uint32 compression)
{	
	int32 stat;
	if(index < 0 || index > uint16(data->number_layers-1))
		return BPM_BAD_PARAMETER;

	Layer *layer=(Layer *)data->layers[index];

//	printf("Writing data to %lx\n",(int32)file.Position());
	file.Write((void *)layer->name,255);	// name is fixed array
//	printf("WriteLayer: Name: %s\n",layer->name);
	file.Write((void *)&layer->width,sizeof(uint32));
//	printf("WriteLayer: Width: %lu\n",layer->width);
	file.Write((void *)&layer->height,sizeof(uint32));
//	printf("WriteLayer: Height: %lu\n",layer->height);
	file.Write((void *)&layer->opacity,sizeof(uint8));
//	printf("WriteLayer: Opacity: %u\n",layer->opacity);
	file.Write((void *)&layer->blendmode,sizeof(uint8));
//	printf("WriteLayer: Blendmode: %u\n",layer->blendmode);
	file.Write((void *)&layer->id,sizeof(int32));
//	printf("WriteLayer: ID: %ld\n",layer->id);
	file.Write((void *)&layer->visible,sizeof(bool));
//	printf("WriteLayer: Visible: %d\n",layer->visible);
 	
	uint32 byteslength=layer->bitmap->BitsLength();
	file.Write((void *)&byteslength,sizeof(uint32));
//	printf("WriteLayer: Byteslength: %lu\n",byteslength);
	switch(compression)
	{
		case 0:	// uncompressed
		{
			stat=file.Write((void *)layer->bitmap->Bits(),byteslength);
//			printf("WriteLayer: Uncompressed bytes written: %ld\n",stat);
			break;
		}
		case 1:	// BZip2 compression
		{
			unsigned int cbytes;
			char cbuffer[byteslength + (byteslength/100) + 600];

			BZ2_bzBuffToBuffCompress(cbuffer, &cbytes,
		      (char *)layer->bitmap->Bits(), byteslength,9,4,30);

			file.Write((void *)&cbytes,sizeof(unsigned int));
//			printf("WriteLayer: Compressed Bytes: %d\n",cbytes);
			stat=file.Write((void *)cbuffer,cbytes);
//			printf("WriteLayer: BZ2ed bytes written: %ld\n",stat);
			break;
		}
		default:
		{	return BPM_BAD_PARAMETER;
			break;
		}
	}
	return BPM_OK;
}

void BPMFile::SetImage(BPMImage *image)
{	data=image;
}
