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
\file mouse_actions.cpp
\brief  Display the mouses action buttons.
*/

#include "mouse_actions.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include <QButtonGroup>
#include <QColorDialog>

static int mouse_action_1 = 3;

int GetMouseAction_1()
  {return mouse_action_1;}
// sizes cannot be known at this point by calling methods
// but the width is defined by pigaleEditorMinXsize = pigaleEditorMaxXsize = 280

const int Height = 150; 
const int Width = 280-5; //5 borders
const int dy = 17;//18;
const int oy = 16;//20;                // origin of first radiobutton  
const int dx = 110;               // width of radiobuttons  (80) 
const int ox1 = 6;               // postion of left group
const int dx1 = 125;              // width radio group (90)
const int sliderOx = ox1+dx1+3;
const int sliderOy = 7;
const int sliderWidth = 16;
const int sliderHeight = 6*dy+oy-3; // +-height of the radiogroup
const int sizegrid = 40; //init value of the grid size of the graph
const int oy2 = 8;
const int ox2 =  sliderOx + sliderWidth + 3;    // postion of right group
const int dx2 = Width-ox2-1;             // group of radiobuttons
const int dxb = dx2/2;// width of buttons

Mouse_Actions::Mouse_Actions(QWidget* parent,GraphWidget* gw)
    : QWidget(parent)
  {
  setMinimumHeight(Height);  setMaximumHeight(Height); 

  ButtonGroup1 = new QGroupBox(this);
  ButtonGroup1->setGeometry(QRect(ox1-1,0,dx1,7*dy+oy+15)); 
  ButtonGroup1->setTitle(tr("Left Button"));
  QButtonGroup *group1 = new QButtonGroup( ButtonGroup1); 
  group1->setExclusive(true);

  QRadioButton *Button0 = new QRadioButton(ButtonGroup1);
  Button0->setGeometry(QRect(ox1,oy,dx,20)); 
  Button0->setText(tr("Col/Thick"));
  group1->addButton(Button0);group1->setId(Button0,0);

  QRadioButton *Button1 = new QRadioButton(ButtonGroup1);
  Button1->setGeometry(QRect(ox1,oy+dy,dx,20)); 
  Button1->setText(tr("Add/Del"));
  group1->addButton(Button1);group1->setId(Button1,1);

  QRadioButton *Button2 = new QRadioButton(ButtonGroup1);
  Button2->setGeometry(QRect(ox1,oy+2*dy,dx,20)); 
  Button2->setText(tr("Orient"));
  group1->addButton(Button2);group1->setId(Button2,2);

  QRadioButton *Button3 = new QRadioButton(ButtonGroup1);
  Button3->setGeometry(QRect(ox1,oy+3*dy,dx,20)); 
  Button3->setText(tr("Move"));
  Button3->setChecked(true);
  group1->addButton(Button3);group1->setId(Button3,3);

  QRadioButton *Button4 = new QRadioButton(ButtonGroup1);
  Button4->setGeometry(QRect(ox1,oy+4*dy,dx,20)); 
  Button4->setText(tr("Bis./Cont."));
  group1->addButton(Button4);group1->setId(Button4,4);

  QRadioButton *Button5 = new QRadioButton(ButtonGroup1);
  Button5->setGeometry(QRect(ox1,oy+5*dy,dx,20)); 
  Button5->setText(tr("Ext Face/extbrin"));
  group1->addButton(Button5);group1->setId(Button5,5);

  QRadioButton *Button6 = new QRadioButton(ButtonGroup1);
  Button6->setGeometry(QRect(ox1,oy+6*dy,dx,20)); 
  Button6->setText(tr("Label/Reset all"));
  group1->addButton(Button6);group1->setId(Button6,6);

    //****************************************************************
  gw->editor->sizegridChanged(sizegrid);
  LCDNumber = new QLCDNumber(this);
  LCDNumber->setGeometry(QRect(ox2,oy2,30,30)); 
  QFont LCDNumber_font(LCDNumber->font());
  LCDNumber_font.setBold(true);
  LCDNumber->setFont(LCDNumber_font); 
  //hub LCDNumber->setNumDigits(3);
  LCDNumber->setDigitCount(3);
  LCDNumber->setSegmentStyle(QLCDNumber::Flat);
  //LCDNumber->setProperty("intValue",sizegrid);
  LCDNumber->display(sizegrid);

  LCDNumberX = new QLCDNumber(this);
  QColor col;         col.setRgb(0,100,0);
  // 0:background gprop 1:background msg 3:background help 2:background mouse
  QPalette pal = QPalette(QColorDialog::customColor(1));
  pal.setColor(QPalette::Foreground,col);
  LCDNumberX->setPalette(pal);
  LCDNumberX->setGeometry(QRect(ox2+35,oy2,30,30)); 
  LCDNumberX->setFont(LCDNumber_font); 
  //LCDNumberX->setNumDigits(3);
  LCDNumberX->setDigitCount(3);
  LCDNumberX->setSegmentStyle(QLCDNumber::Flat);
  LCDNumberX->setProperty("intValue",sizegrid);
  LCDNumberX->display(sizegrid);

  LCDNumberY = new QLCDNumber(this);
  LCDNumberY->setPalette(pal);
  LCDNumberY->setGeometry(QRect(ox2+35+30+2,oy2,30,30)); 
  LCDNumberY->setFont(LCDNumber_font); 
  //LCDNumberY->setNumDigits(3);
  LCDNumberY->setDigitCount(3);
  LCDNumberY->setSegmentStyle(QLCDNumber::Flat);
  //LCDNumberY->setProperty("intValue",sizegrid);
  LCDNumberY->display(sizegrid);

  Slider = new QSlider(this);
  Slider->setGeometry(QRect(sliderOx,sliderOy,sliderWidth,sliderHeight)); 
  Slider->setMinimum(2);    Slider->setMaximum(100);
  Slider->setValue(sizegrid);
  Slider->setOrientation(Qt::Vertical);

  ButtonFitGrid = new QCheckBox(this);
  ButtonFitGrid->setGeometry(QRect(ox2,oy2+35,Width-ox2,20)); 
  ButtonFitGrid->setText(tr("Fit grid"));

  ButtonShowGrid = new QCheckBox(this);
  ButtonShowGrid->setGeometry(QRect(ox2,oy2+55,Width-ox2,20)); 
  ButtonShowGrid->setText(tr("Show grid"));
  
  ButtonForceGrid = new QPushButton(this);
  ButtonForceGrid->setGeometry(QRect(ox2,oy2+78,dxb,20));
  ButtonForceGrid->setText(tr("Force G"));

  ButtonUndoGrid = new QPushButton(this);
  ButtonUndoGrid->setGeometry(QRect(ox2+dxb,oy2+78,dxb,20));
  ButtonUndoGrid->setText(tr("Undo G"));
  ButtonUndoGrid->setDisabled(true);

  // signals and slots connections
  connect(group1,SIGNAL(buttonClicked(int)),SLOT(valueChanged(int)));
  connect(Slider,SIGNAL(valueChanged(int)),LCDNumber,SLOT(display(int)));
  connect(Slider,SIGNAL(valueChanged(int)),gw->editor,SLOT(sizegridChanged(int)));
  connect(ButtonShowGrid,SIGNAL(toggled(bool)),gw->editor,SLOT(showGrid(bool)));
  connect(ButtonFitGrid,SIGNAL(toggled(bool)),gw->editor,SLOT(fitgridChanged(bool)));
  connect(ButtonForceGrid,SIGNAL(clicked()),gw->editor,SLOT(ForceToGrid()));
  connect(ButtonUndoGrid,SIGNAL(clicked()),gw->editor,SLOT(UndoGrid()));
  }
Mouse_Actions::~Mouse_Actions()
{}
void Mouse_Actions::valueChanged(int i)
  {mouse_action_1 = i;
  }
