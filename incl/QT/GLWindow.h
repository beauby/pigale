/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

/*!
\file GLWindow.h

\brief 3D Drawing window class
*/
#ifndef _GLWINDOW_H_INCLUDED_
#define _GLWINDOW_H_INCLUDED_


#include <QGLWidget>

class GLControlWidget; 

class GLWindow : public GLControlWidget
{

public:
  GLWindow(GraphGLPrivate *g,QWidget * parent);
  ~GLWindow();
  void initializeGL();
  void initialize(bool init);
  void animate();
  void setAnimationDelay(int ms);
  GLuint load(bool init);
  void image(QPrinter *printer,QString suffix);
  void print(QPrinter* printer);
private:
  void loadFaces();
  void paintGL();
  void paintEvent(QPaintEvent *e);
  void hideEvent(QHideEvent *e);
  void showEvent(QShowEvent *e);
  QSize sizeHint() const;
  void resizeGL(int w,int h);
  void drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size);
  void drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size,const QColor &col);
  void drawLabel(tvertex v,GLfloat x,GLfloat y,GLfloat z,GLfloat size);
  void drawText(void * font,const char *txt);
private:
  GraphGLPrivate *glp;
  bool is_init;
  GLuint object;
  //int CharSize;
};

#endif

