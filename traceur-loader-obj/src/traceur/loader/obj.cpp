/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Traceur authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <memory>
#include <errno.h>
#include <filesystem/path.h>

#include <glm/glm.hpp>

#include <traceur/loader/obj.hpp>
#include <traceur/core/scene/primitive/triangle.hpp>
#include <traceur/core/scene/graph/vector.hpp>

// XXX Assume line has maximum lenght of 256 bytes (hack)
constexpr unsigned int LINE_LEN = 256;

std::unique_ptr<traceur::Scene> traceur::ObjLoader::load(const std::string
														 &file) const
{
	auto builder = factory->create();
	auto path = filesystem::path(file);

	std::shared_ptr<traceur::Material> defaultMat = std::make_shared<traceur::Material>(
			glm::vec3(0.f, 0.f, 0.f),   /* Ambient */
			glm::vec3(0.5f, 0.5f, 0.5f),/* Diffuse */
			glm::vec3(0.5f, 0.5f, 0.5f),/* Specular */
			96.7f                       /* Shininess */
	);

	std::map<std::string, std::shared_ptr<traceur::Material>> materials {
			{"$default$", defaultMat}
	};
	float x, y, z;

	std::vector<glm::vec3> vertices;
	std::string matname;

	char s[LINE_LEN] = {0};
	FILE *in = fopen(file.c_str(), "r");

	std::vector<int> vhandles;
	std::vector<int> texhandles;

	while(in && !feof(in) && fgets(s, LINE_LEN, in))
	{
		// comment
		if (s[0] == '#' || isspace(s[0]) || s[0] == '\0') 
			continue;

		// material file
		else if (strncmp(s, "mtllib ", 7) == 0) {
			char *p0 = s+6, *p1;
			while( isspace(*++p0) ); p1=p0;
			std::string t = p1;
			int i;
			for (i = 0; i < t.length(); ++i) {
				if (t[i] < 32 || t[i] == 255) {
					break; 
				}
			}
			auto matPath = filesystem::path(t.length() == i ? t : t.substr(0, i));
			loadMaterials((path.parent_path()/matPath).str(), materials);
		}
		// usemtl
		else if (strncmp(s, "usemtl ", 7) == 0) {
			char *p0 = s+6, *p1;
			while( isspace(*++p0) ); p1=p0;
			while(!isspace(*p1)) ++p1; *p1='\0';
			matname = p0;
			if (!materials.count(matname))
			{
				fprintf(stdout, "warning: material '%s' not defined in material file. Taking default!\n", matname.c_str());
				matname = "$default$";
			}
		}
		// vertex
		else if (strncmp(s, "v ", 2) == 0) {
			sscanf(s, "v %f %f %f", &x, &y, &z);
			vertices.push_back(glm::vec3(x, y, z));
		}


		// texture coord
		else if (strncmp(s, "vt ", 3) == 0) {
			// TODO add support for texture coordinates
			//Vec3Df texCoords(0,0,0);
		
			//we only support 2d tex coords
			////sscanf(s, "vt %f %f", &texCoords[0], &texCoords[1]);
			////texcoords.push_back(texCoords);
		}
		// normal
		else if (strncmp(s, "vn ", 3) == 0) {
			//are recalculated
		}
		// face
		else if (strncmp(s, "f ", 2) == 0) {
			int component(0), nV(0);
			bool endOfVertex(false);
			char *p0, *p1(s+2); //place behind the "f "

			vhandles.clear();
			texhandles.clear();

			while (*p1 == ' ') ++p1; // skip white-spaces

			while (p1) {
				p0 = p1;

				// overwrite next separator
				// skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
				while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && 
					*p1 != ' ' && *p1 != '\0')
				++p1;

				// detect end of vertex
				if (*p1 != '/') 
					endOfVertex = true;

				// replace separator by '\0'
				if (*p1 != '\0') {
					*p1 = '\0';
					p1++; // point to next token
				}

				// detect end of line and break
				if (*p1 == '\0' || *p1 == '\n')
					p1 = 0;

				// read next vertex component
				if (*p0 != '\0') {
					switch (component) {
					// vertex
					case 0: {
						int tmp = atoi(p0)-1;
						vhandles.push_back(tmp);
						break;
					}
					// texture coord
					case 1: {
						int tmp = atoi(p0)-1;
						texhandles.push_back(tmp);
						break;
					}
					// normal
					case 2:
						// calculated by the program itself
						break;
					}
				}

				++component;

				if (endOfVertex) {
					component = 0;
					nV++;
					endOfVertex = false;
				}
			}

			if (vhandles.size() != texhandles.size())
				texhandles.resize(vhandles.size(), 0);

			if (vhandles.size() > 3) {
				// Model is not triangulated, so let us do this on the fly...
				// to have a more uniform mesh, we add randomization
				unsigned int k = 0;
				for (int i = 0; i < vhandles.size() - 2; ++i) {
					const int v0 = (k + 0) % vhandles.size();
					const int v1 = (k + i + 1) % vhandles.size();
					const int v2 = (k + i + 2) % vhandles.size();

					const auto &m = materials[matname];
					auto o = vertices[vhandles[v0]];
					auto u = vertices[vhandles[v1]] - o;
					auto v = vertices[vhandles[v2]] - o;
					builder->add(traceur::Triangle(o, u, v, m));
				}
			}
			else if (vhandles.size() == 3) {
				auto o = vertices[vhandles[0]];
				auto u = vertices[vhandles[1]] - o;
				auto v = vertices[vhandles[2]] - o;

				const auto &m  = materials[matname];
				builder->add(traceur::Triangle(o, u, v, m));
			}
			else {
				fprintf(stdout, "warning: unexpected number of face vertices (<3). Ignoring face");
			}
		}
		memset(&s, 0, LINE_LEN);
	}
	fclose(in);
	return std::make_unique<traceur::Scene>(builder->build());
}

