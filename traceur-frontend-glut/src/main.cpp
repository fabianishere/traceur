#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "raytracing.h"
#include "mesh.h"
#include "traqueboule.h"

Vec3Df MyCameraPosition;

std::vector<Vec3Df> MyLightPositions;

//image class just dumped to hide...
class RGBValue
{
	public:
	RGBValue(float rI=0, float gI=0, float bI=0)
	: r(rI)
	, g(gI)
	, b(bI)
	{
	};
	
	float operator[](int i) const
	{
		switch(i)
		{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			default: 
				return r;
		}
	}
	float & operator[](int i)
	{
		switch(i)
		{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			default: 
				return r;
		}
	}
	float r, b,g;
};





class Image
{
	public:
	Image(int width, int height)
	: _width(width)
	, _height(height)
	{
		_image.resize(3*_width*_height);
	}
	void setPixel(int i, int j, const RGBValue & rgb)
	{
		_image[3*(_width*j+i)]=rgb[0];
		_image[3*(_width*j+i)+1]=rgb[1];
		_image[3*(_width*j+i)+2]=rgb[2];
		
	}
	std::vector<float> _image;
	int _width;
	int _height;

	bool writeImage(const char * filename);	
};

bool Image::writeImage(const char * filename)
{
	FILE* file;
    file = fopen(filename, "wb");
	if (!file)
	{
		printf("dump file problem... file\n");
		return false;
	}

	fprintf(file, "P6\n%i %i\n255\n",_width, _height);

	
	std::vector<unsigned char> imageC(_image.size());
	
	for (unsigned int i=0; i<_image.size();++i)
		imageC[i]=(unsigned char)(_image[i]*255.0f);
	
	int t = fwrite(&(imageC[0]), _width * _height * 3, 1, file);
	if (t!=1)
	{
		printf("Dump file problem... fwrite\n");
		return false;
	}

	fclose(file);
	return true;
}










Mesh MyMesh; //Main mesh



// Utilisé pour essayer différents types de rendu
// Utilisé via le paramètre "-t" en ligne de commande
enum { TRIANGLE=0, MODEL=1, };
unsigned int type = MODEL;

unsigned int WindowSize_X = 800;  // largeur fenetre
unsigned int WindowSize_Y = 800;  // hauteur fenetre

unsigned int RayTracingResolutionX = 800;  // largeur fenetre
unsigned int RayTracingResolutionY = 800;  // largeur fenetre



void dessinerRepere(float length)
{
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(length,0,0);

		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,length,0);

		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,length);
	glEnd();
	glEnable(GL_LIGHTING);

}

/**
 * Appel des différentes fonctions de dessin
*/
void dessiner( )
{
	switch( type )
	{
	case TRIANGLE:
		glutSolidSphere(1,10,10);
		dessinerRepere(1);
		break;
	case MODEL:
		{
			MyMesh.draw();
			//glBegin(GL_TRIANGLES);

			//for (unsigned int i=0;i<MyMesh.triangles.size();++i)
			//{
			//	glColor3f(MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[0], MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[1], MyMesh.materials[MyMesh.triangleMaterials[i]].Kd()[2]);
			//	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[0]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[0]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[0]].p[2]);
			//	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[1]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[1]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[1]].p[2]);
			//	glVertex3f(MyMesh.vertices[MyMesh.triangles[i].v[2]].p[0], MyMesh.vertices[MyMesh.triangles[i].v[2]].p[1], MyMesh.vertices[MyMesh.triangles[i].v[2]].p[2]);
			//}
			//glEnd();
		}
	default:
		dessinerRepere(1); // Par défaut
		break;
	}
	
	yourDebugDraw();
}

void animate()
{
	MyCameraPosition=getCameraPosition();
	glutPostRedisplay();
}



void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

