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
\file Macro.cpp
\ingroup pigaleWindow
\brief Macro
*/

#include "pigaleWindow.h"
#include "GraphWidget.h"
#include "mouse_actions.h" 
#include "gprop.h"
#include <QT/Misc.h> 
#include <QT/Action_def.h>  
#include <TAXI/Tgf.h> 

#include <QApplication>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QToolBar>

class eventEater : public QObject
{public:
  eventEater(){};
  ~eventEater(){};
protected:
  bool eventFilter(QObject *o,QEvent *e);
};

bool eventEater::eventFilter(QObject *,QEvent *e)
  {if(e->type() == QEvent::MouseButtonPress) return true; // eat event
    return false;
  }

void pigaleWindow::keyPressEvent(QKeyEvent *k)
  {_key = k->key();
  if(_key == Qt::Key_Escape)MacroLooping = MacroExecuting = MacroWait = false;
  }
int pigaleWindow::getKey()
  {int key0 = _key;
  _key = 0;
  return key0;
  }
void pigaleWindow::blockInput(bool t)
  {static bool _inputBlocked = false;
  if(!t && (ServerExecuting || MacroLooping))return;
  if(_inputBlocked == t)return;
  _inputBlocked = t;
  menuBar()->setDisabled(t); 
  tb->setDisabled(t); 
  static eventEater *EventEater = 0;
  if(!EventEater)EventEater = new eventEater();
  if(t)
      {qApp->installEventFilter(EventEater);
      qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
      }
  else 
      {qApp->removeEventFilter(EventEater);
      qApp->restoreOverrideCursor();
      }
  }
void pigaleWindow::timerWait()
  {MacroWait = false;
  }
void pigaleWindow::wait(int millisec)
  {MacroWait = true;
  QTimer::singleShot(millisec,this,SLOT(timerWait()));
  do
      {qApp->processEvents();
      }while(MacroWait);
  }

/*
void macroDefColors(int record)
  {GeometricGraph G(GC);
  G.ecolor.definit(MacroEcolor[record]); 
  G.vcolor.definit(MacroVcolor[record]); 
  G.ewidth.definit(MacroEwidth[record]);
  }
*/
int pigaleWindow::macroLoad(QString FileName)
  {if(FileName.isEmpty())return -1;
  if(QFileInfo(FileName).isReadable() == false)return -1;
  QFile file( FileName);
  file.open(QIODevice::ReadOnly);
  QTextStream stream(&file);
  QString str = stream.readLine();
  
  if(str == "Macro Version:1")
      stream.readLine();
  else if(str != "Macro Version:2")
      {Tprintf("Wrong Macro File -%s-",(const char *)str.toLatin1());return -1;}
  if(stream.atEnd())return -1;
  MacroNumActions = 0;
  int action;
  while(!stream.atEnd())
      {str = stream.readLine();
      action = getActionInt(str);
      if(action < 99 || action > A_TEST_END)
          {Tprintf("Unknown action:%s",(const char *)str.toLatin1());continue;}
      Tprintf("Action (%d):%s",MacroNumActions,(const char *)str.toLatin1());
      MacroActions(++MacroNumActions) = action;
      }
  return 0;
  }

void pigaleWindow::macroRecord(int action)
  {if(action > A_SERVER)return;
  MacroActions(++MacroNumActions) = action;
  QString str_action = getActionString(action);
  Tprintf("Recording action (%d):%s",MacroNumActions,(const char *)str_action.toLatin1());
//   GeometricGraph G(GC);
//   short ecol;  G.ecolor.getinit(ecol); MacroEcolor(MacroNumActions) = ecol;
//   short vcol;  G.vcolor.getinit(vcol); MacroVcolor(MacroNumActions) = vcol;
//   int width;   G.ewidth.getinit(width);MacroEwidth(MacroNumActions) = width;
  }

