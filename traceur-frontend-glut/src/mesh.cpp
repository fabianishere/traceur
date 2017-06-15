#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "mesh.h"

/************************************************************
 * SKIP THIS FILE
 ************************************************************/
//This is the code to load a mesh and a material file. 
//it is not beautiful, but works on all tested systems
//IGNORE this file you do not need to understand this code,
//nor change it.

   
using namespace std;
//dirty hack... do not do this at home... ;)
const unsigned int LINE_LEN=256;


/************************************************************
 * Normal calculations
 ************************************************************/
void Mesh::computeVertexNormals () {
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].n = Vec3Df (0.0, 0.0, 0.0);

    //Sum up neighboring normals
    for (unsigned int i = 0; i < triangles.size (); i++) {
        Vec3Df edge01 = vertices[triangles[i].v[1]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df edge02 = vertices[triangles[i].v[2]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df n = Vec3Df::crossProduct (edge01, edge02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            vertices[triangles[i].v[j]].n += n;
    }

    //Normalize
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].n.normalize ();
}


/************************************************************
 * draw
 ************************************************************/
void Mesh::drawSmooth(){

    glBegin(GL_TRIANGLES);

    for (unsigned int i=0;i<triangles.size();++i)
    {
		Vec3Df col=this->materials[triangleMaterials[i]].Kd();

		glColor3fv(col.pointer());
        for(int v = 0; v < 3 ; v++){
            glNormal3f(vertices[triangles[i].v[v]].n[0], vertices[triangles[i].v[v]].n[1], vertices[triangles[i].v[v]].n[2]);
            glVertex3f(vertices[triangles[i].v[v]].p[0], vertices[triangles[i].v[v]].p[1] , vertices[triangles[i].v[v]].p[2]);
        }

    }
    glEnd();
}

void Mesh::draw(){
    glBegin(GL_TRIANGLES);

    for (unsigned int i=0;i<triangles.size();++i)
    {
        unsigned int triMat = triangleMaterials.at(i);
        Vec3Df col=this->materials.at(triMat).Kd();
		glColor3fv(col.pointer());
        Vec3Df edge01 = vertices[triangles[i].v[1]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df edge02 = vertices[triangles[i].v[2]].p -  vertices[triangles[i].v[0]].p;
        Vec3Df n = Vec3Df::crossProduct (edge01, edge02);
        n.normalize ();
        glNormal3f(n[0], n[1], n[2]);
        for(int v = 0; v < 3 ; v++){
            glVertex3f(vertices[triangles[i].v[v]].p[0], vertices[triangles[i].v[v]].p[1] , vertices[triangles[i].v[v]].p[2]);
        }

    }
    glEnd();
}

    
    
    
bool Mesh::loadMesh(const char * filename, bool randomizeTriangulation)
{
    vertices.clear();
    triangles.clear();
	texcoords.clear();
	
    std::vector<int> vhandles;
    std::vector<int> texhandles;

    if (randomizeTriangulation)
        srand(0);

    materials.clear();
    Material defaultMat;
    defaultMat.set_Kd(0.5f,0.5f,0.5f);
    defaultMat.set_Ka(0.f,0.f,0.f);
    defaultMat.set_Ks(0.5f,0.5f,0.5f);
    defaultMat.set_Ns(96.7f);
    //defaultMat.set_Ni();
    //defaultMat.set_Tr();
    defaultMat.set_illum(2);
    defaultMat.set_name(std::string("StandardMaterialInitFromTriMesh"));
    materials.push_back(defaultMat);
    
    map<string, unsigned int> materialIndex;
    char                   s[LINE_LEN];
    float                  x, y, z;

    //we replace the \ by /
    std::string realFilename(filename);
    for (unsigned int i=0;i<realFilename.length();++i)
    {
        if (realFilename[i]=='\\')
            realFilename[i]='/';
    }

    std::vector<Vec3Df>     normals;
    std::string            matname;

    std::string path_;
    std::string temp(realFilename);
    int pos=temp.rfind("/");

    if (pos<0)
    {
    path_="";
    }
    else
    {
        path_=temp.substr(0,pos+1);
    }
    memset(&s, 0, LINE_LEN);

    FILE * in;
    in =fopen(filename,"r");

    while(in && !feof(in) && fgets(s, LINE_LEN, in))
    {     
        // comment
        if (s[0] == '#' || isspace(s[0]) || s[0]=='\0') continue;   

        // material file
        else if (strncmp(s, "mtllib ", 7)==0)
        {
            char mtlfile[128];
            char *p0 = s+6, *p1;
            while( isspace(*++p0) ); p1=p0;
            std::string t = p1;
			int i;
            for (i = 0; i < t.length(); ++i)
            {
				if (t[i] < 32 || t[i] == 255)
				{
					break; 
				}
            }
			std::string file;
			if (t.length() == i)
	    		file = path_.append(t);
			else
            file = path_.append(t.substr(0, i));
			printf("Load material file %s\n", file.c_str());
			loadMtl(file.c_str(), materialIndex);
        }
        // usemtl
        else if (strncmp(s, "usemtl ", 7)==0)
        {
            char *p0 = s+6, *p1;
            while( isspace(*++p0) ); p1=p0;
            while(!isspace(*p1)) ++p1; *p1='\0';
            matname = p0;      
            if (materialIndex.find(matname)==materialIndex.end())
            {
				printf("Warning! Material '%s' not defined in material file. Taking default!\n", matname.c_str());
                matname="";
            }
        }
        // vertex
        else if (strncmp(s, "v ", 2) == 0)
        {
            sscanf(s, "v %f %f %f", &x, &y, &z);
            vertices.push_back(Vec3Df(x,y,z));
        }


        // texture coord
        else if (strncmp(s, "vt ", 3) == 0)
        {
            //we do nothing
			Vec3Df texCoords(0,0,0);
		
			//we only support 2d tex coords
            sscanf(s, "vt %f %f", &texCoords[0], &texCoords[1]);
            texcoords.push_back(texCoords);
        }
        // normal
        else if (strncmp(s, "vn ", 3) == 0)
        {
            //are recalculated
        }
        // face
        else if (strncmp(s, "f ", 2) == 0)
        {
            int component(0), nV(0);
            bool endOfVertex(false);
            char *p0, *p1(s+2); //place behind the "f "

            vhandles.clear();
			texhandles.clear();

            while (*p1 == ' ') ++p1; // skip white-spaces

            while (p1)
            {
                p0 = p1;

                // overwrite next separator

                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && 
                       *p1 != ' ' && *p1 != '\0')
                  ++p1;
                    
                // detect end of vertex
                if (*p1 != '/') endOfVertex = true;

                // replace separator by '\0'
                if (*p1 != '\0') 
                {
                    *p1 = '\0';
                    p1++; // point to next token
                }

                // detect end of line and break
                if (*p1 == '\0' || *p1 == '\n')
                    p1 = 0;
                

                // read next vertex component
                if (*p0 != '\0')
                {
                    switch (component)
                    {
                        case 0: // vertex
                        {
                            int tmp = atoi(p0)-1;
                            vhandles.push_back(tmp);
                        }
                        break;
                      
                        case 1: // texture coord
                        {
                            int tmp = atoi(p0)-1;
                            texhandles.push_back(tmp);
                        }
                        break;
                      
                        case 2: // normal
                        //assert(!vhandles.empty());
                        //assert((unsigned int)(atoi(p0)-1) < normals.size());
                        //_bi.set_normal(vhandles.back(), normals[atoi(p0)-1]);
                        break;
                    }
                }

                ++component;

                if (endOfVertex)
                {
                    component = 0;
                    nV++;
                    endOfVertex = false;
                }
            }

			if (vhandles.size()!=texhandles.size())
				texhandles.resize(vhandles.size(),0);

            if (vhandles.size()>3)
			{
				//model is not triangulated, so let us do this on the fly...
				//to have a more uniform mesh, we add randomization
				unsigned int k=(false)?(rand()%vhandles.size()):0;
				for (unsigned int i=0;i<vhandles.size()-2;++i)
				{
                    const int v0 = (k+0)%vhandles.size();
                    const int v1 = (k+i+1)%vhandles.size();
                    const int v2 = (k+i+2)%vhandles.size();

                    const int t0 = (k+0)%vhandles.size();
                    const int t1 = (k+i+1)%vhandles.size();
                    const int t2 = (k+i+2)%vhandles.size();

                    const int m  = (materialIndex.find(matname))->second;

					triangles.push_back(
                        Triangle(	vhandles[v0], texhandles[t0],
                                    vhandles[v1], texhandles[t1],
                                    vhandles[v2], texhandles[t2]));
                    triangleMaterials.push_back(m);
				}
			}
			else if (vhandles.size()==3)
			{
				triangles.push_back(Triangle(vhandles[0], texhandles[0], vhandles[1], texhandles[1], vhandles[2], texhandles[2]));
				triangleMaterials.push_back((materialIndex.find(matname))->second);
			}
			else
			{
				printf("TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face");
			}            
        }
        memset(&s, 0, LINE_LEN);
    }
	fclose(in);
    return true;
}


bool Mesh::loadMtl(const char * filename, std::map<string, unsigned int> & materialIndex)
{
    FILE * _in;
    _in = fopen(filename, "r" );
    if ( !_in )
    {
       printf("  Warning! Material file '%s' not found!\n", filename);
        return false;
    }            
    
    char   line[LINE_LEN];
    std::string textureName;

    std::string key;
    Material    mat;
    float       f1,f2,f3;
    bool        indef = false;

    memset(line,0,LINE_LEN);
    while( _in && !feof(_in) )
    {
		fgets(line, LINE_LEN, _in);

        if (line[0] == '#') // skip comments
        {
            memset(line,0,LINE_LEN);
            continue;
        }

        else if( isspace(line[0])||line[0]=='\0')
        {
            if (indef && !key.empty() && mat.is_valid())
            {
                if (materialIndex.find(key)==materialIndex.end())
                {
                    mat.set_name(key);
                    materials.push_back(mat);
                    materialIndex[key]=materials.size()-1;
                }
                mat.cleanup();
            }
			if (line[0]=='\0')
				break;
        }
        else if (strncmp(line, "newmtl ", 7)==0) // begin new material definition
        {
            char *p0 = line+6, *p1;
            while( isspace(*++p0) ); p1=p0;
            while(!isspace(*p1)) ++p1; *p1='\0';
            key   = p0;
            indef = true;
        }
        else if (strncmp(line, "Kd ", 3)==0) // diffuse color
        {
            sscanf(line, "Kd %f %f %f", &f1, &f2, &f3);
            mat.set_Kd(f1,f2,f3);
        }
        else if (strncmp(line, "Ka ", 3)==0) // ambient color
        {
            sscanf(line, "Ka %f %f %f", &f1, &f2, &f3);
            mat.set_Ka(f1,f2,f3);
        }
        else if (strncmp(line, "Ks ", 3)==0) // specular color
        {
            sscanf(line, "Ks %f %f %f", &f1, &f2, &f3);
            mat.set_Ks(f1,f2,f3);
        }
        else if (strncmp(line, "Ns ", 3)==0) // Shininess [0..200]
        {
            sscanf(line, "Ns %f", &f1);
            mat.set_Ns(f1);
        }
        else if (strncmp(line, "Ni ", 3)==0) // Shininess [0..200]
        {
            sscanf(line, "Ni %f", &f1);
            mat.set_Ni(f1);
        }
        else if (strncmp(line, "illum ", 6)==0) // diffuse/specular shading model
        {
            int illum=-1;
            sscanf(line, "illum %i", &illum);
            mat.set_illum(illum);
        }
        else if (strncmp(line, "map_Kd ",7)==0) // map images
        {

			std::string t=&(line[7]);
			if (!t.empty() && t[t.length()-1] == '\n') {
				t.erase(t.length()-1);
			}

          // map_Kd, diffuse map
          // map_Ks, specular map
          // map_Ka, ambient map
          // map_Bump, bump map
          // map_d,  opacity map
          // just skip this
			mat.set_textureName(t);
			
        }
        else if (strncmp(line, "Tr ", 3)==0 ) // transparency value
        {
            sscanf(line, "Tr %f", &f1);
            mat.set_Tr(f1);
        }
        else if (strncmp(line, "d ", 2)==0 ) // transparency value
        {
            sscanf(line, "d %f", &f1);
            mat.set_Tr(f1);
        }

        if (feof( _in ) && indef && mat.is_valid() && !key.empty())
        {
            if (materialIndex.find(key)==materialIndex.end())
            {
                mat.set_name(key);
                materials.push_back(mat);
                materialIndex[key]=materials.size()-1;
            }
        }
        memset(line,0,LINE_LEN);
    }
    unsigned int msize = materials.size();
    printf("%u  materials loaded.\n", msize);
    fclose( _in );
    return true;
}
