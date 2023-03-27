#ifndef vector2d_h
#define vector2d_h

class vector2d
{
public:
    float x = 0;
    float y = 0;
    float z = 0;

    // A default constructor
    vector2d() {
        x = 0;
        y = 0;
        z = 0;
    }

    // This is our constructor that allows us to initialize our data upon creating an instance
    vector2d(float X, float Y) {
        x = X;
        y = Y;
        z = 0;
    }

    vector2d operator + (vector2d& tvec) {
        //vector2d temp;
        //temp.x = x + tvec.x;
        //temp.y = y + tvec.y;
        //return (temp);

        return(vector2d(x+tvec.x, y+tvec.y));
    }

    vector2d operator - (vector2d& tvec) {
        //vector2d temp;
        //temp.x = x-tvec.x;
        //temp.y = y-tvec.y;
        //return temp;
       return(vector2d(x-tvec.x, y-tvec.y));
    }

    vector2d operator * (vector2d& tvec) {
        //vector2d temp;
        //temp.x = x*tvec.x;
        //temp.y = y*tvec.y;
        //return temp;
        return(vector2d(x*tvec.x, y*tvec.y));
    }

    vector2d operator / (vector2d& tvec) {
        //vector2d temp;
        //temp.x = x/tvec.x;
        //temp.y = y/tvec.y;
        //return temp;
        return(vector2d(x/tvec.x, y/tvec.y));
    }

    void operator += (vector2d& tvec) {
        x = x + tvec.x;
        y = y + tvec.y;
    }

    void operator -= (vector2d& tvec) {
        x = x - tvec.x;
        y = y - tvec.y;
    }
    /*
    void operator = (vector2d& tvec) {
        x = tvec.x;
        y = tvec.y;
    }

    void operator += (vector2d tvec)
    {
        x+=tvec.x;
        y+=tvec.y;
    }
    void operator -= (vector2d tvec)
    {
        x-=tvec.x;
        y-=tvec.y;
    }
    */
    vector2d& scalefr(float scalar) {
        //vector2d temp;
        x = x*scalar;
        y = y*scalar;
        //return temp;
        return *this;
        //return vector2d((x*scalar),  (y*scalar));
    }

    void scalef(float scalar) {
        x*=scalar;
        y*=scalar;
    }

    vector2d& divfr(float scalar) {
        //vector2d temp;
        x = x/scalar;
        y = y/scalar;
        //return temp;
        return *this;
        //return vector2d((x/scalar), (y/scalar));
    }

    void divf(float scalar) {
        x/=scalar;
        y/=scalar;
    }

    vector2d getperp() {
        //vector2d temp;
        //temp.x = -y;
        //temp.y = x;
        //return temp;
        return vector2d(-y,  x);
    }

    float magsqr() {
        return x*x+y*y;
    }

    void setpos(float tx, float ty) {
        x = tx;
        y = ty;
    }

};

#endif
