/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non - commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain.We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors.We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

//2017 aaedev@gmail.com version 1.0
//See code for implementation details

#include <windows.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <regex>
#include "log.h"
#define STB_IMAGE_IMPLEMENTATION   
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define debug 0

int colorChannel = 0;
int g_keyColor[3] = { 0, 0, 0 };
std::string mchannels;
std::string newfn;			//Filename to use for output
std::vector<string> args;   // Used to house list of arguments


bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}


vector<string> split(const char *str, char c = ',')
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}
//----------------------------------TEXTURE Handling----------------------------------

int LoadPNG(std::string const &filename)
{

	int x = 0;
	int y = 0;
	int comp = 0;
	unsigned char *image_data;

	//stbi_set_flip_vertically_on_load(true);

	image_data = stbi_load(filename.c_str(), &x, &y, &comp, STBI_rgb_alpha);

	if (!image_data) {
		wrlog("ERROR: could not load %s\n", filename.c_str());
		return 0;
	}
	wrlog("Texture x is %d, y is %d, components %d", x, y, comp);
	// NPOT check, we checked the card capabilities beforehand, flag if it's not npot

	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		wrlog("WARNING: texture %s is not power-of-2 dimensions\n", filename.c_str());
	}

	int imageSize_RGBA = x * y * comp;
	int numColorComponents = 4;
	int width = x;
	int height = y;

	//unsigned char *imagePtr = &image_data[0];
	if (!colorChannel)
	{
		wrlog("Processing image with Color Key -----------------");
	}
	////////////////////// Color Key Conversion && Alpha channel maker ////////////////////////////////////
	//Start processing the image
	for (int i = 0; i < imageSize_RGBA; i += 4)
	{
		//Are we using the key color? If so, process
		// Does the current pixel match the selected color key?
		if (!colorChannel)
		{
			//We are processing the file using the KeyColor Information to build the Alpha Channel.
			if (image_data[i] == g_keyColor[0] && image_data[i + 1] == g_keyColor[1] && image_data[i + 2] == g_keyColor[2])
			{
				image_data[i + 3] = 0;
			}   // If so, set alpha to 0. (Fully transparent) - Allow this to be variable.
			else
			{
				image_data[i + 3] = 255;
			}

		}
		else
		{
			if (colorChannel)
			{
				if (colorChannel == 1) //R
				{
					image_data[i + 3] = image_data[i];
				}
				else if (colorChannel == 2) //G
				{
					image_data[i + 3] = image_data[i + 1];
				}
				else if (colorChannel == 4) //B
				{
					image_data[i + 3] = image_data[i + 2];
				}
			}
			else
			{
				image_data[i + 3] = 255;
			}

		} // If not, set alpha to fully opaque.
	}

	//Write out the new file

	//Are we changing the filename or using the same one with the append?
	if (newfn.empty())
	{
		newfn = filename;
		newfn = std::regex_replace(newfn, std::regex(".png"), "_alpha.png");
	}

	wrlog("new string is %s", newfn.c_str());
	wrlog("New Texture created:");
	wrlog("OK");

	stbi_write_png(newfn.c_str(), x, y, comp, image_data, x * comp);
	stbi_image_free(image_data);
	return 0;
}


static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << " <infile>.png <option(s)>\n"
		<< "Options:\n"
		<< "\t-h,--help\tShow this help message\n"
		<< "\t-k \t<num,num,num>Set a new color key for all RGB Channels, otherwise 255,255,255 (black) is used.\n"
		<< "\t-c \t<channel> Force R/r or G/g or B/b for SINGLE channel to use for alpha, otherwise the default or selected key is used.\n"
		<< "\t-f \t<outfile.png> Specify out filename, otherwise it's <infile>_alpha.png\n"
		<< std::endl;
}


int main(int argc, char *argv[])
{
	// Do we at least have a png file to open?
	if (argc < 2)
	{
		//If not, show usage and exit.
		show_usage(argv[0]);
		return 1;
	}
	//Have to add one.
	args.push_back("front");
	if (debug) cout << "Input arguments are : " << argc << endl;
	//Now handle the help and fill the arguments.
	for (int i = 1; i < argc; ++i)
	{
		args.push_back(argv[i]);
		if ((args[i] == "-h") || (args[i] == "--help"))
		{
			if (debug) cout << "Input arguments are : " << argv[0] << endl;
			show_usage(argv[0]);
			return 0;
		}
		//
	}

	//Open the log
	LogOpen("AlphaAddLogOut.txt");

	//Now handle the help and fill the arguments.
	for (int i = 1; i < argc; ++i)
	{
		cout << "Input arguments are : " << args[i] << endl;
		//Check for output file name:
		if (args[i] == "-f")
		{
			//Make sure someone didn't do something stupid
			if ((i + 1) < argc)
			{
				if (!ends_with(args[i + 1], ".png"))
				{
					cout << "Output filename must be specified as .png?\n" << endl;
				}
				else newfn = args[i + 1];
			}
		}
		else if (args[i] == "-c")
		{
			mchannels = args[i + 1];
			if (!mchannels.empty())
			{
				if (mchannels.length() > 1)
				{
					wrlog("Error - colorChannel can only be ONE value");
					std::cout << "Error - colorChannel can only be ONE R/G/B value" << "\n";
					show_usage("alphaadd.exe");
					exit(1);
				}

				if (mchannels[0] == 'r' || mchannels[0] == 'R')
				{
					colorChannel = 1;
				}
				else if (mchannels[0] == 'g' || mchannels[0] == 'G')
				{
					colorChannel = 2;
				}
				else if (mchannels[0] == 'b' || mchannels[0] == 'B')
				{
					colorChannel = 4;
				}

				std::cout << "colorChannel is  " << colorChannel << "\n";
			}

		}

		else if (args[i] == "-k")
		{
			//Make sure someone didn't do something stupid
			if ((i + 1) < argc)
			{
				const char *splitme = "255,128,12";
				vector<string> done = split(args[i + 1].c_str());
				//Again, check for error
				if (done.size() < 3)
				{
					wrlog("Error, not enough rgb values only %d submitted, need 3. It should be in the form 255,255,255", done.size());
				}
				(int)g_keyColor[0] = std::stoi(done[0]);
				(int)g_keyColor[1] = std::stoi(done[1]);
				(int)g_keyColor[2] = std::stoi(done[2]);
				wrlog("Key color is %d %d %d", g_keyColor[0], g_keyColor[1], g_keyColor[2]);

			}
		}

	}

	//*** Handle the input png.
	std::string temp = argv[1];
	if (ends_with(temp, ".png"))
	{
		wrlog("Yes , Filename ends with .png");
		LoadPNG(temp);
	}
	else
	{
		show_usage(argv[0]);
		wrlog("No - Filename doesn't end with .png - ending");
		return 1;
	}

	LogClose();
	return 0;
}