/**
 * Programme principal
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(WindowSize_X,WindowSize_Y);
    glutCreateWindow(argv[0]);	

    // Initialisation du point de vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-4);
    tbInitTransform();     // initialisation du point de vue
    tbHelp();                      // affiche l'aide sur la traqueboule
	MyCameraPosition=getCameraPosition();
    //
    // Active la lumière
    // Pour la partie
    // ECLAIRAGE
         
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable(GL_COLOR_MATERIAL);
    int LightPos[4] = {0,0,3,1};
    int MatSpec [4] = {1,1,1,1};
    glLightiv(GL_LIGHT0,GL_POSITION,LightPos);
    //glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
    //glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);
    
	glEnable(GL_NORMALIZE);
    glClearColor (0.0, 0.0, 0.0, 0.0);

	// Details sur le mode de tracé
    glEnable( GL_DEPTH_TEST );            // effectuer le test de profondeur
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT,GL_FILL);
    glPolygonMode(GL_BACK,GL_LINE);
    glShadeModel(GL_SMOOTH);


	// cablage des callback
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMouseFunc(tbMouseFunc);    // traqueboule utilise la souris
    glutMotionFunc(tbMotionFunc);  // traqueboule utilise la souris
    glutIdleFunc( animate);


	init();


    // lancement de la boucle principale
    glutMainLoop();
        
    return 0;  // instruction jamais exécutée
}

/**
 * Fonctions de gestion opengl à ne pas toucher
 */
// Actions d'affichage
// Ne pas changer
void display(void)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Effacer tout
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT); // la couleur et le z
    
    glLoadIdentity();  // repere camera

    tbVisuTransform(); // origine et orientation de la scene

    dessiner( );    

    glutSwapBuffers();
	glPopAttrib();
}
// pour changement de taille ou desiconification
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective (50, (float)w/h, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}


//transformer le x, y en position 3D
void produceRay(int x_I, int y_I, Vec3Df * origin, Vec3Df * dest)
{
		int viewport[4];
		double modelview[16];
		double projection[16];
		//point sur near plane 
		//double positionN[3];
		//point sur far plane 
		//double positionF[3];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //recuperer matrices
		glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
		glGetIntegerv(GL_VIEWPORT, viewport);//viewport
		int y_new = viewport[3] - y_I;

		double x, y, z;
		
		gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
		origin->p[0]=float(x);
		origin->p[1]=float(y);
		origin->p[2]=float(z);
		gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
		dest->p[0]=float(x);
		dest->p[1]=float(y);
		dest->p[2]=float(z);
}

void produceRay(int x_I, int y_I, Vec3Df & origin, Vec3Df & dest)
{
	produceRay(x_I, y_I, &origin, &dest);
}

// prise en compte du clavier
void keyboard(unsigned char key, int x, int y)
{
    printf("key %d pressed at %d,%d\n",key,x,y);
    fflush(stdout);
    switch (key)
    {
	case 'L':
		MyLightPositions.push_back(getCameraPosition());
		break;
	case 'l':
		MyLightPositions[MyLightPositions.size()-1]=getCameraPosition();
		break;
	case 'r':
	{
		//C'est nouveau!!!
		//commencez ici et lancez vos propres fonctions par rayon.

		cout<<"Raytracing"<<endl;
				
		Image result(WindowSize_X,WindowSize_Y);
		Vec3Df origin00, dest00;
		Vec3Df origin01, dest01;
		Vec3Df origin10, dest10;
		Vec3Df origin11, dest11;
		Vec3Df origin, dest;


		produceRay(0,0, &origin00, &dest00);
		produceRay(0,WindowSize_Y-1, &origin01, &dest01);
		produceRay(WindowSize_X-1,0, &origin10, &dest10);
		produceRay(WindowSize_X-1,WindowSize_Y-1, &origin11, &dest11);

		for (unsigned int y=0; y<WindowSize_Y;++y)
			for (unsigned int x=0; x<WindowSize_X;++x)
			{
				//svp, decidez vous memes quels parametres vous allez passer à la fonction
				//e.g., maillage, triangles, sphères etc.
				float xscale=1.0f-float(x)/(WindowSize_X-1);
				float yscale=float(y)/(WindowSize_Y-1);

				origin=yscale*(xscale*origin00+(1-xscale)*origin10)+
					(1-yscale)*(xscale*origin01+(1-xscale)*origin11);
				dest=yscale*(xscale*dest00+(1-xscale)*dest10)+
					(1-yscale)*(xscale*dest01+(1-xscale)*dest11);

		
				Vec3Df rgb = performRayTracing(origin, dest);
				result.setPixel(x,y, RGBValue(rgb[0], rgb[1], rgb[2]));
			}
		

		result.writeImage("result.ppm");
		break;
	}
	case 27:     // touche ESC
        exit(0);
    }

	yourKeyboardFunc(key,x,y);
}

