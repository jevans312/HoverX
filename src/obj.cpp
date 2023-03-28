#include "obj.h"

objModel::objModel( void )    {
    TextureID = 0;
    Loaded = false;
}

objModel::~objModel( void )   {
	//delete [] m_vertices;
}

void skipLine(istream& is)  {
    char next;
	is >> std::noskipws;
    while( (is >> next) && ('\n' != next) );
}

//	skip a comment line
bool skipCommentLine(istream& is)   {
	char next;
	while( is >> std::skipws >> next )
    {
		is.putback(next);
		if ('#' == next)
			skipLine(is);
		else
			return true;
    }
    return false;
}

void objModel::Clear() {
    TextureID = 0;
}

//-----objModel::Load
//this function loads a .obj file, parses it into vertex data
//then draws into a display list. I know this should be 2 or 3
//functions but this seems most efficent.
//TODO: return bool value on load
bool objModel::Load(const string& modelfile, const string& texturefile)   {
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

    //return 0 if the model is alreay loaded; should be more discriptive
    if (Loaded) {
        cout << "this object has a model loaded" << endl;
        return 0;
    }

	ifstream is(modelfile.c_str(), ios::in|ios::ate);
	if (!is) cout << "empty obj file" << endl;

	is.tellg();                //get the file size (we started at the end)...
	is.seekg (0, ios::beg);    //...then get back to start

    is >> ele_id;

    //read data in
	while(skipCommentLine(is)) {
	if (!(is >> ele_id)) cout << "theres a problem with the obj file" << endl;

	if ("mtllib" == ele_id) {
		//string strFileName;
		//is >> strFileName;
		//parseMtlLib(strFileName);
	}
	else if ("usemtl" == ele_id) {
		//string strMtlName;
		//is >> strMtlName;
		//map<string, SMaterial>::iterator it = m_materialMap.find(strMtlName);
		//if (it != m_materialMap.end())
		//	pMesh->setMaterial((*it).second);
		//else
		//	CLogger::get() << "  * Material not found in current mtllib :\"" << strMtlName << "\". Ignoring material.\n";
	}
	else if ("v" == ele_id) {	//	vertex data
		is >> x >> y >> z;

		if(VertexCount < MAXOBJ_VERTEX) {
            V[VertexCount].x = x;
            V[VertexCount].y = y;
            V[VertexCount].z = z;
            VertexCount++;
		}
		else cout << "too many vertexs in file, will not load correctly" << endl;

		is.clear();
	}
	else if ("vt" == ele_id) {	// texture data
		is >> x >> y;
		if(TextureCount < MAXOBJ_TEXTURE) {
            T[TextureCount].x = x;
            T[TextureCount].y = -y; //flip the y cord so the textues display correctly
            TextureCount++;
		}
		else cout << "too many texture cordinates, will not load correctly" << endl;
		is.clear();                           // is z (i.e. w) is not available, have to clear error flag.
	}
	else if ("vn" == ele_id) {	// normal data
		is >> x >> y >> z;
		if(!is.good()) {        // in case it is -1#IND00
			x = y = z = 0.0;
			is.clear();
			skipLine(is);
		}
		if(NormalCount < MAXOBJ_NORMAL) {
            N[NormalCount].x = x;
            N[NormalCount].y = y;
            N[NormalCount].z = z;
            //cout << "added normal " << NormalCount << " x" << N[NormalCount].x << " y " << N[NormalCount].y << " z " << N[NormalCount].z << endl;
            NormalCount++;
		}
		else cout << "too many normals, will not load correctly" << endl;
	}
	else if ("f" == ele_id) {	//	face data
	    char c;

        if(VertexCount < MAXOBJ_FACE) {
            is >> F[FaceCount].v1 >> c >> F[FaceCount].vt1 >> c >> F[FaceCount].vn1 >>
                    F[FaceCount].v2 >> c >> F[FaceCount].vt2 >> c >> F[FaceCount].vn2 >>
                    F[FaceCount].v3 >> c >> F[FaceCount].vt3 >> c >> F[FaceCount].vn3;
                    FaceCount++;
        }
        else cout << "too many polys in file, it will not load correctly" << endl;

        is.clear();
	}

	else    skipLine(is);
	} //end of while

	//close file
    is.close();

    //load texture
    TextureID = LoadGLTexture( (char*)texturefile.c_str() );

    //get the display list address and start to build it
	DisplayList = glGenLists(1);
	glNewList(DisplayList, GL_COMPILE);

    //setup materials for the mesh
    GLfloat AMB_Material[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat DIF_Material[] = {0.5, 0.5, 0.5, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, AMB_Material);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, DIF_Material);

    //bind texture to mesh
    glBindTexture(GL_TEXTURE_2D, TextureID);

    //draw triangles vertex by vertex
    glBegin(GL_TRIANGLES);
        for (uint i=1; i < FaceCount; i++) {
            //set vert 1 xyz of face i
            glNormal3f(N[F[i].vn1].x, N[F[i].vn1].y, N[F[i].vn1].z);
            glTexCoord2f(T[F[i].vt1].x, T[F[i].vt1].y);
            glVertex3f(V[F[i].v1].x, V[F[i].v1].y, V[F[i].v1].z);

            //set vert 2 xyz of face i
            glNormal3f(N[F[i].vn2].x, N[F[i].vn2].y, N[F[i].vn2].z);
            glTexCoord2f(T[F[i].vt2].x, T[F[i].vt2].y);
            glVertex3f(V[F[i].v2].x, V[F[i].v2].y, V[F[i].v2].z);

            //set vert 3 xyz of face i
            glNormal3f(N[F[i].vn3].x, N[F[i].vn3].y, N[F[i].vn3].z);
            glTexCoord2f(T[F[i].vt3].x, T[F[i].vt3].y);
            glVertex3f(V[F[i].v3].x, V[F[i].v3].y, V[F[i].v3].z);
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
}
