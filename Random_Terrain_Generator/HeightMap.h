#ifndef _HEIGHT_MAP_H
#define _HEIGHT_MAP_H
#include <vector>
#include "stb_image.h"

using namespace std;

struct Height_Map
{
	vector<float> heights;
	int width = 0;
	int height = 0;
	int max_height = 512;

	Height_Map() {}
	Height_Map(string& image_name, int max_height_) : max_height(max_height_)
	{
		generate_height_map(image_name);
	}

	void generate_height_map(string& image_name)
	{
		int w, h, n;

		unsigned char* c = stbi_load(image_name.c_str(), &w, &h, &n, 0);

		width = w;
		height = h;

		heights.resize(w * h);

		/*for (int i = 0; i < w; ++i)
		{
			for (int j = 0; j < h; ++j)
			{
				 float h = c[j * w + i] / 255.0f;

				 heights[j * w + i] = h * max_height;
			}
		}*/

		for (int i = 0; i < w * h; ++i)
		{
			float x = c[3 * i] / 255.0f;
			//float y = c[3 * i + 1] / 255.0f;
			//float z = c[3 * i + 2] / 255.0f;

			heights[i] = max_height * x;
			//heights[3 * i + 1] = max_height * y;
			//heights[3 * i + 2] = max_height * z;
		}
	}
};

#endif // !_HEIGHT_MAP_H

