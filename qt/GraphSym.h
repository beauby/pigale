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
\file GraphSym.h
\brief widget of pigaleWindow
*/

#ifndef _GRAPH_SYM_H_INCLUDED_
#define _GRAPH_SYM_H_INCLUDED_

#include <QWidget>
#include <QEvent>

class GraphSymPrivate;
class pigaleWindow; 
class QPrinter;
//! Window to display symetries
class GraphSym : public QWidget
{
  Q_OBJECT
public:
  GraphSym( QWidget *parent,pigaleWindow* mw);
  ~GraphSym();
  void print(QPrinter *printer);
  void image(QPrinter* printer, QString suffix);

public slots:
  int update();
  void Next();
  void Factorial();
  void Optimal();
  void SymLabel();
private:
  void resizeEvent(QResizeEvent* e);
  GraphSymPrivate* d;
};

#endif
