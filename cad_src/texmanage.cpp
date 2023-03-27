#include "hovercad.h"
string TextureList[255]; //probley should be a vector

void AddTexture(char* texname) {
    const char *filename = fl_filename_name(texname);
    if (FindTexture(texname) == -1) {
        fltexlist->add(texname);
        fllinetex->add(filename);
        flfloortex->add(filename);
        flrooftex->add(filename);

        TextureList[TextureListCount] = texname;
        cout << "added texture #" << TextureListCount << " called " << texname << endl;
        TextureListCount++;
    }

}

void DeleteTexture(int id) {
    fltexlist->remove(id);
    fllinetex->remove(id);
    flfloortex->remove(id);
    flrooftex->remove(id);
}

int FindTexture(char *tex) {
    for(int i = 0; i < TextureListCount; i++)
        if(TextureList[i].compare(tex) == 0) return i;

    return -1;
}

void SetupTextures() {
//load set up the texture list with default textures
fltexlist->add("None");
fllinetex->add("None");
flfloortex->add("None");
flrooftex->add("None");

//load default textures
AddTexture("img/floor.jpg");
AddTexture("img/Red Right.jpg");
AddTexture("img/Red Left.jpg");
AddTexture("img/Yellow Right.jpg");
AddTexture("img/Yellow Left.jpg");
AddTexture("img/Grey Right.jpg");
AddTexture("img/Grey Left.JPG");
AddTexture("img/Green Right.jpg");
AddTexture("img/Green Left.JPG");
AddTexture("img/caution.jpg");

}

void ClearTextureList() {
    fltexlist->clear();
    fllinetex->clear();
    flfloortex->clear();
    flrooftex->clear();

    for(int i = 0; i < 255; i++) TextureList[i].clear();
    TextureListCount = 0;
    SetupTextures();
}