bool traceur::ObjLoader::loadMaterials(const std::string &path, std::map<std::string, std::shared_ptr<traceur::Material>> &materials) const
{
	FILE * in = fopen(path.c_str(), "r");
	if (!in) {
		fprintf(stdout, "warning: material file '%s' not found (%s)\n", path.c_str(), strerror(errno));
		return false;
	}

	char line[LINE_LEN];
	traceur::Material mat;
	std::string key;
	float f1, f2, f3;
	bool indef = false;

	memset(line, 0, LINE_LEN);
	while(in && !feof(in)) {
		fgets(line, LINE_LEN, in);

		// Skip comments
		if (line[0] == '#') {
			memset(line, 0, LINE_LEN);
			continue;
		}
		else if (isspace(line[0]) || line[0] == '\0') {
			if (indef && !key.empty()) {
				if (!materials.count(key)) {
					materials[key] = std::make_shared<traceur::Material>(mat);
				}
			}
			if (line[0] == '\0')
				break;
		}
		// begin new material definition
		else if (strncmp(line, "newmtl ", 7) == 0) {
			char *p0 = line+6, *p1;
			while( isspace(*++p0) ); p1=p0;
			while(!isspace(*p1)) ++p1; *p1='\0';
			key   = p0;
			indef = true;
		}
		// Diffuse factor
		else if (strncmp(line, "Kd ", 3) == 0) {
			sscanf(line, "Kd %f %f %f", &f1, &f2, &f3);
			mat.diffuse = glm::vec3(f1, f2, f3);
		}
		// Ambient color
		else if (strncmp(line, "Ka ", 3) == 0) {
			sscanf(line, "Ka %f %f %f", &f1, &f2, &f3);
			mat.ambient = glm::vec3(f1, f2, f3);
		}
		// Specular color
		else if (strncmp(line, "Ks ", 3) == 0) {
			sscanf(line, "Ks %f %f %f", &f1, &f2, &f3);
			mat.specular = glm::vec3(f1, f2, f3);
		}
		// Shininess [0..200]
		else if (strncmp(line, "Ns ", 3) == 0) {
			sscanf(line, "Ns %f", &f1);
			mat.shininess = f1;
		}
		else if (strncmp(line, "Ni ", 3) == 0) {
			sscanf(line, "Ni %f", &f1);
			//mat.ni = f1;
		}
		// Diffuse/specular shading model
		else if (strncmp(line, "illum ", 6)==0) {
			int illum = -1;
			sscanf(line, "illum %i", &illum);
			//mat.illum = illum;
		}
		// map images
		else if (strncmp(line, "map_Kd ",7) == 0) {

			std::string t= &(line[7]);
			if (!t.empty() && t[t.length()-1] == '\n') {
				t.erase(t.length()-1);
			}

			// map_Kd, diffuse map
			// map_Ks, specular map
			// map_Ka, ambient map
			// map_Bump, bump map
			// map_d,  opacity map
			// just skip this
			// mat.textureName = t;	
		}
		// transparency value
		else if (strncmp(line, "Tr ", 3) == 0) {
			sscanf(line, "Tr %f", &f1);
			//mat.transparency = f1;
		}
		// transparency value
		else if (strncmp(line, "d ", 2) == 0 ) {
			sscanf(line, "d %f", &f1);
			// mat.transparency = f2;
		}

		if (feof(in) && indef && !key.empty()) {
			if (!materials.count(key)) {
				materials[key] = std::make_shared<traceur::Material>(mat);
			}
		}
		memset(line, 0, LINE_LEN);
	}
	fclose(in);
	return true;
}

