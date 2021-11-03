#ifndef _PERLIN_NOISE_H_
#define _PERLIN_NOISE_H_

#include <iostream>
#include <vector>
#include <time.h>

using namespace std;

#define max(x, y) x > y ? x : y
#define min(x, y) x < y ? x : y

int p[512];
int permutation[256] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

void init_data()
{
	for (int i = 0; i < 256; i++)
	{
		p[256 + i] = p[i] = permutation[i];
	}
}

static float fade(const float& t) { return t * t * t * (t * (t * 6 - 15) + 10); }

static float lerp(const float& t, const float& a, const float& b) { return a + t * (b - a); }

static float Grad(const int& hash, const float& x, const float& y, const float& z)
{
	//int h = hash & 255;

	//return ((h & 1) == 0 ? x : -x) + ((h & 2) == 0 ? y : -y);

	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static float weight(const int& octaves)
{
	float amplitude = 1.0f;
	float value = 0.0f;

	for (int i = 0; i < octaves; ++i)
	{
		value += amplitude;
		amplitude /= 2.0f;
	}
	return value;
}

thread_local uint32_t s_RndState = 10;
static const float imax = 1.0f / UINT32_MAX;
static const float irand_max = 1.0f / RAND_MAX;
float randf()
{
	uint32_t x = s_RndState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	s_RndState = x;
	return x * imax;
}

struct Perlin_Noise
{
	int width = 129;
	int height = 129;
	float roughness = 8.0f;//frequency
	float octaves = 2.0f;
	int max_height = 512;
	vector<float> heights;

	Perlin_Noise() { init_data(); generate_height_map(); }
	Perlin_Noise(float roughness_, float max_height_) : roughness(roughness_), max_height(max_height_) { init_data(); generate_height_map(); }
	Perlin_Noise(int width_, float roughness_, float octaves_, int max_height_) : width(width_), roughness(roughness_), octaves(octaves_), max_height(max_height_) { init_data(); generate_height_map(); }
	
	void generate_height_map()
	{
		heights.resize(width * height);

		srand(static_cast<unsigned int>(time(0)));
		//unsigned int time_ui = unsigned int(time(NULL));
		//srand(time_ui);

		int random = rand() % 1000;

		const float fx = width / roughness;
		
		const float fy = height / roughness;

		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				float x = float((i + random) / fx);
				float y = float((j + random) / fy);
		
				float h = normalize_2D_octaves_01(x, y, octaves);

				heights[j * width + i] = max_height * h;
			}
		}

	}

	static float Noise3D(float& x_, float& y_, float& z_)
	{
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x_)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y_)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z_)) & 255;

		float x = x_ - std::floor(x_);
		float y = y_ - std::floor(y_);
		float z = z_ - std::floor(z_);

		const float u = fade(x);
		const float v = fade(y);
		const float w = fade(z);

		const int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return lerp(w, lerp(v, lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			lerp(v, lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	static float Noise2D(float& x, float& y)
	{
		float z = 0.0f;
		return Noise3D(x, y, z);
	}

	static float accumulate_2D_octaves(float& x, float& y, const int& octaves)
	{
		float result = 0.0f;
		float amp = 1.0f;

		float a = x;
		float b = y;
		for (int i = 0; i < octaves; ++i)
		{
			result += Noise2D(a, b) * amp;
			a *= 2.0f;
			b *= 2.0f;

			amp /= 2.0f;
		}
		return result;
	}

	static float normalize_2D_octaves(float& x, float& y, const int& octaves)
	{
		float h = accumulate_2D_octaves(x, y, octaves);// / weight(octaves);

													   //cout << h << "\n";

		return h;
	}

	static float normalize_2D_octaves_01(float& x, float& y, const int& octaves)
	{
		float h = normalize_2D_octaves(x, y, octaves) * 0.5f + 0.5f;


		return h;
	}
};



#endif // !_PERLIN_NOiSE_H_