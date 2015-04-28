 
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
\file pigaleMenus.cpp
\ingroup pigaleWindow
\brief Menus, Toolbar and most widgets
*/

#include <config.h>
#include "pigaleWindow.h"
#include <TAXI/Tgf.h>
#include <TAXI/Tdebug.h>
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include "ClientSocket.h"

#include <QT/pigaleWindow_doc.h> 
#include <QT/staticData.h>
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <QT/Action_def.h>
#include <QT/pigalePaint.h> 
#include <QT/clientEvent.h> 

#include <QIcon>
#include <QPixmap>
#include <QTextBrowser>
#include <QBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QToolBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QWhatsThis>
#include <QValidator>
#include <QDesktopWidget>

#include "icones/fileopen.xpm"
#include "icones/filenew.xpm"
#include "icones/fileprint.xpm"
#include "icones/filesave.xpm"
#include "icones/info.xpm"
#include "icones/sleft.xpm"
#include "icones/sright.xpm"
#include "icones/sreload.xpm"
#include "icones/help.xpm"
#include "icones/xman.xpm"
#include "icones/sleftarrow.xpm"
#include "icones/srightarrow.xpm"
#include "icones/sfilesave.xpm"
#include "icones/macroplay.xpm"
#include "icones/film.xpm"

void Init_IOGraphml();
void UndoErase();
void initGraphDebug();

pigaleWindow::pigaleWindow()
    :QMainWindow()
    ,ServerExecuting(false),ServerClientId(0) // end public
    ,GraphIndex1(1)
    //start private
    ,server(NULL)
    ,pGraphIndex(&GraphIndex1),GraphIndex2(1)
    ,UndoIndex(0),UndoMax(0)
    ,MacroNumActions(0),MacroRecording(false),MacroLooping(false)
    ,MacroExecuting(false),MacroPlay(false),_key(0),Server(false)
    ,numMessages(0)
  {setObjectName("Main Pigale Window");
  // set Title
#ifdef TDEBUG
  setWindowTitle(tr("Qt5 Pigale Editor:")+" "+PACKAGE_VERSION+" "+tr("Debug Mode"));
#else
  setWindowTitle(tr("Qt5 Pigale Editor:")+" "PACKAGE_VERSION);
#endif
  // Load settings
  LoadSettings();
  // Modify settings according to passed arguments
  ParseArguments();
  // Create a printer
  printer = new QPrinter(); 
  //printer->setOrientation(QPrinter::Portrait); 
  printer->setColorMode(QPrinter::Color);
  //printer->setDocName("Pigale (C) 2001");
   
  // mainWidget
  QWidget *mainWidget = new QWidget(this);  mainWidget->setAutoFillBackground(true);
  setCentralWidget(mainWidget);
  mainWidget->setFocus();
  setAutoFillBackground(true);
  // toolBar
  createToolBar();
  // create all widgets
  createLayout(mainWidget);
  // menus
  createMenus();
 // progressBar
  progressBar = new QProgressBar(statusBar());
  progressBar->hide();
  progressBar->setGeometry(QRect(0,0,width()*2/3,30)); 
  UpdatePigaleColors();  
  initPigale();
  gw->editor->update(1);
  // post a message to know when initialization complete
  readyEvent *e = new readyEvent();
  QApplication::postEvent(this,e);
  }
void pigaleWindow::whenReady()
  {if(MacroPlay && macroLoad(MacroFileName) != -1)
      {load(0);macroPlay(true);}
  else if(Server)
      initServer();
  else
      load(0);
  }
int pigaleWindow::setId(QAction * action,int Id)
  {action->setData(Id);
  menuIntAction[Id] = action;
  return Id;
  }
int pigaleWindow::getId(QAction *action)
  {return (action->data()).toInt();
  }
void pigaleWindow::initPigale()
  {// Initialize Error
  setPigaleError();
  // Macros
  MacroActions.resize(0,4); MacroActions.SetName("MacroActions");
#ifdef _WINDOWS
   initGraphDebug();// as the Microsoftcompiler does not initialize static ...
#endif
   DefinepigaleWindow(this); // only used by EmbedCurve
  //thread
   createThread();

  //qApp->setMainWidget(this); // to be able to retrieve the mainWidget
   mapActionsInit();// Create the actions map
   Init_IO();// Initialize input/output drivers
   Init_IOGraphml();


  QFileInfo fi0 =  QFileInfo(InputFileName);
  QFileInfo fi = QFileInfo(fi0.absolutePath());
  if(!fi.exists() || !fi.isDir())
      {QString DirFile = QFileDialog::
      getExistingDirectory(this
                           ,tr("Choose the TGF directory")
                           ,"."
                           ,QFileDialog::ShowDirsOnly);
      InputFileName = DirFile + QDir::separator() + fi0.fileName();
      OutputFileName = InputFileName;
      }
  // Load inpu/output drivers
  InputDriver = IO_WhoseIs((const char *)InputFileName.toLatin1());
  if (InputDriver<0) InputDriver=0;
  OutputDriver = IO_WhoseIs((const char *)OutputFileName.toLatin1());
  if (OutputDriver<0) OutputDriver=0;

  // Init random generator
  if(staticData::RandomSeed())randomInitSeed();
  //Check for documentation directory
  CheckDocumentationPath();
  if(CheckLogFile() == -1)
      {QString msg = QString("Impossible to write in ")+logfile();
      Twait((const char *)msg.toLatin1());
      }
  UndoInit();// Create a tgf file with no records
  LogPrintf("Init seed:%ld\n",randomSetSeed());
  }
void pigaleWindow::createLayout(QWidget *mainWidget)
  {QHBoxLayout * leftLayout = new QHBoxLayout(mainWidget);
  tabWidget = new  QTabWidget();
  leftLayout->addWidget(tabWidget,1);
  tabWidget->setMinimumSize(465,425);
  mypaint =  new pigalePaint(0,this);
  gw = new  GraphWidget(0,this);   gw->setAutoFillBackground(true);
  graphgl  = new GraphGL(mainWidget,this);   graphgl->setAutoFillBackground(true);
  graphsym = new GraphSym(0,this);  graphsym->setAutoFillBackground(true);
  browser = new QTextBrowser(0);    browser->setAutoFillBackground(true);
  QPalette bop(QColorDialog::customColor(3));
  browser->setPalette(bop);
  gw->setPalette(bop);
  tabWidget->addTab(gw,tr("Graph Editor"));
  tabWidget->addTab(mypaint,"");
  tabWidget->addTab(graphgl,"");
  tabWidget->addTab(graphsym,"");
  tabWidget->addTab(browser,tr("User Guide"));
  createRightLayout(leftLayout);
  }
