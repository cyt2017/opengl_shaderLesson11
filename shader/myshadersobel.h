#ifndef MYSHADERSOBEL_H
#define MYSHADERSOBEL_H

#include "tool/programid.h"
class MyShaderSobel : public ProgramId
{
public:
    typedef int attribute;
    typedef int uniform;
public:
    uniform     _MVP;
    uniform     _texture;
    uniform     _hCoef;
    uniform     _vCoef;
    uniform     _size;
    attribute   _position;
public:
    MyShaderSobel();

    virtual bool    initialize();
    virtual void    begin();
    virtual void    end();
};

#endif // MYSHADERSOBEL_H
