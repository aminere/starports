/* 

Amine Rehioui
Created: April 22nd 2012

*/

#include "Shoot.h"

#include "PNGLoader.h"

#include "File.h"

#include "png.h"
#include "pnginfo.h"

namespace shoot
{
	//! PNG malloc
	png_voidp PNGMalloc(png_structp png_ptr, png_size_t size)
	{
		return snew uchar[size];
	}

	//! PNG free
    void PNGFree(png_structp png_ptr, png_voidp ptr)
	{
		delete[] (uchar*)ptr;
	}

	//! PNG error
	void PNGError(png_structp png_ptr, png_const_charp msg)
	{
		SHOOT_ASSERT(false, msg);
	}

	//! PNG warning
	void PNGWarning(png_structp png_ptr, png_const_charp msg)
	{
		SHOOT_WARNING(false, msg);
	}

	//! PNG read
	void PNGRead(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		File* pFile = (File*)png_get_io_ptr(png_ptr);
		pFile->Read(data, length);
	}	

	void* PNGLoader::LoadFromRawPng(const std::string& path, int& width, int& height, int& channels, int& bpp, File* binFile, bool force32Bits)
	{
		File* pFile = File::CreateNative(path.c_str(), File::M_ReadBinary);
		if (!pFile->Open())
		{
			sdelete(pFile);
			return NULL;
		}

		png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
			/*error_ptr*/NULL,
			PNGError,
			PNGWarning,
			/*mem_ptr*/NULL,
			PNGMalloc,
			PNGFree);

		SHOOT_ASSERT(png_ptr, "png_create_read_struct_2 failed");

		png_infop info_ptr = png_create_info_struct(png_ptr);
		SHOOT_ASSERT(info_ptr, "png_create_info_struct failed");

		png_set_read_fn(png_ptr, pFile, PNGRead);

		int sig_read = 0;
		png_set_sig_bytes(png_ptr, sig_read);

		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

		width = info_ptr->width;
		height = info_ptr->height;

		switch (info_ptr->color_type)
		{
		case PNG_COLOR_TYPE_RGBA:
			channels = 4;
			break;

		case PNG_COLOR_TYPE_RGB:
			channels = 3;
			break;

		default: SHOOT_ASSERT(false, "Unsupported PNG format");
		}

		auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		SHOOT_ASSERT(row_bytes == width*channels, "PNG row_bytes inconsistent with image format");
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		uchar* pngData = snew uchar[row_bytes * height];
		for (int i = 0; i<height; ++i)
		{
			memcpy(pngData + (row_bytes*i), row_pointers[i], row_bytes);
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		pFile->Close();
		sdelete(pFile);

		// force non-multiple of 2 textures to 32 bit to avoid dealing with padding in 16bit
		if ((width % 2) != 0)
			force32Bits = true;

		if (force32Bits)
		{
#ifdef DX11
			bpp = 4; // it seems there is no 24bit RGB format in DX11 so always use RBGA
#else
			bpp = (channels == 3) ? 3 : 4;
#endif
		}
		else
		{
			// attempt to convert to 16 bit data to save space
#ifdef DX11
			bpp = (channels == 3) ? 2 : 4; // never use 16bit RGBA (4444) because it can't have mipmapping in DX11, so bump it to 32bits for RGBA textures.
#else
			bpp = 2;
#endif
		}

		void* data = pngData;
		if (bpp != channels)
		{
			if (bpp == 2)
			{
				ushort* pData = snew ushort[width*height];
				int row_bytes = width*channels;
				switch (channels)
				{
				case 4: // convert from 32 to 16 bit
					for (int i = 0; i<height; ++i)
					{
						for (int j = 0; j<row_bytes; j += channels)
						{
							uchar r = pngData[i*row_bytes + j + 0];
							uchar g = pngData[i*row_bytes + j + 1];
							uchar b = pngData[i*row_bytes + j + 2];
							uchar a = pngData[i*row_bytes + j + 3];
							r = uchar((float(r) * 0xf) / 0xff) & 0xf;
							g = uchar((float(g) * 0xf) / 0xff) & 0xf;
							b = uchar((float(b) * 0xf) / 0xff) & 0xf;
							a = uchar((float(a) * 0xf) / 0xff) & 0xf;
							int column = j / channels;
							int index = i*width + column;
							pData[index] = (r << 12) | (g << 8) | (b << 4) | a;
						}
					}
					break;

				case 3: // convert from 24 to 16 bit
					for (int i = 0; i<height; ++i)
					{
						for (int j = 0; j<row_bytes; j += channels)
						{
							uchar r = pngData[i*row_bytes + j + 0];
							uchar g = pngData[i*row_bytes + j + 1];
							uchar b = pngData[i*row_bytes + j + 2];
							r = uchar((float(r) * 0x1f) / 0xff) & 0x1f;
							g = uchar((float(g) * 0x3f) / 0xff) & 0x3f;
							b = uchar((float(b) * 0x1f) / 0xff) & 0x1f;
							int column = j / channels;
							int index = i*width + column;
							pData[index] = (r << 11) | (g << 5) | b;
						}
					}
					break;
				}

				sdelete_array(pngData);
				data = pData;
			}
			else if (bpp == 4)
			{
				// convert to 32 bits
				if (channels == 3)
				{
					uchar* pData = snew uchar[width*height*bpp];
					int row_bytes = width*channels;
					int curByte = 0;

					for (int i = 0; i<height; ++i)
					{
						for (int j = 0; j<row_bytes; j += channels)
						{
							uchar r = pngData[i*row_bytes + j + 0];
							uchar g = pngData[i*row_bytes + j + 1];
							uchar b = pngData[i*row_bytes + j + 2];
							pData[curByte] = r;
							pData[curByte + 1] = g;
							pData[curByte + 2] = b;
							pData[curByte + 3] = 0xff;
							curByte += 4;
						}
					}

					channels = bpp;
					sdelete_array(pngData);
					data = pData;
				}
			}
			else
			{
				SHOOT_ASSERT(false, "Texture loading error");
			}
		}

		// save to bin file
		binFile->Write(&width, sizeof(int));
		binFile->Write(&height, sizeof(int));
		binFile->Write(&channels, sizeof(int));
		binFile->Write(&bpp, sizeof(int));
		binFile->Write(data, width*height*bpp);
		return data;
	}	
}