void pigaleWindow::createRightLayout(QHBoxLayout * leftLayout)
  {rtabWidget = new  QTabWidget(); 
  gSettings = new QWidget();  gSettings->setAutoFillBackground(true);  
  gInfo = new QWidget(); gInfo->setAutoFillBackground(true);  
  rtabWidget->setMaximumWidth(300);  rtabWidget->setMinimumWidth(300);  rtabWidget->setMinimumHeight(500); 
  leftLayout->addWidget(rtabWidget,2);
  rtabWidget->addTab(gInfo,tr("Information"));
  rtabWidget->addTab(gSettings,tr("Settings"));
  createPageInfo(gInfo);
  createPageSettings(gSettings,leftLayout);
  }
void pigaleWindow::createPageInfo(QWidget *gInfo)
  {QGridLayout *rightLayout = new  QGridLayout(gInfo);
  //messages
  messages = new QTextEdit(gInfo);
  QBrush pb(QColorDialog::customColor(1));
  messages->setReadOnly(true);
  //graph_properties
  graph_properties = new Graph_Properties(gInfo,menuBar(),this);
  graph_properties->setAutoFillBackground(true); 
  //mouse_action
  mouse_actions = new Mouse_Actions(gInfo,gw);
  mouse_actions->setAutoFillBackground(true); 

  rightLayout->addWidget(messages,0,0,1,2);
  rightLayout->addWidget(graph_properties,1,0,1,2);
  rightLayout->addWidget(mouse_actions,2,0,1,2);
  }
