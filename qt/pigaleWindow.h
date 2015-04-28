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
\defgroup pigaleWindow Class pigaleWindow
\brief Main window of Pigale
*/
/*!
\file pigaleWindow.h
\ingroup pigaleWindow
\brief Main window of Pigale
*/

#ifndef MYWINDOW_H 
#define MYWINDOW_H

#include <QApplication>
#include <QPrinter>
#include <QFileDialog>
#include <QPrintDialog>
#include <QTabWidget>
#include <QThread>
#include <QtNetwork>
#include <QCloseEvent>
#include <QMutex>
#include <QWaitCondition>
#include <QAction>
#include <QMainWindow>
#include <QDateTime>
#include <QString>
#include <QMap>
#include <QEvent>
#include <QTimer>
#include <QSettings>
#include <QTextStream>


#include <Pigale.h> 
#include <QT/staticData.h>

class QHBoxLayout;
class QTextEdit;
class QTextBrowser;
class pigaleWindow;
class GraphWidget;
class GraphGL;
class GraphSym;
class Graph_Properties;
class Mouse_Actions;
class pigalePaint;
class ClientSocket;
class QTabWidget;
class QSpinBox;
class QCheckBox;
class QToolButton;
class QToolBar;
class QPrinter;
class LineEditNum;
class QProgressBar;
class QLineEdit;
class QPalette;
class PigaleServer;

/*! 
\class PigaleThread
\brief Main non GUI thread that executes all algorithms.

When an algorithm has finished its execution, the thread post an event to the GUI thread signalling  
what graphic operations need to be done. 
*/
class PigaleThread : public QThread 
{Q_OBJECT

private:
  QMutex mutex;
  QWaitCondition condition;
  bool abort;
  int previous_action;
  int action; 
  int N,N1,N2,M;
  int delay;
public:
  PigaleThread(QObject *parent);
  ~PigaleThread();
  void stop();
  void run(int action,int N = 0,int N1 = 0,int N2 = 0,int M = 0,int delay = 0);
  pigaleWindow *mw;
signals:
  void handlerSignal(int ret,int drawingType,int saveType);
protected:
  void run();
};

//! Main class of Pigale which constructs all the menus and widgets
class pigaleWindow: public QMainWindow 
{Q_OBJECT

private slots:
  void load();
  int load(int pos);
  void previous();
  void reload();
  void next();
  int save(bool askTitle = true);
  void saveAs();
  void deleterecord();
  void switchInputOutput();
  void macroHandler(QAction *);
  void macroPlay(bool start=false);
  void about();
  void aboutQt();
  void showLabel(int action);
  void distOption(int use);
  void Undo();
  void Redo();
  void UndoClear();
  void UndoEnable(bool enable);
  void LoadSettings();
  void SetPigaleFont();
  void settingsHandler(int action);
  void initServer();
  void seedEdited(const QString & t);
  void spinN1Changed(int val);
  void spinN2Changed(int val);
  void spinMChanged(int val);
  void spinNChanged(int val);
  void spinRepeatChanged(int val);
  void spinDelayChanged(int val);
  void spinPNGChanged(int val);
  void spinMaxNSChanged(int i);
  void spinMaxNDChanged(int i);
  void EditPigaleColors();
  void SetPigaleColorsProfile1(); 
  void SetPigaleColorsProfile2(); 
  void SaveSettings();
  void ResetSettings();
  void SetDocumentationPath();
  void postHandler(int ret,int drawingType,int saveType);
private:
  void createThread();
  void initPigale();
  void CheckDocumentationPath(); 
  void createMenus();
  void createToolBar();
  void createLayout(QWidget *mainWidget);
  void createRightLayout(QHBoxLayout * leftLayout);
  void createPageInfo(QWidget *gInfo);
  void createPageSettings(QWidget *gSettings,QHBoxLayout * leftLayout);
  int setId(QAction *action,int Id);
  void AllowAllMenus();
  void mapActionsInit();
  int  macroLoad(QString FileName);
  void macroRecord(int action);
  void ParseArguments(); 
  void Message(QString s);
  void UndoInit();
  void initMenuTest();
  void UpdatePigaleColors();
  bool InitPrinter(QPrinter* printer);
  bool InitPicture(QString & formats,QString & suffix);
public slots:
  void handler(QAction *action);
  void banner();
  void timerWait();
  void UndoSave();
  void computeInformation();
  void information(bool erase = true);
  void print();
  void image();
  int  handler(int action);
  void NewGraph();
protected:
  void closeEvent(QCloseEvent *event);
  void customEvent( QEvent * e );
  void keyPressEvent(QKeyEvent *k);
public:
  pigaleWindow();
  ~pigaleWindow(){};
  void whenReady();
  void showInfoTab();
  void postMessage(const QString &msg);
  void postWait(const QString &msg);
  void postMessageClear();
  void postDrawG();
  void wait(int millisec);
  void UndoTouch(bool save=false);
  void blockInput(bool t);
  int getKey();
  QString getActionString(int action);
  int getActionInt(QString action_str);
  int publicLoad(int pos);
  int publicSave(QString filename);
  void setUserMenu(int i, const QString &txt);
  void setShowOrientation(bool val);
  int &  getResultHandler();
  int getId(QAction *action);
public:
  QActionGroup *menuActions;
  QMap<int,QAction*> menuIntAction;
  QAction *undoLAct,*undoRAct,*undoSAct; 
  QProgressBar *progressBar;
  pigalePaint *mypaint;
  QTabWidget *tabWidget;
  GraphWidget *gw;
  GraphGL *graphgl;
  GraphSym *graphsym;
  Graph_Properties *graph_properties;
  Mouse_Actions *mouse_actions;
  GraphContainer GC;
  QPalette LightPalette;
  QString InputFileName;
  QString OutputFileName;
  int InputDriver;
  int OutputDriver;
  bool ServerExecuting;
  int ServerClientId;
  QTime timer;
  ClientSocket *threadServer;
  int GraphIndex1;
  int pigaleThreadRet;
private:
  PigaleThread *pigaleThread;
  PigaleServer  *server;
  QTextEdit *messages;
  QToolBar *tb;
  QTextBrowser *browser;
  QTabWidget *rtabWidget;
  QCheckBox *chkOrient;
  QWidget *gInfo,*gSettings;
  QMenu *userMenu; 
  QPrinter *printer;
  QString DirFileMacro;
  QString MacroFileName;
  int *pGraphIndex,GraphIndex2,UndoIndex,UndoMax;
  QString DirFileDoc;
  svector<int> MacroActions;
  bool EditNeedUpdate,InfoNeedUpdate;
  int MacroNumActions;
  bool MacroRecording,MacroLooping,MacroExecuting,MacroWait,MacroPlay;
  int _key;
  bool Server;
  int numMessages;
  typedef QMap<int,QString> IntStringMap;
  IntStringMap mapActionsString;
  typedef QMap<QString,int> StringIntMap;
  StringIntMap mapActionsInt;   
  int drawingType;
};


#endif
