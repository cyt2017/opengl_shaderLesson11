#include "qgleswidget.h"

QGLESWIDGET::QGLESWIDGET(QWidget *parent) : QWidget(parent)
{
    leftB = false;
    rightB = false;
    typeMouse = MOUSE_NULL;
}

QGLESWIDGET::~QGLESWIDGET()
{
    destroyOpenGLES20();
}

bool QGLESWIDGET::init_QGW(std::vector<QString> fileName)
{
    if(!initOpenGLES20())
    {
        return false;
    }
    _shader.initialize();
    _shaderCart.initialize();
    _shaderSobel.initialize();

    glEnable(GL_DEPTH_TEST);

    //!初始化摄像机的数据
    CELL::float3 eye   =   CELL::real3(0.0f,200.0f,0.0f);
    CELL::float3 look  =   CELL::real3(0.0f,1.0f,0.0f);
    CELL::float3 right =   CELL::float3(1.0f,0.0f,0.0f);
    float moveSpeed    =   5.0f;

    _camera.initMycamera(eye,look,right,moveSpeed);
    _camera.update();

    _box.initDataBox(CELL::float3(10.0f,10.0f,10.0f));
    _torus.createTorus(_torus._torus,_torus._pris);

    QString name = "/home/cty/workspace/OPENGL/OpenGL_Shader/opengl_shaderLesson11/build/terrainMask.png";
    _texture = loadTexture(name.toLatin1().data());

    connect(this,SIGNAL(sendKeyEvent(KEYMODE)),&_camera,SLOT(reciveKeyEvent(KEYMODE)));
    connect(this,SIGNAL(sendMouseEvent(MOUSEMODE,CELL::int2,CELL::int2)),&_camera,SLOT(reciveMouseEvent(MOUSEMODE,CELL::int2,CELL::int2)));
    connect(this,SIGNAL(sendWheelEvent(MOUSEMODE,int,CELL::int2)),&_camera,SLOT(reciveWheelEvent(MOUSEMODE,int,CELL::int2)));
}

bool QGLESWIDGET::initOpenGLES20()
{
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE,24,
        EGL_NONE
    };
    EGLint 	format(0);
    EGLint	numConfigs(0);
    EGLint  major;
    EGLint  minor;

    //! 1
    _display	    =	eglGetDisplay(EGL_DEFAULT_DISPLAY);

    //! 2init
    eglInitialize(_display, &major, &minor);

    //! 3
    eglChooseConfig(_display, attribs, &_config, 1, &numConfigs);

    eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
    //!!! 4 使opengl与qt的窗口进行绑定<this->winId()>
    _surface	    = 	eglCreateWindowSurface(_display, _config, this->winId(), NULL);

    //! 5
    EGLint attr[]   =   { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    _context 	    = 	eglCreateContext(_display, _config, 0, attr);
    //! 6
    if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
    {
        return false;
    }

    eglQuerySurface(_display, _surface, EGL_WIDTH,  &_width);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &_height);

    return  true;
}

void QGLESWIDGET::destroyOpenGLES20()
{
    if (_display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (_context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(_display, _context);
        }
        if (_surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(_display, _surface);
        }
        eglTerminate(_display);
    }
    _display    =   EGL_NO_DISPLAY;
    _context    =   EGL_NO_CONTEXT;
    _surface    =   EGL_NO_SURFACE;//asdsafsaf
}

