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
/*! \file GraphGL.h
\brief 3D embedding class definition
\ingroup qt */

#ifndef _GRAPH_GL_H_INCLUDED_
#define _GRAPH_GL_H_INCLUDED_

#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QEvent>
#include <QTabWidget>
#include <QLayout>
#include <QPrinter>


class GraphGLPrivate;
class pigaleWindow; 

class GraphGL : public QWidget
{
  Q_OBJECT
public:
  GraphGL(QWidget *parent,pigaleWindow* mw);
  ~GraphGL();
  int init();
public slots:
  int update();
  void image(QPrinter *printer,QString suffix);
  void print(QPrinter* printer);
  void delayChanged(int i);
private slots:
  void Reload();
  void Reload(int i);
  void axisChanged(int i);
  void EdgeWidth(int i);
  void VertexWidth(int i);
private:
  void resizeEvent(QResizeEvent*);
  GraphGLPrivate* d;
  QSpinBox *spin_X,*spin_Y,*spin_Z;
};

#endif