void pigaleWindow::createPageSettings(QWidget *gSettings,QHBoxLayout * leftLayout)
  {  //generators
  QSpinBox *spin_N1 = new QSpinBox();  
  spin_N1->setRange(1,65000); spin_N1->setSingleStep(1); spin_N1->setValue(staticData::Gen_N1);  
  QSpinBox *spin_N2 = new QSpinBox();  
  spin_N2->setRange(1,65000); spin_N2->setSingleStep(1); spin_N2->setValue(staticData::Gen_N2);  
  QSpinBox *spin_M = new QSpinBox();     
  spin_M->setRange(1,300000); spin_M->setSingleStep(1); spin_M->setValue(staticData::Gen_M); 
  connect(spin_N1,SIGNAL(valueChanged(int)),SLOT(spinN1Changed(int)));
  connect(spin_N2,SIGNAL(valueChanged(int)),SLOT(spinN2Changed(int)));
  connect(spin_M,SIGNAL(valueChanged(int)),SLOT(spinMChanged(int)));
  // Seed
  QValidator *validator = new QIntValidator(this);
  QLineEdit *seedEdit =  new QLineEdit(0);
  seedEdit->setValidator(validator);
  connect(seedEdit,SIGNAL(textEdited(const QString & )),SLOT(seedEdited(const QString & )));
  seedEdit->setText(QString("%1").arg(randomSetSeed()));
  // Macro delay
  QSpinBox *spinDelay = new QSpinBox(); 
  spinDelay->setRange(0,600); spinDelay->setSingleStep(10); 
  spinDelay->setValue(staticData::macroDelay);
  connect(spinDelay,SIGNAL(valueChanged(int)),SLOT(spinDelayChanged(int)));
  // Macro repeat
  QSpinBox *spinRepeat = new QSpinBox();     
  spinRepeat->setRange(0,1000000); spinRepeat->setSingleStep(100);
  spinRepeat->setValue(staticData::macroRepeat); 
  connect(spinRepeat,SIGNAL(valueChanged(int)),SLOT(spinRepeatChanged(int)));
  // Labels of vertices
  QComboBox *comboLabel = new QComboBox(0);
  comboLabel->addItem(tr("Nothing"));
  comboLabel->addItem(tr("Index"));
  comboLabel->addItem(tr("Text"));
  comboLabel->addItem(tr("Label"));
  int current = staticData::ShowVertex();current += 3;
  comboLabel->setCurrentIndex(current);showLabel(current);
  connect(comboLabel,SIGNAL(activated(int)),SLOT(showLabel(int)));
  // Partition
  QSpinBox *spin_N = new QSpinBox();  
  spin_N->setRange(2,50);  spin_N->setSingleStep(1); spin_N->setValue(2);
  connect(spin_N,SIGNAL(valueChanged(int)),SLOT(spinNChanged(int)));
  // Distances
  QComboBox *comboDistance  = new QComboBox(0);
  comboDistance->addItem(tr("Czekanovski-Dice"));
  comboDistance->addItem(tr("Bisect"));
  comboDistance->addItem(tr("Adjacence"));
  comboDistance->addItem(tr("Adjacence M"));
  comboDistance->addItem(tr("Laplacian"));
  comboDistance->addItem(tr("Q-distance"));
  comboDistance->addItem(tr("Oriented"));
  comboDistance->addItem(tr("R2"));
  comboDistance->setCurrentIndex(staticData::UseDistance());distOption(staticData::UseDistance());
  connect(comboDistance,SIGNAL(activated(int)),SLOT(distOption(int)));
  // Limits
  QSpinBox *spin_MaxNS = new QSpinBox();     
  spin_MaxNS->setRange(1,50000); spin_MaxNS->setSingleStep(100); spin_MaxNS->setValue(staticData::MaxNS); 
  QSpinBox *spin_MaxND = new QSpinBox();        
  spin_MaxND->setRange(1,5000); spin_MaxND->setSingleStep(100); spin_MaxND->setValue(staticData::MaxND);
  connect(spin_MaxNS,SIGNAL(valueChanged(int)),this,SLOT(spinMaxNSChanged(int)));
  connect(spin_MaxND,SIGNAL(valueChanged(int)),this,SLOT(spinMaxNDChanged(int)));
  // CheckBoxes
  QButtonGroup *Group1 = new QButtonGroup(0);  Group1->setExclusive(false); //0 -> this ==
  connect(Group1,SIGNAL(buttonClicked(int)),this,SLOT(settingsHandler(int)));
  QCheckBox *box1,*box2,*box3,*box4,*box5,*box7,*box8,*box9,*box10,*box11;
  box1 = new  QCheckBox("Debug",0);
  box1->setCheckState(debug() ? Qt::Checked : Qt::Unchecked); 
  Group1->addButton(box1,A_SET_DEBUG);
  box2 = new  QCheckBox(tr("Undo"),0);
  box2->setCheckState(staticData::IsUndoEnable ? Qt::Checked : Qt::Unchecked);
  Group1->addButton(box2,A_SET_UNDO);
  box3 = new  QCheckBox(tr("Erase multiple edges"),0);
  box3->setCheckState(staticData::RandomEraseMultipleEdges() ? Qt::Checked : Qt::Unchecked); 
  Group1->addButton(box3,A_SET_ERASE_MULT);
  box4 = new  QCheckBox(tr("Use generated cir"),0);  
  box4->setCheckState(staticData::RandomUseGeneratedCir() ? Qt::Checked : Qt::Unchecked);
  Group1->addButton(box4,A_SET_GEN_CIR);
  box5 = new  QCheckBox(tr("Random seed"),0);    
  box5->setCheckState(staticData::RandomSeed() ? Qt::Checked : Qt::Unchecked);
  Group1->addButton(box5,A_SET_RANDOM_SEED);
  chkOrient = new  QCheckBox(tr("Show orientation"),0);  
  chkOrient->setCheckState(staticData::ShowOrientation() ? Qt::Checked : Qt::Unchecked);
  Group1->addButton(chkOrient, A_SET_ORIENT);
  box7 = new  QCheckBox(tr("Show arrows"),0);    
  box7->setCheckState(staticData::ShowArrow() ? Qt::Checked : Qt::Unchecked);   
  Group1->addButton(box7, A_SET_ARROW);
  box8 = new  QCheckBox(tr("Show exterior edge"),0);  
  box8->setCheckState(staticData::ShowExtTbrin() ? Qt::Checked : Qt::Unchecked);  
  Group1->addButton(box8,A_SET_EXTBRIN);
  box9 = new  QCheckBox(tr("Schnyder Rect"),0);
  box9->setCheckState(staticData::SchnyderRect() ? Qt::Checked : Qt::Unchecked);  
  Group1->addButton(box9,A_SET_SCH_RECT); 
  box10 = new  QCheckBox(tr("Schnyder color"),0);
  box10->setCheckState(staticData::SchnyderColor() ? Qt::Checked : Qt::Unchecked); 
  Group1->addButton(box10,A_SET_SCH_COLOR);
  box11 = new  QCheckBox(tr("Use longest face"),0);
  box11->setCheckState(staticData::SchnyderLongestFace() ? Qt::Checked : Qt::Unchecked); 
  Group1->addButton(box11,A_SET_LFACE);
  
  // png
  QSpinBox *spinPNG = new QSpinBox();     
  spinPNG->setRange(100,1600); spinPNG->setSingleStep(50);  spinPNG->setValue(staticData::sizeImage);
  connect(spinPNG,SIGNAL(valueChanged(int)),SLOT(spinPNGChanged(int)));

  // Add to the Layout
  QDesktopWidget *desktop = QApplication::desktop();
  bool bigScreen = (desktop->height() >= 768); //630
  //bigScreen=false;
  const int mw = 100;
  const int mh = 18;
  if(bigScreen)
  // minimum Height window = 640
  // minimum Width window  = 800
      {gSettings->setMinimumHeight(495);
      gSettings->setMaximumHeight(640);// 600
      }
  else
  // minimum Height window = 600
  // minimum Width window  = 780
      {gSettings->setMaximumHeight(485);
      tb->setIconSize(QSize(14,14));
      leftLayout->setMargin(0);
      }
      
  comboLabel->setMaximumWidth(mw);comboLabel->setMinimumHeight(mh);
  comboDistance->setMaximumWidth(mw+40);comboDistance->setMinimumHeight(mh);
  seedEdit->setMaximumWidth(mw);seedEdit->setMinimumHeight(mh);
  spin_N->setMaximumWidth(mw);spin_N->setMinimumHeight(mh);
  spin_N1->setMaximumWidth(mw);spin_N1->setMinimumHeight(mh);
  spin_N2->setMaximumWidth(mw);spin_N2->setMinimumHeight(mh);
  spin_M->setMaximumWidth(mw);spin_M->setMinimumHeight(mh);
  spin_MaxNS->setMaximumWidth(mw);spin_MaxNS->setMinimumHeight(mh);
  spin_MaxND->setMaximumWidth(mw);spin_MaxND->setMinimumHeight(mh);
  spinDelay->setMaximumWidth(mw);spinDelay->setMinimumHeight(mh);
  spinRepeat->setMaximumWidth(mw);spinRepeat->setMinimumHeight(mh);
  spinPNG->setMaximumWidth(mw);spinPNG->setMinimumHeight(mh);
  
//   // for styles like CDE
//   comboDistance->setFrame(false);
//   comboLabel->setFrame(false);


  QGridLayout *setLayout = new  QGridLayout(gSettings);
 
  int row = -1;
  // General
  if(bigScreen)
      setLayout->addWidget(new QLabel(tr("<b>General settings</b>"),0),++row,1,1,2,Qt::AlignHCenter);
  setLayout->addWidget(box1,++row,1);  setLayout->addWidget(box2,row,2); //debug,undo
  setLayout->addWidget(chkOrient,++row,1);  setLayout->addWidget(box7,row,2); // orientation,arrowq
  setLayout->addWidget(box8,++row,1);  setLayout->addWidget(box11,row,2); // exterioredge, longest face
  setLayout->addWidget(new QLabel(tr("Vertex label"),0),++row,1,Qt::AlignLeft);  setLayout->addWidget(comboLabel,row,2);
  setLayout->addWidget(new QLabel(tr("Max N display"),0),++row,1);
  setLayout->addWidget(new QLabel(tr("Max N slow algorithms"),0),row,2);
  setLayout->addWidget(spin_MaxND,++row,1); setLayout->addWidget(spin_MaxNS,row,2);
  
  // generators
  if(bigScreen)
      {setLayout->addWidget(new QLabel(" ",0),++row,1);
      setLayout->addWidget(new QLabel(tr("<b>Graph Generators</b>"),0),++row,1,1,2,Qt::AlignHCenter);
      //setLayout->addWidget(new QLabel(" ",0),++row,1);
      }
  setLayout->addWidget(new QLabel("N1",0),++row,1);    setLayout->addWidget(spin_N1,row,2);
  setLayout->addWidget(new QLabel("N2",0),++row,1);    setLayout->addWidget(spin_N2,row,2);
  setLayout->addWidget(new QLabel("M",0),++row,1);     setLayout->addWidget(spin_M,row,2);
  setLayout->addWidget(new QLabel(tr("Seed"),0),++row,1);  setLayout->addWidget(seedEdit,row,2);
  setLayout->addWidget(box3,++row,1);  setLayout->addWidget(box4,row,2); // multiple edges, generated cir
  setLayout->addWidget(box5,++row,1); // random seed

  // macro
  if(bigScreen)
      {setLayout->addWidget(new QLabel(" ",0),++row,1);
      setLayout->addWidget(new QLabel(tr("<b>Macro Settings</b>"),0),++row,1,1,2,Qt::AlignHCenter);
      //setLayout->addWidget(new QLabel(" ",0),++row,1);
      }
  setLayout->addWidget(new QLabel(tr("Delay (1/10 seconds)"),0),++row,1);  setLayout->addWidget(spinDelay,row,2);
  setLayout->addWidget(new QLabel(tr("Repeat"),0),++row,1);setLayout->addWidget(spinRepeat,row,2);

  //Factorial Analysis
  if(bigScreen)
      {setLayout->addWidget(new QLabel(" ",0),++row,1);
      setLayout->addWidget(new QLabel(tr("<b>Factorial analysis</b>"),0),++row,1,1,2,Qt::AlignHCenter);
      //setLayout->addWidget(new QLabel(" ",0),++row,1);
      }
  setLayout->addWidget(new QLabel(tr("Distance"),0),++row,1);  setLayout->addWidget(new QLabel(tr("Number of classes"),0),row,2);
  setLayout->addWidget(comboDistance,++row,1);setLayout->addWidget(spin_N,row,2);
  
  // PNG
  if(bigScreen)
      setLayout->addWidget(new QLabel(" ",0),++row,1);
  setLayout->addWidget(new QLabel(tr("Image size"),0),++row,1);
  setLayout->addWidget(spinPNG,++row,1);
  
  // Schnyder
  if(bigScreen)
      {setLayout->addWidget(new QLabel(" ",0),++row,1);
      setLayout->addWidget(new QLabel("<b>Schnyder</b>",0),++row,1,1,2,Qt::AlignHCenter);
      //setLayout->addWidget(new QLabel(" ",0),++row,1);
      }
  setLayout->addWidget(box9,++row,1); setLayout->addWidget(box10,row,2); //Schnyder rect,color
  //cout<<"row:"<<row<<endl;
  }