void QGLESWIDGET::render()
{
    if(_fbo._texFbo == 0)
    {
        _fbo.setFBO(_width,_height);
    }
    _fbo.begin();
    //! 清空缓冲区
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //! 视口，在Windows窗口指定的位置和大小上绘制OpenGL内容
    glViewport(0,0,_width,_height);

    _camera.setViewSize(CELL::real2(_width,_height));
    _camera.perspective(45, (float)_width /(float)_height, 0.1, 100000);

    float   gSizeX = 500;
    float   gSizeY = 500;
    float   gPos = 0;
    float   rept = 1;

    Vertex grounds[] =
    {
        { -gSizeX, gPos,-gSizeY, 0,-1,0, 0.0f, 0.0f},
        {  gSizeX, gPos,-gSizeY, 0,-1,0, rept, 0.0f},
        {  gSizeX, gPos, gSizeY, 0,-1,0, rept, rept},

        { -gSizeX, gPos,-gSizeY, 0,-1,0, 0.0f, 0.0f},
        {  gSizeX, gPos, gSizeY, 0,-1,0, rept, rept},
        { -gSizeX, gPos, gSizeY, 0,-1,0, 0.0f, rept},
    };

    glEnable(GL_DEPTH_TEST);

    CELL::matrix4   matMV       =   _camera.getView();
    CELL::matrix4   matPRJ      =   _camera.getProject();
    CELL::matrix4   matMVP      =   matPRJ * matMV ;

    _shader.begin();
    {
        glUniformMatrix4fv(_shader._MVP, 1, GL_FALSE, matMVP.data());

        glUniform3f(_shader._lightDir, _camera._dir.x, _camera._dir.y, _camera._dir.z);
        glUniform3f(_shader._colorLight,0.0f,0.0f,1.0f);
        glUniform3f(_shader._colorOffset,0.1f,0.1f,0.1f);

        glVertexAttribPointer(_shader._position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), grounds);
        glVertexAttribPointer(_shader._normal,3,GL_FLOAT,false,sizeof(Vertex),&grounds[0].nx);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        static float angle = 0;
        angle += 0.1f;

        CELL::matrix4   matRot;
        matRot.rotateYXZ(angle,angle,angle);
        CELL::matrix4   mvp = matMVP*matRot;
        glUniformMatrix4fv(_shader._MVP,1,false,mvp.data());
        glUniform3f(_shader._colorLight,  1.0f,1.0f,1.0f);

        glVertexAttribPointer(_shader._position, 3, GL_FLOAT, GL_FALSE, sizeof(QDataBoxOpengl::V3N3UV2), _box._dataNor);
        glVertexAttribPointer(_shader._normal, 3, GL_FLOAT, GL_FALSE, sizeof(QDataBoxOpengl::V3N3UV2), &_box._dataNor[0].nx);

        glDrawArrays(GL_TRIANGLES, 0, 36);

    }
    _shader.end();

    _shaderCart.begin();
    {
        QDataTorus::Vertex toru = _torus._torus[0];


        glUniformMatrix4fv(_shaderCart._MVP, 1, GL_FALSE, matMVP.data());

        glUniform3f(_shaderCart._lightDir, _camera._dir.x, _camera._dir.y, _camera._dir.z);
        glUniform4f(_shaderCart._color, 0, 0, 1, 1);

        glVertexAttribPointer(_shaderCart._position, 3, GL_FLOAT, GL_FALSE, sizeof(QDataTorus::Vertex), &toru.x);
        glVertexAttribPointer(_shaderCart._normal, 3, GL_FLOAT, GL_FALSE, sizeof(QDataTorus::Vertex), &toru.nx);

        for (size_t i = 0; i < _torus._pris.size(); ++i)
        {
            QDataTorus::Primative pris = _torus._pris[i];
            glDrawArrays(pris.pri,pris.start, pris.count);
        }
    }
    _shaderCart.end();

    _fbo.end();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //! 视口，在Windows窗口指定的位置和大小上绘制OpenGL内容
    glViewport(0,0,_width,_height);

    CELL::matrix4   mat2D = CELL::ortho<float>(0, _width, _height, 0, -100, 100);

    struct Vertex2D
    {
        float x,y,z;
        float u,v;
    };

    Vertex2D  quad[] =
    {
        { 0,   0, 0,  0,  1},
        { _width,  0, 0,  1,  1},
        { _width,  _height,0,  1,  0},
        { 0,   _height,0,  0,  0},
    };
    float hCoef[] = {
            1.0f, 0.0f, -1.0f,
            2.0f, 0.0f, -2.0f,
            1.0f, 0.0f, -1.0f
    };
    float vCoef[] =  {
        1.0f, 2.0f, 1.0f,
        0.0f, 0.0f, 0.0f,
        -1.0f, -2.0f, -1.0f
    };
//    glBindTexture(GL_TEXTURE_2D, _fbo._texFbo);
    glBindTexture(GL_TEXTURE_2D,_texture);
    glEnable(GL_TEXTURE_2D);
    _shaderSobel.begin();
    {
        glUniformMatrix4fv(_shaderSobel._MVP, 1, 0, mat2D.data());
        glUniform1i(_shaderSobel._texture, 0);
        glUniform2f(_shaderSobel._size, _width,_height);
        glUniform1fv(_shaderSobel._hCoef, 9,hCoef);
        glUniform1fv(_shaderSobel._vCoef, 9,vCoef);
        glEnableVertexAttribArray(_shaderSobel._position);
        glVertexAttribPointer(_shaderSobel._position,   3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), quad);

        glDrawArrays(GL_TRIANGLES, 0, 4);

    }
    _shaderSobel.end();
}

void QGLESWIDGET::drawImage()
{
    render();
    eglSwapBuffers(_display,_surface);
}

bool QGLESWIDGET::eventFilter(QObject *target, QEvent *event)
{
    if( target == parent )
     {
         if( event->type() == QEvent::KeyPress )
         {
             QKeyEvent *ke = (QKeyEvent *) event;
             keyPressEvent(ke);
          }
         if( event->type() == QEvent::MouseTrackingChange )
         {
             QMouseEvent *mouse = (QMouseEvent *) event;
             mouseMoveEvent(mouse);
             mousePressEvent(mouse);
             mouseReleaseEvent(mouse);
             QWheelEvent *wheel = (QWheelEvent *) event;
             wheelEvent(wheel);
         }
     }
    return true;
}

