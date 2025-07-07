#include <iostream>
#include <string>
#include <fstream>

#include "main.h"
#include "glex.h"
#include "obj.h"

using namespace std;

objModel::objModel( void )    {
    TextureID = 0;
    Loaded = false;
}

objModel::~objModel( void )   {
	//TODO: clean up!
}

void objModel::Clear() {
    TextureID = 0;
}

//-----objModel::Load
//this function loads a .obj file, parses it into vertex data
//then draws into a display list. I know this should be 2 or 3
//functions but this seems most efficent.
//TODO: return bool value on load
bool objModel::Load(const std::string& modelfile, const std::string& texturefile)   {
    string ele_id;
    float x, y, z;

    //all local data so we dont have to keep this in memory twice
    Vertex          V[MAXOBJ_VERTEX];
    Vertex          N[MAXOBJ_NORMAL];
    Triangle        F[MAXOBJ_FACE];
    Vertex2D        T[MAXOBJ_TEXTURE];
    unsigned int    VertexCount = 1;
    unsigned int    NormalCount = 1;
    unsigned int    FaceCount = 1;
    unsigned int    TextureCount = 1;

    if (Loaded) {
        cout << "objModel::Load: Object already has a model loaded" << '\n';
        return false;
    }

	ifstream is(modelfile.c_str(), ios::in|ios::ate);
	if (!is) {
        cout << "objModel::Load: Empty obj could not be opened!" << '\n';
        return false;
    }

	is.tellg();             //get the file size (we are at the end of file)...
	is.seekg(0, ios::beg);  //... get back to start

    //is >> ele_id;
    //read data in
	while(skipCommentLine(is)) {
        is >> ele_id;

        if(is.fail()) {
            cout << "objModel::Load->fail(): Maybe recoverable." << '\n';

            if(is.bad()) { // in case it is -1#IND00
                cout << "objModel::Load->bad() Set also! May not be recoverable." << '\n';
            }
            is.clear();

            if(is.good()) {
                cout << "objModel::Load: Cleared error. good() is set" << '\n';
            }
        }

        //TODO: Implement mtllib and usemtl
        if ("v" == ele_id) {	//	vertex data
            is >> x >> y >> z;

            if(VertexCount < MAXOBJ_VERTEX) {
                V[VertexCount].x = x;
                V[VertexCount].y = y;
                V[VertexCount].z = z;
                VertexCount++;
            }
            else cout << "objModel::Load: too many vertexs in file, will not load correctly" << '\n';
        }
        else if ("vt" == ele_id) {	// texture data
            is >> x >> y;
            if(TextureCount < MAXOBJ_TEXTURE) {
                T[TextureCount].x = x;
                T[TextureCount].y = -y; //flip the y cord so the textues display correctly
                TextureCount++;
            }
            else cout << "objModel::Load: too many texture cordinates, will not load correctly" << '\n';                        // is z (i.e. w) is not available, have to clear error flag.
        }
        else if ("vn" == ele_id) {	// normal data
            is >> x >> y >> z;
            if(NormalCount < MAXOBJ_NORMAL) {
                N[NormalCount].x = x;
                N[NormalCount].y = y;
                N[NormalCount].z = z;
                //cout << "added normal " << NormalCount << " x" << N[NormalCount].x << " y " << N[NormalCount].y << " z " << N[NormalCount].z << '\n';
                NormalCount++;
            }
            else cout << "objModel::Load: too many normals, will not load correctly" << '\n';
        }
        else if ("f" == ele_id) {	//	face data
            char c;
            if(VertexCount < MAXOBJ_FACE) {
                is >>   F[FaceCount].v1 >> c >> F[FaceCount].vt1 >> c >> F[FaceCount].vn1 >>
                        F[FaceCount].v2 >> c >> F[FaceCount].vt2 >> c >> F[FaceCount].vn2 >>
                        F[FaceCount].v3 >> c >> F[FaceCount].vt3 >> c >> F[FaceCount].vn3;
                        FaceCount++;
            }
            else cout << "objModel::Load: too many polys in file, it will not load correctly" << '\n';
        }
        else {
            cout << "objModel::Load: Property type \'" << ele_id << "\' not implemented" << '\n';
            skipLine(is);
        }
	}
    is.close();

    //load texture
    TextureID = LoadGLTexture( (char*)texturefile.c_str() );

    //get the display list address and start to build it
	DisplayList = glGenLists(1);
	glNewList(DisplayList, GL_COMPILE);

    //setup materials for the mesh
    /*
    GLfloat AMB_Material[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat DIF_Material[] = {0.5, 0.5, 0.5, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, AMB_Material);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, DIF_Material);
    */

    //bind texture to mesh
    glBindTexture(GL_TEXTURE_2D, TextureID);

    //draw triangles vertex by vertex
    glBegin(GL_TRIANGLES);
        for (uint i=1; i < FaceCount; i++) {
            glNormal3f(     N[F[i].vn1].x,  N[F[i].vn1].y,  N[F[i].vn1].z);
            glTexCoord2f(   T[F[i].vt1].x,  T[F[i].vt1].y);
            glVertex3f(     V[F[i].v1].x,   V[F[i].v1].y,   V[F[i].v1].z);

            glNormal3f(     N[F[i].vn2].x,  N[F[i].vn2].y,  N[F[i].vn2].z);
            glTexCoord2f(   T[F[i].vt2].x,  T[F[i].vt2].y);
            glVertex3f(     V[F[i].v2].x,   V[F[i].v2].y,   V[F[i].v2].z);

            glNormal3f(     N[F[i].vn3].x,  N[F[i].vn3].y,  N[F[i].vn3].z);
            glTexCoord2f(   T[F[i].vt3].x,  T[F[i].vt3].y);
            glVertex3f(     V[F[i].v3].x,   V[F[i].v3].y,   V[F[i].v3].z);
        }
    glEnd();
	glEndList();

    Loaded = true;
    return true;
}

//FIX ME: we are leaking texture data; not released in anyway after being loaded
//prob should implement a texture management system
void objModel::Unload() {
    glDeleteLists(DisplayList, 1);
    TextureID = 0;
    Loaded = false;
    Clear();
}