void pigaleWindow::createToolBar()
  {//Pixmaps

  //QPixmap _open  = QPixmap(fileopen);
  QIcon openIcon = QPixmap(fileopen);
  QIcon newIcon = QPixmap(filenew),saveIcon = QPixmap(filesave);
  QIcon leftIcon = QPixmap(sleft),   rightIcon = QPixmap(sright);
  QIcon reloadIcon = QPixmap(sreload);
  QIcon infoIcon = QPixmap(info), helpIcon = QPixmap(help),printIcon = QPixmap(fileprint);
  QIcon xmanIcon = QPixmap(xman), undoLIcon = QPixmap(sleftarrow);
  QIcon undoSIcon = QPixmap(sfilesave),undoRIcon = QPixmap(srightarrow);
  QIcon macroplayIcon = QPixmap(macroplay),filmIcon = QPixmap(film);

  //ToolBar
  // NEW LOAD SAVE
  tb = new QToolBar(this);  addToolBar(tb);  tb->setMovable(true); 
  QAction *newAct = new QAction(newIcon, tr("&New"), this);
  newAct->setStatusTip(tr("New graph"));
  connect(newAct, SIGNAL(triggered()),this,SLOT(NewGraph()));
  tb->addAction(newAct);
  QAction *openAct = new QAction(openIcon, tr("&Open"), this);
  openAct->setStatusTip(tr("Open a new graph"));openAct->setWhatsThis(fileopen_txt);
  connect(openAct, SIGNAL(triggered()),this,SLOT(load()));
  tb->addAction(openAct);
  QAction *saveAct = new QAction(saveIcon, tr("Save"), this);
  saveAct->setStatusTip(tr("Save the graph"));
  connect(saveAct, SIGNAL(triggered()),this,SLOT(save()));
  tb->addAction(saveAct);
  // PRINT PNG
  QAction *printAct = new QAction(printIcon, tr("&Print"), this);
  printAct->setStatusTip(tr("Print the graph"));
  connect(printAct, SIGNAL(triggered()),this,SLOT(print()));
  tb->addAction(printAct);
  QAction *imageAct = new QAction(filmIcon, tr("Image"), this);
  imageAct->setStatusTip(tr("Save image"));
  connect(imageAct, SIGNAL(triggered()),this,SLOT(image()));
  tb->addAction(imageAct);
  // INFO
  QAction *infoAct = new QAction(infoIcon, tr("&Information"), this);
  infoAct->setStatusTip(tr("Information"));
  connect(infoAct, SIGNAL(triggered()),this,SLOT(information()));
  tb->addAction(infoAct);
  tb->addSeparator();
  // LEFT RELOAD RIGHT
  QAction *leftAct = new QAction(leftIcon, tr("Previous graph"), this);
  leftAct->setStatusTip(tr("Previous graph (tgf file)"));leftAct->setWhatsThis(left_txt);
  connect(leftAct, SIGNAL(triggered()),this,SLOT(previous()));
  tb->addAction(leftAct);
  QAction *reloadAct = new QAction(reloadIcon, tr("Reload"), this);
  reloadAct->setStatusTip(tr("Reload graph"));reloadAct->setWhatsThis(redo_txt);
  connect(reloadAct, SIGNAL(triggered()),this,SLOT(reload()));
  tb->addAction(reloadAct);
  QAction *rightAct = new QAction(rightIcon, tr("Next graph"), this);
  rightAct->setStatusTip(tr("Next graph (tgf file)"));rightAct->setWhatsThis(right_txt);
  connect(rightAct, SIGNAL(triggered()),this,SLOT(next()));
  tb->addAction(rightAct);
  tb->addSeparator();
  // UNDO
  undoLAct = new QAction(undoLIcon, tr("Undo"), this);
  undoLAct->setStatusTip(tr("Undo last action"));
  connect(undoLAct, SIGNAL(triggered()),this,SLOT(Undo()));
  tb->addAction(undoLAct);
  undoSAct = new QAction(undoSIcon, tr("Undo save"), this);
  undoSAct->setStatusTip(tr("Undo save"));
  connect(undoSAct, SIGNAL(triggered()),this,SLOT(UndoSave()));
  tb->addAction(undoSAct);
  undoRAct = new QAction(undoRIcon, tr("Redo"), this);
  undoRAct->setStatusTip(tr("Redo last action"));
  connect(undoRAct, SIGNAL(triggered()),this,SLOT(Redo()));
  tb->addAction(undoRAct);
  undoLAct->setEnabled(false); undoSAct->setEnabled(staticData::IsUndoEnable);undoRAct->setEnabled(false);
  tb->addSeparator();
  // MACRO
  QAction *macroAct = new QAction(macroplayIcon, tr("Play the macro"), this);
  macroAct->setStatusTip(tr("Play the macro"));
  connect(macroAct, SIGNAL(triggered()),this,SLOT(macroPlay()));
  tb->addAction(macroAct);
  // Help
  QAction *helpAct =  QWhatsThis::createAction(); 
  //connect(helpAct, SIGNAL(triggered()),this,SLOT(whatsThis()));
  helpAct->setShortcut(Qt::SHIFT+Qt::Key_F1);
  tb->addAction(helpAct);
  }
