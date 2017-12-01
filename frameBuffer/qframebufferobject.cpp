#include "qframebufferobject.h"

QFrameBufferObject::QFrameBufferObject()
{

}

bool QFrameBufferObject::setFBO(int width, int height)
{
    _wFbo = width;  _hFbo = height;

    glActiveTexture(GL_TEXTURE_2D);
    glGenTextures(1,&_texFbo);
    glBindTexture(GL_TEXTURE_2D,_texFbo);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,_wFbo,_hFbo,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenBuffers(1,&_fbo);
    glEnable(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER,_fbo);

    glGenBuffers(1,&_rbo);
    glEnable(GL_RENDERBUFFER);
    glBindRenderbuffer(GL_RENDERBUFFER,_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,_wFbo,_hFbo);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return true;
}

void QFrameBufferObject::begin()
{
    glBindFramebuffer(GL_FRAMEBUFFER,_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texFbo, 0);
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo );
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void QFrameBufferObject::end()
{
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void QFrameBufferObject::destroy()
{
    glDeleteFramebuffers(1, &_fbo);
    glDeleteRenderbuffers(1,&_rbo);
    _rbo  =   0;
    _fbo  =   0;
}

