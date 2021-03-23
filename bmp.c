/*!
 * https://www.vbforums.com/showthread.php?261522-C-C-Loading-Bitmap-Files-%28Manually%29
 * and
 * Liyanboy74
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "bmp.h"

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); // small edit. forgot to add the closing bracket at sizeof

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0;imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3) // fixed semicolon
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap iamge data
    fclose(filePtr);
    return bitmapImage;
}

unsigned char SaveBitmap24File(char * name,uint16_t width,uint16_t height,unsigned char * Buffer)
{
    int i,j;
    uint8_t *Buf,rname[64];
    FILE *img;
    uint32_t ImgMByte=(((width*3)+(width%4))*height);

    BITMAPFILEHEADER info;
    memset(&info,0,sizeof(info));

    info.bfType=0x4d42;
    info.bfSize=0x36+ImgMByte;
    info.bfOffBits=0x36;

    BITMAPINFOHEADER header;
    memset(&header,0,sizeof(header));

    header.biSize=0x28;
    header.biWidth=width;
    header.biHeight=height;
    header.biPlanes=0x01;
    header.biBitCount=24;
    header.biSizeImage=ImgMByte;
    header.biXPelsPerMeter=0x0b13;
    header.biYPelsPerMeter=0x0b13;

    sprintf(rname,"%s.bmp",name);
    img=fopen(rname,"wb");
    if(img==NULL)return 0;

    Buf=(uint8_t *)calloc(ImgMByte,sizeof(unsigned char));
    memset(Buf,0x00,ImgMByte);

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            Buf[(i*((width*3)+(width%4)))+(j*3)+0]=Buffer[(i*width*3)+(j*3)+0];
            Buf[(i*((width*3)+(width%4)))+(j*3)+1]=Buffer[(i*width*3)+(j*3)+1];
            Buf[(i*((width*3)+(width%4)))+(j*3)+2]=Buffer[(i*width*3)+(j*3)+2];
        }
    }

    fwrite(&info,sizeof(BITMAPFILEHEADER),1,img);
    fwrite(&header,sizeof(BITMAPINFOHEADER),1,img);
    fwrite(Buf,ImgMByte,1,img);

    fclose(img);
    free(Buf);
    return 1;
}