void pigaleWindow::createMenus()
  {connect(menuBar(),SIGNAL(triggered(QAction *)),this,SLOT(handler(QAction *)));
  QAction *action;
  //Pixmaps
  QIcon openIcon = QPixmap(fileopen),newIcon = QPixmap(filenew),saveIcon = QPixmap(filesave);
  QIcon helpIcon = QPixmap(help),printIcon = QPixmap(fileprint);
  QIcon xmanIcon = QPixmap(xman),infoIcon = QPixmap(info);

  QMenu *file = menuBar()->addMenu( tr("&File"));  //-> inutil
  file->addAction(newIcon,tr("&New Graph"),this, SLOT(NewGraph()));
  action = file->addAction(openIcon,tr("&Open"),this, SLOT(load()));
  action->setWhatsThis(fileopen_txt);
  file->addAction(tr("Save as"), this, SLOT(saveAs()));
  file->addAction(saveIcon,tr("Save"), this, SLOT(save()));
  file->addSeparator();
  file->addAction(tr("Delete current record"),this,SLOT(deleterecord()));
  file->addAction(tr("Switch Input/Output files"),this,SLOT(switchInputOutput()));
  file->addSeparator();
  file->addAction(printIcon,tr("&Print"),this, SLOT(print()));
  file->addSeparator();
  //file->addAction(tr("Init server"),this, SLOT(initServer()));
  action = file->addAction(tr("Init server")); 
  setId(action,A_SERVER_INIT);
  connect(action, SIGNAL(triggered()),this,SLOT(initServer()));
  file->addSeparator();
  QAction * exitAct = new QAction(tr("E&xit"),this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()),this,SLOT(close()));
  file->addAction(exitAct);
  //file->addAction(tr("&Quit"),this,SLOT(close()));
  //file->addAction(tr("&Quit"),qApp,SLOT(closeAllWindows()));

  QMenu *augment = menuBar()->addMenu( tr("&Augment")); 

  action = augment->addAction(xmanIcon,tr("Make &Connected (edge)")); 
  setId(action,A_AUGMENT_CONNECT);
  action->setWhatsThis(tr("Make a graph connected adding edges"));

  action = augment->addAction(xmanIcon,tr("Make Connected (vertex)"));
  setId(action,A_AUGMENT_CONNECT_V);
  action->setWhatsThis(tr("Make a graph connected adding a vertex"));

  action = augment->addAction(xmanIcon,tr("Make &2-Connected a planar graph (edge)")); 
  setId(action,A_AUGMENT_BICONNECT);
  action->setWhatsThis(tr("Make a PLANAR graph 2-connected adding edges"));

  action = augment->addAction(xmanIcon,tr("Make 2-Connected a planar graph &Opt (edge)"));
  setId(action,A_AUGMENT_BICONNECT_6);
  action->setWhatsThis(tr("Make a PLANAR graph 2-connected\n adding edges ( minimal degree increase)"));

  action = augment->addAction(xmanIcon,tr("Make 2-Connected  (edge)")); 
  setId(action,A_AUGMENT_BICONNECT_NP);
  action->setWhatsThis(tr("Make 2-connected a graph (planar or not)  adding edges"));

  action = augment->addAction(xmanIcon,tr("Make 2-Connected   (vertex)")); 
  setId(action,A_AUGMENT_BICONNECT_NP_V);
  action->setWhatsThis(tr("Make 2-connected a graph (planar or not) adding vertices"));

  action = augment->addAction(xmanIcon,tr("&Vertex Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_V);
  action->setWhatsThis(tr("Triangulate a PLANAR graph  adding vertices"));

  action = augment->addAction(xmanIcon,tr("&ZigZag Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_ZZ);
  action->setWhatsThis(tr("Triangulate a PLANAR graph by adding edges"));

  action = augment->addAction(xmanIcon,tr("T&ricon. Triangulate")); 
  setId(action,A_AUGMENT_TRIANGULATE_3C);
  action->setWhatsThis(tr("Optimally triangulate a PLANAR graph by adding edges"));

  action = augment->addAction(xmanIcon,tr("Vertex &Quadrangulate")); 
  setId(action,A_AUGMENT_QUADRANGULATE_V);
  action->setWhatsThis(tr("Quadrangulate a PLANAR  bipartite graph"));

  augment->addSeparator();
  action = augment->addAction(xmanIcon,tr("&Bisect all edges")); 
  setId(action,A_AUGMENT_BISSECT_ALL_E);
  
  QMenu *remove = menuBar()->addMenu( tr("&Remove")); 
  action = remove->addAction(tr("&Isolated vertices")); 
  setId(action,A_REMOVE_ISOLATED_V);
  action = remove->addAction(tr("&Multiple edges")); 
  setId(action,A_REMOVE_MULTIPLE_E);
  action = remove->addAction(tr("Ist&hmus")); 
  setId(action,A_REMOVE_BRIDGES);
  remove->addSeparator();
  action = remove->addAction(tr("Colored &vertices")); 
  setId(action,A_REMOVE_COLOR_V);
  action = remove->addAction(tr("Colored &edges")); 
  setId(action,A_REMOVE_COLOR_E);

  QMenu *embed = menuBar()->addMenu( tr("E&mbed")); 
  action = embed->addAction(xmanIcon,tr("&FPP Fary")); 
  action->setWhatsThis(tr(fpp_txt));
  setId(action,A_EMBED_FPP);
  action = embed->addAction(xmanIcon,tr("&Schnyder")); 
  action->setWhatsThis(tr(schnyder_txt));
  setId(action,A_EMBED_SCHNYDER_E);
  action = embed->addAction(xmanIcon,tr("Schnyder &V")); 
  action->setWhatsThis(tr(schnyder_txt));
  setId(action,A_EMBED_SCHNYDER_V);
  action = embed->addAction(xmanIcon,tr("Convex Drawing")); 
  action->setWhatsThis(tr(cd_txt));
  setId(action,A_EMBED_CD);
  action = embed->addAction(xmanIcon,tr("Convex Compact Drawing")); 
  action->setWhatsThis(tr(ccd_txt));
  setId(action,A_EMBED_CCD);
  embed->addSeparator();
  action = embed->addAction(tr("&Tutte")); 
  setId(action, A_EMBED_TUTTE);
  embed->addSeparator();
  action = embed->addAction(xmanIcon,tr("Tutte &Circle")); 
  action->setWhatsThis(tr(tutte_circle_txt));
  setId(action,A_EMBED_TUTTE_CIRCLE);
  embed->addSeparator();
#ifdef VERSION_ALPHA
  action = embed->addAction(tr("Double Occurrence (&DFS)")); 
  setId(action, A_EMBED_POLREC_DFS);
#endif
  action = embed->addAction(tr("Double Occurrence (&LR DFS)")); 
  setId(action, A_EMBED_POLREC_DFSLR);  
  action = embed->addAction(tr("Double Occurrence (&BFS)")); 
  setId(action, A_EMBED_POLREC_BFS);
  //#ifdef VERSION_ALPHA
  action = embed->addAction(tr("Double Occurrence Circular (BFS)")); 
  setId(action, A_EMBED_POLAR);
  //#endif
  embed->addSeparator();
  action = embed->addAction(tr("&Visibility"));   setId(action,A_EMBED_VISION);
  action = embed->addAction(tr("FPP Visibility"));   setId(action,A_EMBED_FPP_RECTI);
  action = embed->addAction(tr("&General Visibility"));   setId(action,A_EMBED_GVISION);
  embed->addSeparator();
  action = embed->addAction(tr("&T Contact"));   setId(action,A_EMBED_T_CONTACT);
  action = embed->addAction(tr("&R Triangle Contact"));   setId(action,A_EMBED_TRIANGLE);
  action = embed->addAction(tr("&Contact (Biparti)"));   setId(action,A_EMBED_CONTACT_BIP);
  action = embed->addAction(tr("&2-Pages (Biparti)"));   setId(action,A_EMBED_BIP_2PAGES);
  embed->addSeparator();
  action = embed->addAction(tr("&Polyline"));   setId(action,A_EMBED_POLYLINE);
  action = embed->addAction(tr("&Curves"));   setId(action,A_EMBED_CURVES);
  embed->addSeparator();
  action = embed->addAction(xmanIcon,tr("Spring (Map Preserving)")); 
  action->setWhatsThis(tr(springPM_txt));  
  setId(action,A_EMBED_SPRING_PM);
#ifdef VERSION_ALPHA
  action = embed->addAction(xmanIcon,tr("Spring Planar")); 
  action->setWhatsThis(tr(jacquard_txt));  
  setId(action,A_EMBED_JACQUARD);
#endif
  //#ifdef VERSION_ALPHA
  action = embed->addAction(xmanIcon,tr("Spring")); 
  action->setWhatsThis(tr(spring_txt));  
  setId(action,A_EMBED_SPRING);
  //#endif
  embed->addSeparator(); 
  action = embed->addAction(xmanIcon,tr("Embedding in Rn")); 
  action->setWhatsThis(tr(embed3d_txt));  
  setId(action,A_EMBED_3d);
  action = embed->addAction(xmanIcon,tr("Schnyder in R3")); 
  action->setWhatsThis(tr(embed3dSchnyder_txt));  
  setId(action,A_EMBED_3dSCHNYDER);
 
  QMenu *dual = menuBar()->addMenu(tr("&Dual/Angle")); 
  action = dual->addAction(tr("&Dual"));   setId(action,A_GRAPH_DUAL);
  action = dual->addAction(xmanIcon,tr("Geometric Dual")); 
  action->setWhatsThis(tr(dual_g_txt));  
  setId(action,A_GRAPH_DUAL_G);
  dual->addSeparator();
  action = dual->addAction(tr("Angle"));   setId(action,A_GRAPH_ANGLE);
  action = dual->addAction(xmanIcon,tr("Geometric Angle")); 
  action->setWhatsThis(tr(angle_g_txt));  
  setId(action,A_GRAPH_ANGLE_G);

  QMenu *algo = menuBar()->addMenu(tr("&Algo")); 
  action = algo->addAction(tr("Find &Kuratowski"));   setId(action,A_ALGO_KURATOWSKI);
  action = algo->addAction(tr("Find &Cotree Critical"));   setId(action, A_ALGO_COTREE_CRITICAL);
  action = algo->addAction(tr("Color red  non critical edges"));   setId(action,A_ALGO_COLOR_NON_CRITIC);
  action = algo->addAction(tr("Max.Planar (simple graph) Fast"));   setId(action,A_ALGO_NPSET);
  action = algo->addAction(tr("&Max.Planar (simple graph) Slow"));   setId(action,A_ALGO_MAX_PLANAR);
  algo->addSeparator();
  action = algo->addAction(tr("Use &Geometric Map"));   setId(action,A_ALGO_GEOMETRIC_CIR);
  action = algo->addAction(tr("Use &LRALGO Map"));   setId(action,A_ALGO_LRALGO_CIR);
  algo->addSeparator();
  action = algo->addAction(tr("&Color everything"));   setId(action,A_ALGO_RESET_COLORS);
  action = algo->addAction(tr("Color &bipartite"));   setId(action,A_ALGO_COLOR_BIPARTI);
  action = algo->addAction(tr("Color current e&xterior face"));   setId(action,A_ALGO_COLOR_EXT);
  action = algo->addAction(tr("Color c&onnected components"));   setId(action,A_ALGO_COLOR_CONNECTED);
  algo->addSeparator();
  action = algo->addAction(tr("&Symmetry"));   setId(action,A_ALGO_SYM);
  algo->addSeparator();
  action = algo->addAction(tr("&Partition"));   setId(action, A_ALGO_NETCUT);
 
  QMenu *orient = menuBar()->addMenu(tr("&Orient")); 
  action = orient->addAction(tr("&Orient all edges"));   setId(action,A_ORIENT_E);
  action = orient->addAction(tr("&Unorient all edges"));   setId(action, A_ORIENT_NOE);
  action = orient->addAction(tr("&Color Poles"));   setId(action,A_ORIENT_SHOW);
  action = orient->addAction(tr("&ReOrient color edges"));   setId(action,A_ORIENT_SHOW);
  action = orient->addAction(tr("&Inf Orientation"));   setId(action,A_ORIENT_INF);
  action = orient->addAction(tr("Inf Orientation 3-connex"));   setId(action,A_ORIENT_TRICON);
  action = orient->addAction(tr("Inf Orientation bipartite"));   setId(action,A_ORIENT_BIPAR);
  action = orient->addAction(tr("Schnyder Orientation"));   setId(action,A_ORIENT_SCHNYDER);
  action = orient->addAction(tr("B&ipolarOrient Planar"));   setId(action,A_ORIENT_BIPOLAR);
  action = orient->addAction(tr("BipolarOrient"));   setId(action,A_ORIENT_BIPOLAR_NP);
  action = orient->addAction(tr("BFS Orientation"));   setId(action,A_ORIENT_BFS);

  QMenu *generate = menuBar()->addMenu(tr("&Generate")); 
  QMenu *outer    = generate->addMenu(tr("&Outer Planar"));
  action = outer->addAction(tr("&Outer Planar (N1)"));   setId(action, A_GENERATE_P_OUTER_N);
  action = outer->addAction(tr("O&uter Planar (N1,M))"));   setId(action,A_GENERATE_P_OUTER_NM);
  QMenu *plan    = generate->addMenu(tr("&Planar"));
  action = plan->addAction(tr("connected (M)"));   setId(action,A_GENERATE_P);
  action = plan->addAction(tr("2-connected (M)"));   setId(action,A_GENERATE_P_2C);
  action = plan->addAction(tr("3-connected (M))"));   setId(action,A_GENERATE_P_3C);
  QMenu *cubic    = generate->addMenu(tr("Planar &cubic"));
  action = cubic->addAction(tr("2-connected (M)"));   setId(action,A_GENERATE_P_3R_2C);
  action = cubic->addAction(tr("3-connected (M)"));   setId(action,A_GENERATE_P_3R_3C);
  action = cubic->addAction(tr("dual:4-connected (M)"));   setId(action,A_GENERATE_P_3R_D4C);
  QMenu *four = generate->addMenu(tr("Planar &4-regular"));
  action = four->addAction(tr("4-regular 2-connected (M)"));   setId(action,A_GENERATE_P_4R_2C);
  action = four->addAction(tr("4-regular 3-connected (M)"));   setId(action,A_GENERATE_P_4R_3C);
  action = four->addAction(tr("4-regular bipartite (M)"));   setId(action,A_GENERATE_P_4R_BIP);
  QMenu *bip = generate->addMenu(tr("Planar &bipartite"));
  action = bip->addAction(tr("Bipartite (M)"));   setId(action,A_GENERATE_P_BIP);
  action = bip->addAction(tr("Bipartite cubic 2-connected (M)"));   setId(action,A_GENERATE_P_BIP_2C);
  action = bip->addAction(tr("Bipartite cubic 3-connected (M)"));   setId(action,A_GENERATE_P_BIP_3C);
  generate->addSeparator();
  action = generate->addAction(tr("&Grid (N1,N2)"));   setId(action,A_GENERATE_GRID);
  generate->addSeparator();
  action = generate->addAction(tr("&Complete (N1)"));   setId(action, A_GENERATE_COMPLETE);
  action = generate->addAction(tr("&Bipartite complete (N1,N2)"));   setId(action,A_GENERATE_COMPLETE_BIP);
  generate->addSeparator();
  action = generate->addAction(tr("&Random (N1,M)"));   setId(action,A_GENERATE_RANDOM);

  QMenu *macro = menuBar()->addMenu(tr("&Macro")); 
  connect(macro,SIGNAL(triggered(QAction *)),SLOT(macroHandler(QAction *)));
  action = macro->addAction(tr("Start recording"));action->setData(1);
  action = macro->addAction(tr("Stop  recording"));action->setData(2);
  action = macro->addAction(tr("Continue recording"));action->setData(3);
  macro->addSeparator();
  action = macro->addAction(tr("Display Macro"));action->setData(6);
  action = macro->addAction(tr("Save Macro"));action->setData(7);
  action = macro->addAction(tr("Read Macro"));action->setData(8);
  macro->addSeparator();
  action = macro->addAction(tr("Insert a Pause"));action->setData(5);
  action = macro->addAction(tr("Repeat macro"));action->setData(4);

  userMenu = menuBar()->addMenu(tr("&User menu")); 
  action = userMenu->addAction(tr("Test &1"));   setId(action,A_TEST_1);
  action = userMenu->addAction(tr("Test &2"));   setId(action,A_TEST_2);
  action = userMenu->addAction(tr("Test &3"));   setId(action,A_TEST_3);

  QMenu *set = menuBar()->addMenu(tr("&Settings")); 
  QMenu *profile = set->addMenu(tr("&Pigale colors")); 
  profile->addAction(tr("&Edit Pigale Colors"),this,SLOT(EditPigaleColors()));
  profile->addAction(tr("&Gray profile"),this,SLOT(SetPigaleColorsProfile1()));
  profile->addAction(tr("&Yellow profile"),this,SLOT(SetPigaleColorsProfile2()));
  set->addAction(tr("Set a font"),this,SLOT(SetPigaleFont()));
  set->addAction(tr("&Documentation path"),this,SLOT(SetDocumentationPath()));
  set->addAction(tr("&Save Settings"),this,SLOT(SaveSettings()));
  set->addAction(tr("&Reset Settings"),this,SLOT(ResetSettings()));

  QMenu *help = menuBar()->addMenu(tr("&Information")); 
  QAction *helpAct =  QWhatsThis::createAction(); 
  help->addAction(infoIcon,tr("&Graph properties"),this,SLOT(computeInformation()),Qt::SHIFT+Qt::Key_F2);
  help->addSeparator();
  help->addAction(helpAct);
  help->addSeparator();
  help->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
  help->addAction(tr("&About"), this, SLOT(about()),Qt::Key_F1);

  initMenuTest();
  }
void pigaleWindow::closeEvent(QCloseEvent *event)
  {if(server)
      {server->close();
      delete server;
      }
  pigaleThread->terminate();pigaleThread->wait();
  //pigaleThread->deleteLater();
  UndoErase();
  LogPrintf("END\n");
  delete printer;
  event->accept();
  }
void pigaleWindow::AllowAllMenus()
  {QMapIterator<int,QAction *> i(menuIntAction);
    while(i.hasNext()) 
        {i.next();
        i.value()->setVisible(true);
        i.value()->setEnabled(true);
        }
  }
void  pigaleWindow::setUserMenu(int i, const QString &txt)
 {QList<QAction *> list = userMenu->actions();
 list.at(i-1)->setText(txt);
 }
void pigaleWindow::seedEdited(const QString & t)
  {bool ok;
  long seed = t.toLong(&ok);
  if(ok)randomSetSeed() = seed; 
  }
void pigaleWindow::spinN1Changed(int val)
  {staticData::Gen_N1 = val;}
void pigaleWindow::spinN2Changed(int val)
  {staticData::Gen_N2 = val;}
void pigaleWindow::spinMChanged(int val)
  {staticData::Gen_M = val;}
void pigaleWindow::spinNChanged(int val)
  {staticData::nCut = val;}
void pigaleWindow::spinPNGChanged(int val)
  {staticData::sizeImage = val;}
void pigaleWindow::spinDelayChanged(int val)
  {staticData::macroDelay = val;}
void pigaleWindow::spinMaxNSChanged(int i)
  {staticData::MaxNS = i;}
void pigaleWindow::spinMaxNDChanged(int i)
  {staticData::MaxND = i;}
void pigaleWindow::spinRepeatChanged(int i)
  {staticData::macroRepeat = i;}  
void  pigaleWindow::distOption(int use)
  {staticData::UseDistance() = use;
  }
void  pigaleWindow::setShowOrientation(bool val)
  {staticData::ShowOrientation() = val;
  chkOrient->setCheckState(staticData::ShowOrientation() ? Qt::Checked : Qt::Unchecked);
  }
void pigaleWindow::SetPigaleFont()
 {bool ok;
  QFont font = QFontDialog::getFont( &ok, this->font(), this );
  if(!ok)return;
  QApplication::setFont(font);
  setFont(font);
 }
void pigaleWindow::showLabel(int show)
  {int _show = staticData::ShowVertex();
  staticData::ShowVertex() = show -3;
  if(staticData::ShowVertex() != _show && GC.nv())
      {switch(tabWidget->currentIndex())
          {case 0:
              gw->editor->update(1);
              break;
          case 1:
              mypaint->update();
              break;
          }
      }
  }
void pigaleWindow::about()
  {if(sizeof(void*)==8)
      QMessageBox::about(this,tr("Pigale Editor"), 
                      "<b>"+tr("Pigale Editor")+"</b> (version:  "+PACKAGE_VERSION+")"
                      "<br><b>version 64 bits</b>"
                      "<br><b>Copyright (C) 2001</b>"
                      +"<br>Hubert de Fraysseix"
	    +"<br>Patrice Ossona de Mendez "
	    +"<br> See <em>license.html</em>");
	else
	      QMessageBox::about(this,tr("Pigale Editor"), 
                      "<b>"+tr("Pigale Editor")+"</b> (version:  "+PACKAGE_VERSION+")"
                      "<br><b>version 32 bits</b>"
                      "<br><b>Copyright (C) 2001</b>"
                      +"<br>Hubert de Fraysseix"
	    +"<br>Patrice Ossona de Mendez "
	    +"<br> See <em>license.html</em>"); 
  }
void pigaleWindow::aboutQt()
  {QMessageBox::aboutQt(this,"Qt Toolkit");
  }
bool pigaleWindow::InitPrinter(QPrinter* printer)
  {if(ServerExecuting)
      {QString FileName = QString("/tmp/server%1.ps").arg(ServerClientId);
      printer->setOrientation(QPrinter::Portrait);
      printer->setColorMode(QPrinter::Color);
      printer->setOutputFileName(FileName);
      }
  else 
      {QPrintDialog printDialog(printer,this);
      if(printDialog.exec() != QDialog::Accepted) return false;
       //QString FileName = staticData::dirImage + QDir::separator() + "image.pdf";   
      //printer->setOutputFileName(FileName);    
      //QString OutputFileName = printer->outputFileName();
      //if(!OutputFileName.isNull())staticData::dirImage = QFileInfo(OutputFileName).absolutePath();
      }
  return true;
  }
void pigaleWindow::print()
  {switch(tabWidget->currentIndex())
      {case 0:
          if(!InitPrinter(printer))return;
          gw->editor->print(printer);
          break;
      case 1:
          if(!InitPrinter(printer))return;
          mypaint->print(printer);
          break;
      case 2:
          if(!InitPrinter(printer))return;
          graphgl->print(printer);
          break;
      case 3:
          if(!InitPrinter(printer))return;
          graphsym->print(printer);
          break;
      default:
          break;
      }
  }
bool pigaleWindow::InitPicture(QString & formats,QString & suffix)
  {if(ServerExecuting)
      {staticData::fileImage = QString("/tmp/server%1.png").arg(ServerClientId);
      suffix = "png";
      return true;
      }
  QString filter ="*.*";
  staticData::fileImage = QFileDialog::getSaveFileName(this,
                                                       tr("Choose a file to save under"),
                                                       staticData::fileImage,
                                                       formats,&filter);

  if(staticData::fileImage.isEmpty())return false; 
  staticData::dirImage = QFileInfo(staticData::fileImage).absolutePath();
  suffix = QFileInfo(staticData::fileImage).suffix();

  // construct lis of allowed suffixes
  QStringList suffixes = formats.split(" ");
  for(int i = 1; i < suffixes.count();i++)
      {QString str = suffixes[i].mid(suffixes[i].indexOf('.')+1);
      str = str.left(str.indexOf(')'));
      if(str == suffix)
          {if(suffix == "pdf" || suffix == "ps")
              {printer->setOrientation(QPrinter::Portrait);
              printer->setColorMode(QPrinter::Color);
              printer->setOutputFileName(staticData::fileImage);
              }
          return true;
          }
      }
  QString msg = "Unknown extension: " + suffix;
  QMessageBox::warning(this,"Pigale Editor",msg);
  return false;
  }
void pigaleWindow::image()
//{QString formats = "Png (*.png);;Jpeg (*.jpg);;Svg (*.svg);;Ps (*.ps);;Pdf (*.pdf)";
  {QString formats = "Image Files (*.png *.jpg *.svg *.ps *.pdf)";
  QString suffix;
  switch(tabWidget->currentIndex())
      {case 0:
          if(!InitPicture(formats,suffix))return;
          gw->editor->image(printer,suffix);
          break;
      case 1:
          if(!InitPicture(formats,suffix))return;
           mypaint->image(printer,suffix);
          break;
      case 2:
          if(!InitPicture(formats,suffix))return;
          graphgl->image(printer,suffix);
          break;
      case 3:
          if(!InitPicture(formats,suffix))return;
          graphsym->image(printer,suffix);
          break;
      default:
          break;
      }
  }
