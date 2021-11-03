#ifndef _DIAMOND_SQUARE_H_
#define _DIAMOND_SQUARE_H_
#include <vector>
#include <time.h>

#define min(x, y) x < y ? x : y
#define max(x, y) x > y ? x : y

using namespace std;

static float random(const float& low, const float& high)
{
	return low + (high - low) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

struct Diamond_Square
{
	int size;//size = 2 ^ n + 1
	float grid_size = 4.0f;
	float roughness = 1.0f;
	int maximum_height = 512;
	vector<float> heights;

	Diamond_Square(int size_ = 129, int maximum_height_ = 512) : size(size_), maximum_height(maximum_height_)
	{
		heights.resize(size * size);
		generate_height_map();
	}

	int height_index_at(const int& i, const int& j)
	{
		return ((j + size) % size) * size + ((i + size) % size);
	}

	void generate_height_map()
	{
		srand(static_cast<unsigned int>(time(0)));

		fill(heights.begin(), heights.end(), 0.0f);

		int index_size = size - 1;
		float dH = index_size * 0.5f;
		float dHFactor = powf(2.0f, -roughness);

		float minH = 0.0f, maxH = 0.0f;


		for (int w = index_size; w > 0; dH *= dHFactor, w /= 2)
		{
			for (int x = 0; x < index_size; x += w)
			{
				for (int z = 0; z < index_size; z += w)
				{
					//diamond step
					int p1 = height_index_at(x, z);
					int p2 = height_index_at(x + w, z);
					int p3 = height_index_at(x + w, z + w);
					int p4 = height_index_at(x, z + w);

					int mid = height_index_at(x + w / 2, z + w / 2);

					heights[mid] = random(-dH, dH) + (heights[p1] + heights[p2] + heights[p3] + heights[p4]) * 0.25f;

					minH = min(minH, heights[mid]);
					maxH = max(maxH, heights[mid]);
				}
			}

			for (int x = 0; x < index_size; x += w)
			{
				for (int z = 0; z < index_size; z += w)
				{
					//square step
					int p1 = height_index_at(x, z);
					int p2 = height_index_at(x + w / 2, z - w / 2);
					int p3 = height_index_at(x + w, z);
					int p4 = height_index_at(x + w / 2, z + w / 2);

					int mid = height_index_at(x + w / 2, z);

					heights[mid] = random(-dH, dH) + (heights[p1] + heights[p2] + heights[p3] + heights[p4]) * 0.25f;

					minH = min(minH, heights[mid]);
					maxH = max(maxH, heights[mid]);

					p1 = height_index_at(x, z);
					p2 = height_index_at(x + w / 2, z + w / 2);
					p3 = height_index_at(x, z + w);
					p4 = height_index_at(x - w / 2, z - w / 2);

					mid = height_index_at(x, z + w /2);

					heights[mid] = random(-dH, dH) + (heights[p1] + heights[p2] + heights[p3] + heights[p4]) * 0.25f;

					minH = min(minH, heights[mid]);
					maxH = max(maxH, heights[mid]);
				}
			}
		}

		int num_height = size * size;

		smooth();

		float inv_range = 1.0f / (maxH - minH);
		for (int i = 0; i < num_height; ++i)
			heights[i] = maximum_height * (heights[i] - minH) * inv_range;
	}

	void smooth()
	{
		int num_height = size * size - 1;

		float value = 0.0f;
		float cellAverage = 0.0f;


		// --------------->i
		// |
		// |
		// |
		// v
		// j
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				float value = 0.0f;
				float cellAverage = 0.0f;
				int index;

				index = (j - 1) * size + (i - 1);
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = (j - 1) * size + i;
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = (j - 1) * size + (i + 1);
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = j * size + (i - 1);
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = j * size + i;
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = j * size + i + 1;
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = (j + 1) * size + (i - 1);
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = (j + 1) * size + i;
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				index = (j + 1) * size + (i + 1);
				if (index > 0 && index < num_height)
				{
					value += heights[index];
					cellAverage += 1.0f;
				}

				int mid = j * size + i;
				heights[mid] = value / cellAverage;
			}
		}
	}
};

#endif // !_DIAMOND_SQUARE_H_