void QGLESWIDGET::keyPressEvent(QKeyEvent *e)
{
//    printf("keyPressEvent\n");
//    fflush(NULL);
    KEYMODE type = KEY_NULL;
    if(e->key()==Qt::Key_A)
    {

        type = KEY_A;
    }
    else if(e->key()==Qt::Key_S)
    {
        type = KEY_S;
    }
    else if(e->key()==Qt::Key_D)
    {
        type = KEY_D;
    }
    else if(e->key()==Qt::Key_W)
    {
        type = KEY_W;
    }
    emit sendKeyEvent(type);
}

void QGLESWIDGET::mouseMoveEvent(QMouseEvent *event)
{
    CELL::int2 point;
    if(leftB)
    {
        point = CELL::int2(event->x(),event->y());
    }
    if(rightB)
    {
        point = CELL::int2(event->x(),event->y());
    }
    emit sendMouseEvent(typeMouse,pos,point);
    pos = point;
}

void QGLESWIDGET::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !leftB)
    {
        leftB       = true;
        typeMouse   = MOUSE_LEFTDOWN;
        pos         = CELL::int2(event->x(),event->y());
    }
    if(event->button() == Qt::RightButton && !rightB)
    {
        rightB      = true;
        typeMouse   = MOUSE_RIGHTDOWN;
        pos         = CELL::int2(event->x(),event->y());
    }
}

void QGLESWIDGET::mouseReleaseEvent(QMouseEvent *event)
{
    MOUSEMODE type;
    CELL::int2 point;
    if(event->button() == Qt::LeftButton && leftB)
    {
        leftB   = false;
        type    = MOUSE_LEFTUP;
        point   = CELL::int2(event->x(),event->y());
    }
    if(event->button() == Qt::RightButton && rightB)
    {
        rightB  = false;
        type    = MOUSE_RIGHTUP;
        point   = CELL::int2(event->x(),event->y());
    }
    emit sendMouseEvent(type,pos,point);
    pos = point;
}

void QGLESWIDGET::wheelEvent(QWheelEvent *event)
{
    typeMouse = MOUSE_WHEEL;
    int p = event->delta();
    CELL::int2 pp = CELL::int2(event->x(),event->y());
    emit sendWheelEvent(typeMouse,p,pp);
}

unsigned int QGLESWIDGET::loadTexture(const char *fileName)
{
    unsigned int textureId   =   0;
    //1 获取图片格式
    FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);

    //2 加载图片
    FIBITMAP    *dib = FreeImage_Load(fifmt, fileName,0);

    int fmt = GL_RGB;
    //4 获取数据指针
    BYTE    *pixels =   (BYTE*)FreeImage_GetBits(dib);

    int     width   =   FreeImage_GetWidth(dib);
    int     height  =   FreeImage_GetHeight(dib);

    /**
         *注意：1、当启用alpha测试时，那么文理格式就是（GL_RGBA），
         *       需要把图像转化成32位图，同时改变（glTexImage2D）中相对应的参数
         *     2、可根据文理的文件格式进行判断，然后确定转化成24位图还是转换成32位图，也就是是否存在alpha值
         */

    if(fifmt == FIF_PNG)
    {
        dib =   FreeImage_ConvertTo32Bits(dib);
        fmt =   GL_RGBA;
        for (size_t i = 0 ;i < width * height * 4 ; i+=4 )
        {
            BYTE temp       =   pixels[i];
            pixels[i]       =   pixels[i + 2];
            pixels[i + 2]   =   temp;
        }
    }
    else
    {
        //3 转化为rgb 24色
        dib     =   FreeImage_ConvertTo24Bits(dib);
        for (size_t i = 0 ;i < width * height * 3 ; i+=3 )
        {
            BYTE temp       =   pixels[i];
            pixels[i]       =   pixels[i + 2];
            pixels[i + 2]   =   temp;
        }
    }


    /**
            *   产生一个纹理Id,可以认为是纹理句柄，后面的操作将书用这个纹理id
            */
    glGenTextures( 1, &textureId );

    /**
            *   使用这个纹理id,或者叫绑定(关联)
            */
    glBindTexture( GL_TEXTURE_2D, textureId );
    /**
            *   指定纹理的放大,缩小滤波，使用线性方式，即当图片放大的时候插值方式
            */
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    /**
            *   将图片的rgb数据上传给opengl.
            */
    glTexImage2D(
                GL_TEXTURE_2D,      //! 指定是二维图片
                0,                  //! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
                fmt,                //! 纹理的使用的存储格式
                width,              //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
                height,             //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
                0,                  //! 是否存在边
                fmt,                //! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
                GL_UNSIGNED_BYTE,   //! 数据是8bit数据
                pixels
                );
    /**
            *   释放内存
            */
    FreeImage_Unload(dib);

    return  textureId;
}

