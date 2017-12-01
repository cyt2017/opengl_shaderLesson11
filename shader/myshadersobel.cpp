#include "myshadersobel.h"

MyShaderSobel::MyShaderSobel()
{

}
bool MyShaderSobel::initialize()
{
    const char* vs  =
    {
        "uniform    mat4    _MVP;"
        "attribute  vec3    _Position;"
        "void main()"
        "{"
            "gl_Position    =	_MVP * vec4(_Position,1.0);"
        "}"
    };
    const char* ps  =
    {
        "precision  lowp        float; "
        "uniform    sampler2D	_texture;"
        "uniform    vec2	    _size;"
        "uniform    float	    _hCoef[9];"
        "uniform    float	    _vCoef[9];"
        "void main()"
        "{"
        "   vec2 offset[9];"
        "   offset[0] = vec2(-1.0 ,-1.0);"
        "   offset[1] = vec2( 0.0 ,-1.0);"
        "   offset[2] = vec2( 0.0 ,-1.0);"
        "   offset[3] = vec2(-1.0 , 0.0);"
        "   offset[4] = vec2( 0.0 , 0.0);"
        "   offset[5] = vec2( 1.0 , 0.0);"
        "   offset[6] = vec2(-1.0 , 1.0);"
        "   offset[7] = vec2( 0.0 , 1.0);"
        "   offset[8] = vec2( 1.0 , 1.0);"
        "   vec2  tFrag = vec2(1.0 / _size.x,1.0 / _size.y);"
        "   vec2 fc = vec2(gl_FragCoord.s, _size.y - gl_FragCoord.t);"
        "   vec3 hCoord = vec3(0.0);"
        "   vec3 vCoord = vec3(0.0);"
        "hCoord += (texture2D(_texture,(fc+offset[0])*tFrag)).rgb*_hCoef[0];"
        "hCoord += (texture2D(_texture,(fc+offset[1])*tFrag)).rgb*_hCoef[1];"
        "hCoord += (texture2D(_texture,(fc+offset[2])*tFrag)).rgb*_hCoef[2];"
        "hCoord += (texture2D(_texture,(fc+offset[3])*tFrag)).rgb*_hCoef[3];"
        "hCoord += (texture2D(_texture,(fc+offset[4])*tFrag)).rgb*_hCoef[4];"
        "hCoord += (texture2D(_texture,(fc+offset[5])*tFrag)).rgb*_hCoef[5];"
        "hCoord += (texture2D(_texture,(fc+offset[6])*tFrag)).rgb*_hCoef[6];"
        "hCoord += (texture2D(_texture,(fc+offset[7])*tFrag)).rgb*_hCoef[7];"
        "hCoord += (texture2D(_texture,(fc+offset[8])*tFrag)).rgb*_hCoef[8];"
        "vCoord += (texture2D(_texture,(fc+offset[0])*tFrag)).rgb*_vCoef[0];"
        "vCoord += (texture2D(_texture,(fc+offset[1])*tFrag)).rgb*_vCoef[1];"
        "vCoord += (texture2D(_texture,(fc+offset[2])*tFrag)).rgb*_vCoef[2];"
        "vCoord += (texture2D(_texture,(fc+offset[3])*tFrag)).rgb*_vCoef[3];"
        "vCoord += (texture2D(_texture,(fc+offset[4])*tFrag)).rgb*_vCoef[4];"
        "vCoord += (texture2D(_texture,(fc+offset[5])*tFrag)).rgb*_vCoef[5];"
        "vCoord += (texture2D(_texture,(fc+offset[6])*tFrag)).rgb*_vCoef[6];"
        "vCoord += (texture2D(_texture,(fc+offset[7])*tFrag)).rgb*_vCoef[7];"
        "vCoord += (texture2D(_texture,(fc+offset[8])*tFrag)).rgb*_vCoef[8];"

        "gl_FragColor = vec4(vec3(sqrt(hCoord * hCoord + vCoord * vCoord)), 1.0);"
        "}"
    };
    bool    res =   createProgram(vs,ps);
    if(res)
    {
        _position   =   glGetAttribLocation(_programId,"_position");
        _MVP        =   glGetUniformLocation(_programId,"_MVP");
        _texture    =   glGetUniformLocation(_programId,"_texture");
        _size       =   glGetUniformLocation(_programId,"_size");
        _hCoef      =   glGetUniformLocation(_programId,"_hCoef");
        _vCoef      =   glGetUniformLocation(_programId,"_vCoef");
    }
    return  res;
}

void MyShaderSobel::begin()
{
    glUseProgram(_programId);
    //!在显卡里面使用的局部变量，在使用时是需要进行使能和关闭的
    glEnableVertexAttribArray(_position );
}

void MyShaderSobel::end()
{
    glDisableVertexAttribArray(_position );

    glUseProgram(0);
}
