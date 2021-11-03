#include <gl\glew.h>
#include "Utility.h"
#include "Controls.h"
#include <gl\glfw3.h>
#include <gl\freeglut.h>
#include<unordered_map>
#include "Diamond_Square.h"
#include "Perlin_Noise.h"
#include "HeightMap.h"

//int screenwidthidth = 1200;
//int screenHeight = 800;

GLuint vao;
GLuint vbo[3];
GLuint ibo;

GLuint mvpLoc;
GLuint nLoc;

GLuint program;

//GLuint num_trs;


GLuint globalAmbLoc;
GLuint ambLoc;
GLuint diffLoc;
GLuint specLoc;
GLuint posLoc;

//Light Properties
float a = 0.4f;
float globalAmbient[4] = { a, a, a, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


void init_light(GLuint program, vec3 light_position)
{
	float light_pos[3];
	light_pos[0] = light_position.x;
	light_pos[1] = light_position.y;
	light_pos[2] = light_position.z;

	globalAmbLoc = glGetUniformLocation(program, "globalAmbient");

	ambLoc = glGetUniformLocation(program, "light.ambient");
	diffLoc = glGetUniformLocation(program, "light.diffuse");
	specLoc = glGetUniformLocation(program, "light.specular");
	posLoc = glGetUniformLocation(program, "light.position");

	glProgramUniform4fv(program, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(program, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(program, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(program, specLoc, 1, lightSpecular);
	glProgramUniform3fv(program, posLoc, 1, light_pos);
}


int width, h, n;
int num_trs;

const float GRID_SIZE = 32.0f;
const float HEIGHT_SCALE = 4.0f;

struct Normal_Vertex
{
	vec3 normal = vec3(0.0f);
	int count = 0;
	Normal_Vertex() {}
};

void init_data(const Diamond_Square& Ds)
{

	/*for (int i = 0; i < Ds.size; ++i)
	{
	for (int j = 0; j < Ds.size; ++j)
	cout << Ds.heights[j * Ds.size + i] << "\n";
	}*/

	mvpLoc = glGetUniformLocation(program, "mvp_matrix");
	nLoc = glGetUniformLocation(program, "normal_matrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(3, vbo);
	glGenBuffers(1, &ibo);


	vector<float> v;

	int w = Ds.size;
	int h = Ds.size;

	float grid_size = Ds.grid_size;

	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			//int ind = 3 * (j * width + i);
			//float height = (data[ind] / 255.0f) * 40;

			v.emplace_back((float)i * GRID_SIZE);
			v.emplace_back(Ds.heights[j * w + i] * HEIGHT_SCALE);
			v.emplace_back((float)j * GRID_SIZE);
		}
	}
	//cout << v.size();
	//init texture data
	vector<float> vt;
	float iwidth1 = 1.0f / (w + 1);
	float ih1 = 1.0f / (h + 1);
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			//cout << i*iwidth1 << "\n";

			vt.emplace_back(i * iwidth1);
			vt.emplace_back(j * ih1);
		}
	}

	//create normal

	//vector<vec3> vn;

	//vn.resize(w * h);


	vector<unsigned int> fs;
	for (int i = 0; i < w - 1; ++i)
	{
		for (int j = 0; j < h - 1; ++j)
		{

			fs.emplace_back(j * w + i);
			fs.emplace_back((j + 1) * w + i);
			fs.emplace_back(j * w + i + 1);

			fs.emplace_back(j * w + i + 1);
			fs.emplace_back((j + 1) * w + i);
			fs.emplace_back((j + 1) * w + i + 1);
		}
	}

	unordered_map<int, Normal_Vertex> normal_map;

	for (int i = 0; i < fs.size(); i += 3)
	{
		int ind0 = fs[i];
		int ind1 = fs[i + 1];
		int ind2 = fs[i + 2];

		vec3 v0 = vec3(v[3 * ind0], v[3 * ind0 + 1], v[3 * ind0 + 2]);
		vec3 v1 = vec3(v[3 * ind1], v[3 * ind1 + 1], v[3 * ind1 + 2]);
		vec3 v2 = vec3(v[3 * ind2], v[3 * ind2 + 1], v[3 * ind2 + 2]);

		vec3 n0 = (cross((v1 - v0), (v2 - v0)));
		vec3 n1 = (cross((v2 - v1), (v0 - v1)));
		vec3 n2 = (cross((v0 - v2), (v1 - v2)));

		normal_map[ind0].normal = n0;
		normal_map[ind0].count++;

		normal_map[ind1].normal = n1;
		normal_map[ind1].count++;

		normal_map[ind2].normal = n2;
		normal_map[ind2].count++;
	}

	/*for (auto& v : normal_map)
	{
		v.second.normal /= v.second.count;
		v.second.normal = normalize(v.second.normal);
	}*/
	vector<vec3> vn;
	vn.resize(v.size());

	for (int i = 0; i < v.size(); ++i)
	{
		vn[i] = normal_map[i].normal;
	}

	num_trs = fs.size();
	//cout << num_trs;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(float), &vn[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof(float), &vt[0], GL_STATIC_DRAW);

	//init face index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(unsigned int), &fs[0], GL_STATIC_DRAW);

}

void init_data_2(const vector<float>& heights, int width, int height)
{

	/*for (int i = 0; i < Ds.size; ++i)
	{
	for (int j = 0; j < Ds.size; ++j)
	cout << Ds.heights[j * Ds.size + i] << "\n";
	}*/

	mvpLoc = glGetUniformLocation(program, "mvp_matrix");
	nLoc = glGetUniformLocation(program, "normal_matrix");
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glGenBuffers(1, &ibo);


	vector<float> v;

	int w = width;//Ds.size;
	int h = height;// Ds.size;

				   //float grid_size = grid_size_;//Ds.grid_size;

	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			//int ind = 3 * (j * width + i);
			//float height = (data[ind] / 255.0f) * 40;

			v.emplace_back((float)i * GRID_SIZE);
			v.emplace_back(heights[j * w + i] * HEIGHT_SCALE);
			v.emplace_back((float)j * GRID_SIZE);
		}
	}
	//cout << v.size();
	//init texture data
	vector<float> vt;
	float iwidth1 = 1.0f / (w + 1);
	float ih1 = 1.0f / (h + 1);
	for (int i = 0; i < w; ++i)
	{
		for (int j = 0; j < h; ++j)
		{
			//cout << i*iwidth1 << "\n";

			vt.emplace_back(i * iwidth1);
			vt.emplace_back(j * ih1);
		}
	}

	vector<unsigned int> fs;
	for (int i = 0; i < w - 1; ++i)
	{
		for (int j = 0; j < h - 1; ++j)
		{

			fs.emplace_back(j * w + i);
			fs.emplace_back((j + 1) * w + i);
			fs.emplace_back(j * w + i + 1);

			fs.emplace_back(j * w + i + 1);
			fs.emplace_back((j + 1) * w + i);
			fs.emplace_back((j + 1) * w + i + 1);
		}
	}

	//vector<vec3> vn;

	//vn.resize(w * h);

	unordered_map<int, Normal_Vertex> normal_map;

	for (int i = 0; i < fs.size(); i += 3)
	{
		int ind0 = fs[i];
		int ind1 = fs[i + 1];
		int ind2 = fs[i + 2];

		vec3 v0 = vec3(v[3 * ind0], v[3 * ind0 + 1], v[3 * ind0 + 2]);
		vec3 v1 = vec3(v[3 * ind1], v[3 * ind1 + 1], v[3 * ind1 + 2]);
		vec3 v2 = vec3(v[3 * ind2], v[3 * ind2 + 1], v[3 * ind2 + 2]);

		vec3 n0 = -normalize(cross((v1 - v0), (v2 - v0)));
		vec3 n1 = -normalize(cross((v2 - v1), (v0 - v1)));
		vec3 n2 = -normalize(cross((v0 - v2), (v1 - v2)));

		normal_map[ind0].normal += n0;
		normal_map[ind0].count++;

		normal_map[ind1].normal += n1;
		normal_map[ind1].count++;

		normal_map[ind2].normal += n2;
		normal_map[ind2].count++;
	}

	for (auto& v : normal_map)
		v.second.normal /= v.second.count;

	vector<vec3> vn;
	vn.resize(v.size());

	for (int i = 0; i < v.size(); ++i)
	{
		vn[i] = normal_map[i].normal;
	}

	num_trs = fs.size();
	//cout << num_trs;
	/*glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(float), &vn[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof(float), &vt[0], GL_STATIC_DRAW);
	*/

	num_trs = fs.size();
	//cout << num_trs;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(float), &vn[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof(float), &vt[0], GL_STATIC_DRAW);

	

	//init face index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fs.size() * sizeof(unsigned int), &fs[0], GL_STATIC_DRAW);

}