void pigaleWindow::macroHandler(QAction *qaction)
  {//int repeat = macroLine->getVal();
  int repeat = staticData::macroRepeat;
  int i;
  unsigned j;
  double Time;
  QTime t0;
  QString msg0,msg1;
  int repeat0,record;
  bool _debug = debug();
  int event = getId(qaction);
  showInfoTab();
  switch(event)
      {case 1://start recording
          if(debug())LogPrintf("\nRecord macro\n");
          AllowAllMenus();
          postMessageClear();
          Tprintf("No info while recording");
          MacroRecording = true;
          MacroWait = false;
          MacroNumActions = 0;
          break;
      case 2://stop recording
          MacroRecording = false;
          break;
      case 3://continue recording
          MacroRecording = true;
          break; 
      case 4:// play repeat times
          MacroRecording = false;
          MacroWait = false;
          setFocus(); 
          postMessageClear();
          DebugPrintf("PLAY times=%d MacroNumActions:%d",repeat,MacroNumActions);
          t0.start();
#ifndef _WINDOWS
          msg0 = QString("Macro started at %1").arg(t0.toString(Qt::TextDate));
#else
          msg0 = "Macro started";
#endif
          DebugPrintf("%s",(const char *)msg0.toLatin1());
          MacroLooping = true;
          if(repeat == 0)debug() = false;
          else if(debug())
              {if(QMessageBox::question(this,"Pigale Editor"
                                       ,tr("Stop log debugging information ?")
                                       ,QMessageBox::Ok 
                                       ,QMessageBox::Cancel)  != QMessageBox::Cancel)
                  debug() = false;
              }
          t0.restart();
          repeat0 = (repeat == 0) ? 1000 : repeat;
          progressBar->setRange(0,repeat0);
          progressBar->setValue(0);
          progressBar->show();
          j = 0;
          for(i = 1;i <= repeat0;i++)
              {if(i == repeat0 && repeat == 0)i = 1;
              ++j;
              macroPlay();
              progressBar->setValue(i);
              qApp->processEvents();
              if(!MacroLooping)break;
              }
          progressBar->hide();
          Time = t0.elapsed()/1000.;
          t0.restart();
          if(EditNeedUpdate)gw->editor->update(-1);
          if(InfoNeedUpdate){information();InfoNeedUpdate = false;}
          MacroLooping = false;
#ifndef _WINDOWS
          msg1 = QString("Macro stopped at %1").arg(t0.toString(Qt::TextDate));
#else
          msg1 ="Macro stopped"; 
#endif
          DebugPrintf("Ellapsed time:%.3f mean:%f",Time,Time/j);
          Tprintf("%s",(const char *)msg0.toLatin1());
          DebugPrintf("%s",(const char *)msg1.toLatin1());
          if(!getPigaleError())
              DebugPrintf("END PLAY OK iter:%d",j);
          else
              DebugPrintf("END PLAY ERROR iter=%d",j);
          blockInput(false);
          break;
      case 5:// insert a pause
          if(MacroRecording)macroRecord(A_PAUSE);
          break;
      case 6:// display
          postMessageClear();
          for(record = 1;record <= MacroNumActions;record++)
              Tprintf("Action (%d/%d):%s",record,MacroNumActions
                       ,(const char *)getActionString(MacroActions[record]).toLatin1());
          break;
      case 7://save
          {QString FileName = QFileDialog::getSaveFileName(this
                                                           ,tr("Choose a filename to save under")
                                                           ,DirFileMacro
                                                           ,"Macros (*.mc)");
          if(FileName.isEmpty())break;
          if(QFileInfo(FileName).suffix() != (const char *)"mc")
              {FileName += (const char *)".mc";
              QFileInfo fi = QFileInfo(FileName);
              if(fi.exists())
                  {if(QMessageBox::warning(this,"Pigale Editor"
                                           ,"This file already exixts.<br>"
                                           "Overwrite ?"
                                           ,QMessageBox::Ok 
                                           ,QMessageBox::Cancel) == QMessageBox::Cancel)break;
                  }
              }
          DirFileMacro = QFileInfo(FileName).absolutePath();
          FILE *out = fopen((const char *)FileName.toLatin1(),"wt");
          fprintf(out,"Macro Version:2\n");
          for(record = 1;record <= MacroNumActions;record++)
              fprintf(out,"%s\n",(const char *)getActionString(MacroActions[record]).toLatin1());
          fclose(out);
          }
          break;
      case 8:// read
          {QString FileName = QFileDialog:: getOpenFileName(this
                                                            ,tr("Choose a file to open")
                                                            ,DirFileMacro
                                                            ,"Macro files(*.mc)");
          postMessageClear();
          DirFileMacro = QFileInfo(FileName).absolutePath();
          if(macroLoad(FileName) == -1)break;
          MacroWait = false;
          }
          break;
      default:
          break;
      }
  debug() = _debug;
  }

void pigaleWindow::macroPlay(bool start)
// start = true if pigale was launch with a macro
  {if(MacroNumActions == 0)return;
  blockInput(true);
  if(!MacroLooping){postMessageClear();DebugPrintf("Play macro:%d actions",MacroNumActions);}
  int ret_handler = 0,action;
  EditNeedUpdate =  InfoNeedUpdate = true;
  MacroExecuting = true;
  MacroRecording = MacroWait = false;
  
  // Load next graph if no generator is called
  if((!start && MacroActions[1] < A_GENERATE) || (MacroActions[1] >  A_GENERATE_END))
      {load(1); EditNeedUpdate =  InfoNeedUpdate = false;}

  for(int record = 1;record <= MacroNumActions;++record)
      {action = MacroActions[record];
      if(action != A_PAUSE && action < A_TEST && !graph_properties->actionAllowed(action))
          {if(debug())
              {DebugPrintf("%s:initial conditons not satisfied\n"
                           ,(const char *)getActionString(action).toLatin1());
              InfoNeedUpdate = false; // do not hide the error message 
              blockInput(false);
              return;
              }
          ++record;continue;
          }
      if(debug())LogPrintf("macro action:%s\n",(const char *)getActionString(action).toLatin1());
      if(!MacroExecuting)break;
      // Execute the macro
      if(action ==  A_PAUSE)
          {wait(100*staticData::macroDelay);          
          // update the editor and information
          if(record != MacroNumActions)
              {if(EditNeedUpdate)
                  {gw->editor->update(1);EditNeedUpdate = false;}
              if(InfoNeedUpdate)
                  {information();InfoNeedUpdate = false;}
              }
          continue;
          }

      handler(action);
      wait(100);
      while((ret_handler = getResultHandler()) == -999)
          qApp->processEvents();
      if(ret_handler == 0)
          InfoNeedUpdate = EditNeedUpdate = false;
      else if(ret_handler == 1)
          {InfoNeedUpdate = false;EditNeedUpdate = true;}
      else if(ret_handler == 2)
          InfoNeedUpdate = EditNeedUpdate = true;
      else if(ret_handler == 7 || ret_handler == 8)
          EditNeedUpdate = false;
      if(getPigaleError())
          {DebugPrintf("MACRO %s",(const char *)getPigaleErrorString().toLatin1());
          setPigaleError();
          MacroWait = MacroLooping = false;
          InfoNeedUpdate = false; // do not hide the error message
          break;
          }
      }//end for

  MacroExecuting = false;
  if(!MacroLooping)
      {if(EditNeedUpdate)
          {gw->editor->update(-1);EditNeedUpdate = false;}
      if(InfoNeedUpdate)
          {information(false);InfoNeedUpdate = false;}
      blockInput(false);
      }
  }
