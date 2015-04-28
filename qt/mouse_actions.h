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
\file mouse_actions.h
\brief  Display the mouses action buttons.
*/

#ifndef MOUSE_ACIONS_H
#define MOUSE_ACIONS_H

#include <QGroupBox>
#include <QLayout>
#include <QSlider>
#include <QLCDNumber>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>

class GraphWidget;
class GraphGL;

int GetMouseAction_1();


class Mouse_Actions : public QWidget
{ 
    Q_OBJECT

public:
  Mouse_Actions(QWidget* parent,GraphWidget* gw);
  ~Mouse_Actions();
  
  void contentsMousePressEvent(QMouseEvent* e);

  QGroupBox* ButtonGroup1;
  QRadioButton* ButtonAddV;
  QRadioButton* ButtonAddE;
  QRadioButton* ButtonDel;
  QRadioButton* ButtonMove;
  QRadioButton* ButtonBissect;
  QRadioButton* ButtonContract;
  QRadioButton* ButtonLabel;

  QCheckBox* ButtonFitGrid;
  QCheckBox* ButtonShowGrid;
  QPushButton* ButtonForceGrid;
  QPushButton* ButtonUndoGrid;

  QLCDNumber* LCDNumber;
  QLCDNumber* LCDNumberX;
  QLCDNumber* LCDNumberY;
  QSlider* Slider;


private slots:
  void valueChanged(int i);
};
#endif 