void Draw_Model(GLFWwindow* window, Camera& cam)
{
	cam.Compute_Matrix(window);

	mat4 mvMat = cam.vMat * cam.mMat;
	mat4 mvpMat = cam.pMat * mvMat;
	mat4 nMat = transpose(inverse(mvMat));


	//mat4 nMat = transpose(inverse(cam.vMat));
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvpMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, value_ptr(nMat));

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, num_trs, GL_UNSIGNED_INT, 0);

}



void main()
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Perlin Noise Terrain", NULL, NULL);

	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	Utility utils;
	program = utils.CreateProgram("Light.vs", "Light.fs");

	vec3 light_position(-1102.7348, 2936.7973, 638.4397);

	init_light(program, light_position);

	glUseProgram(program);

	Camera cam;

	int width = 129;
	float octaves = 8;
	int max_height = 128;

	//Diamond Square
	Diamond_Square Ds(width, max_height);
	init_data(Ds);

	//Perlin Noise
	//int roughness = 4.0f;
	//Perlin_Noise Ps(width, roughness, octaves, max_height);
	//init_data_2(Ps.heights, Ps.width, Ps.height);// , GRID_SIZE);

	//Height Map
	
	//string H_Map = "HeightMap.png";

	//string H_Map = "Terrain2.bmp";

	//Height_Map height(H_Map, max_height);
	//init_data_2(height.heights, height.width, height.height);

	//create_test_data(width, h);

	glClearColor(0.0, 0.0, 0.0, 0.95);

	//GLuint TextureID = utils.LoadTexture("map\\green.bmp");

	GLuint HeightMapID = utils.LoadTexture("map\\heightmap2.bmp");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, HeightMapID);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (cam.generate)
		{
			//Diamond square
			Ds.generate_height_map();
			init_data(Ds);

			//Ps.generate_height_map();
			//init_data_2(Ps.heights, Ps.width, Ps.height);// , GRID_SIZE);

			//height.generate_height_map(H_Map);
		}

		/*if (cam.Save)
		{
			float random_id = randf();

			int id = 1000000 * random_id;

			string name = "Terrain_" + to_string(id) + ".txt";

			ofstream ofs(name);

			cout << Ds.size << " " << Ds.size << " " << Ds.heights.size() << "\n";

			ofs << Ds.size << " " << Ds.size << "\n";

			for (int i = 0; i < Ds.size; ++i)
			{
				for (int j = 0; j < Ds.size; ++i)
				{
					int ind = (j * Ds.size + i);

					float x = (float)i * GRID_SIZE;
					//float y = Ds.heights[ind] * HEIGHT_SCALE;
					float z = (float)j * GRID_SIZE;

					ofs << x << " " << z << "\n";
				}
			}

			//for (int i = 0; i < 100000; ++i)//wait so they dont save the same image in one click
				cam.Save = false;
		}*/

		string str = "Pos: " + std::to_string(cam.p.x) + "," + std::to_string(cam.p.y) + "," + std::to_string(cam.p.z)
			+ " direction: " + to_string(cam.direction.x) + " " + to_string(cam.direction.y) + " " + to_string(cam.direction.z);

		/*px = cam.p.x;
		py = cam.p.y;
		pz = cam.p.z;

		dx = cam.d.x;
		dy = cam.d.y;
		dz = cam.d.z;*/

		glfwSetWindowTitle(window, str.c_str());

		Draw_Model(window, cam);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

}
