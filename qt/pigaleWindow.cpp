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
\file pigaleWindow.cpp
\ingroup pigaleWindow
\brief Events and Threads
*/

#include <config.h>
#include "ClientSocket.h"
#include "pigaleWindow.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"

#include <TAXI/Tgf.h>
#include <QT/Misc.h> 
#include <QT/Handler.h>
#include <QT/Action_def.h>
#include <QT/Action.h>
#include <QT/pigalePaint.h> 
#include <QT/clientEvent.h>

#include <QProgressBar>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QMessageBox>
#include <QStatusBar>

int Test(GraphContainer &GC,int action,int & drawing);
void UndoErase();

void pigaleWindow::createThread()
  {pigaleThread = new PigaleThread(this);
  pigaleThread->mw = this;
  connect(pigaleThread,SIGNAL(handlerSignal(int,int,int)),this,SLOT(postHandler(int,int,int)));
  }
void pigaleWindow::initServer()
  {ServerClientId = 0;
  quint16 port;
  NewGraph(); 
#ifdef _WIN32
  QDir tmp = QDir("/tmp");
  if(!tmp.exists())QDir("/").mkdir("tmp");
#endif
  if(!server)
      {server = new PigaleServer(this);
      server->setProxy(QNetworkProxy::NoProxy);
      threadServer = 0;
      if(!server->isListening ())
          {Tprintf("Server: Init failed");
          cout <<"Server: Init failed"<<endl;
          }
      else
          {port = server->serverPort();
          Tprintf("Server using port%d",port);
          menuIntAction[A_SERVER_INIT]->setText(tr("Close server"));
          showMinimized();
          }
      }
  else if(!server->isListening ())
      {server->listen(QHostAddress::Any,4242); 
      if(!server->isListening ())
          {Tprintf("Server: Init failed");
          cout <<"Server: Init failed"<<endl;
          }
      else
          {port = server->serverPort();
          Tprintf("Server using port%d",port);
          menuIntAction[A_SERVER_INIT]->setText(tr("Close server"));
          showMinimized();
          }
      }
  else
      {Tprintf("Server disconnected");
      server->close();
      menuIntAction[A_SERVER_INIT]->setText(tr("Init server"));
      }
  }
void pigaleWindow::customEvent(QEvent * ev)
  {ev->accept();
  int itype = ev->type();
  switch(itype)
      {case  TEXT_EVENT:
          {textEvent *event  =  (textEvent  *)ev;
          Message(event->getString());
          }
          break;
      case CLEARTEXT_EVENT:
          messages->setPlainText("");
          break;
      case BANNER_EVENT:
          {bannerEvent *event  =  (bannerEvent  *)ev;
          statusBar()->showMessage(event->getString());
          }
          break;
      case WAIT_EVENT:
          {waitEvent *event  =  (waitEvent  *)ev;
          QString msg = event->getString();
          int rep = QMessageBox::information (this,"Wait",msg,"","CANCEL","EXIT",0,0);
          if(rep == 2)close();
          }
      case DRAWG_EVENT:
          gw->editor->update(1);
          break;
      case READY_EVENT:
          whenReady();
          break;
      case PROGRESS_EVENT:
          {progressEvent *event  =  (progressEvent  *)ev;
          int action  = event->getAction();
          if(action == -1)
              progressBar->hide();
          else if(action == 1)
              {progressBar->setRange(0,event->getStep());
              progressBar->setValue(0);
              progressBar->show();
              }
          else
              progressBar->setValue(event->getStep());
          }
          break;
      default:
          //cout<<"main unknown event:"<<ev->type()-USER_EVENT<<endl;
          qDebug("UNKNOWN EVENT");
          break;
      }
  }
void pigaleWindow::banner()
  {QString msg;  
  int NumRecords =IO_GetNumRecords(0,(const char *)InputFileName.toLatin1());
  int NumRecordsOut =IO_GetNumRecords(0,(const char *)OutputFileName.toLatin1());
    msg.sprintf("Input: %s %d/%d  Output: %s %d Undo:%d/%d"
                ,(const char *)InputFileName.toLatin1()
                ,*pGraphIndex,NumRecords
                ,(const char *)OutputFileName.toLatin1()
                ,NumRecordsOut
                ,UndoIndex,UndoMax);
  bannerEvent *e = new bannerEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postMessage(const QString &msg)
  {if(++numMessages > 500|| MacroLooping)return;
  textEvent *e = new textEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postMessageClear()
  {clearTextEvent *e = new clearTextEvent();
  numMessages = 0;
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postWait(const QString &msg)
  {waitEvent *e = new waitEvent(msg);
  QApplication::postEvent(this,e);
  }
void pigaleWindow::postDrawG()
  {drawgEvent *e = new drawgEvent();
  QApplication::postEvent(this,e);
  }
void pigaleWindow::Message(QString s)
  {messages->append(s);
  messages->ensureCursorVisible();
  } 
void pigaleWindow::showInfoTab()
  {// index is 0
  rtabWidget->setCurrentIndex(rtabWidget->indexOf(gInfo));
  }
void pigaleWindow::mapActionsInit()
  {int na = (int)(sizeof(Actions)/sizeof(_Action));
  for(int i = 0;i < na;i++)
      {mapActionsString[Actions[i].num] = Actions[i].name;
      mapActionsInt[Actions[i].name] = Actions[i].num;
      }
  }
QString pigaleWindow::getActionString(int action)
  {return mapActionsString[action];
  }
int pigaleWindow::getActionInt(QString action_str)
  {return mapActionsInt[action_str];
  }
void pigaleWindow::computeInformation()
// only called manually from the menu
  {TopologicalGraph G(GC);
  if(G. Set(tvertex()).exist(PROP_NLOOPS))  G. Set(tvertex()).erase(PROP_NLOOPS);
  if(G.Set(tedge()).exist(PROP_MULTIPLICITY))  G.Set(tedge()).erase(PROP_MULTIPLICITY);
  if(G.Set(tedge()).exist(PROP_ORIENTED))  G.Set(tedge()).erase(PROP_ORIENTED);
  if(G.Set(tedge()).exist(PROP_REORIENTED))  G.Set(tedge()).erase(PROP_REORIENTED);
  if(G.Set().exist(PROP_SIMPLE))      G.Set().erase(PROP_SIMPLE);
  if(G.Set().exist(PROP_CONNECTED))  G.Set().erase(PROP_CONNECTED);
  if(G.Set().exist(PROP_BICONNECTED))  G.Set().erase(PROP_BICONNECTED);
  if(G.Set().exist(PROP_TRICONNECTED))  G.Set().erase(PROP_TRICONNECTED);
  if(G.Set().exist(PROP_BIPARTITE))  G.Set().erase(PROP_BIPARTITE);
  if(G.Set().exist(PROP_PLANAR))  G.Set().erase(PROP_PLANAR);
  if(G. Set().exist(PROP_NLOOPS))  G. Set().erase(PROP_NLOOPS);
  Prop1<int> maptype(G.Set(),PROP_MAPTYPE,PROP_MAPTYPE_UNKNOWN);
  G.planarMap() = 0;
  postMessageClear();setPigaleError(0);
  graph_properties->update(GC,true);
  }
void pigaleWindow::information(bool erase)
  {if(!getPigaleError() && erase &&  !MacroLooping  && !ServerExecuting)postMessageClear();
  graph_properties->update(GC,!MacroExecuting && !MacroLooping && !ServerExecuting);
  }
void pigaleWindow::settingsHandler(int action)
  {// called when a checkbox is clicked
  // or from the server
  switch(action)
      {case A_SET_DEBUG:
          debug() = !debug();
          return;
      case  A_SET_SCH_RECT:
          staticData::SchnyderRect() = !staticData::SchnyderRect();
          return;
      case A_SET_LFACE:
          staticData::SchnyderLongestFace() = !staticData::SchnyderLongestFace();
          return;
      case A_SET_SCH_COLOR:
          staticData::SchnyderColor() =!staticData::SchnyderColor();
          return;
      case A_SET_ERASE_MULT:
          staticData::RandomEraseMultipleEdges() = !staticData::RandomEraseMultipleEdges();
          return;
      case A_SET_GEN_CIR:
          staticData::RandomUseGeneratedCir() = !staticData::RandomUseGeneratedCir();
          return;
      case A_SET_RANDOM_SEED:
          staticData::RandomSeed() = !staticData::RandomSeed();
          return;
      case A_SET_UNDO:
          staticData::IsUndoEnable = !staticData::IsUndoEnable;
          UndoEnable(staticData::IsUndoEnable);
          return;
      case A_SET_ORIENT:
          staticData::ShowOrientation() = !staticData::ShowOrientation();
          break;
      case A_SET_ARROW:
          staticData::ShowArrow() = !staticData::ShowArrow();
          break;
      case A_SET_EXTBRIN:
          staticData::ShowExtTbrin() = !staticData::ShowExtTbrin();
          break;
      }
  // update the editor
  handlerEvent *e = new handlerEvent(1,0,0);
  QApplication::postEvent(this,e);
  return;
  }
#include <QWhatsThis>  
void pigaleWindow::handler(QAction *qaction)
  {int Id = getId(qaction);
  //if(QWhatsThis::inWhatsThisMode()){return;}
  if(Id)handler(Id);
  }
int pigaleWindow::handler(int action)
  {if(action < A_PAUSE)return 0;
  getResultHandler() = -999;
  if(MacroRecording)macroRecord(action);
  if(action == A_PAUSE)
      {qApp->processEvents();
      MacroWait = true;
      QTimer::singleShot(100*staticData::macroDelay,this,SLOT(timerWait()));
      return 0;
      }
  if(action < A_AUGMENT_END)
      {UndoSave();
      pigaleThread->run(action);
      }
  else if(action < A_EMBED_END)
      pigaleThread->run(action);
  else if(action < A_GRAPH_END)
      {UndoClear();UndoSave();
      pigaleThread->run(action);
      }
  else if(action < A_REMOVE_END)
      {UndoSave();
      pigaleThread->run(action);
      }
  else if(action < A_GENERATE_END)
      {UndoClear();UndoSave();
      pigaleThread->run(action,0,staticData::Gen_N1,staticData::Gen_N2,staticData::Gen_M);
      }
  else if(action < A_ALGO_END)
      pigaleThread->run(action,staticData::nCut);
  else if(action < A_ORIENT_END)
      pigaleThread->run(action);
  else if(action < A_TEST_END)
      pigaleThread->run(action);
//       {int drawing;
//       timer.start();
//       int ret = Test(GC,action - A_TEST,drawing);
//       handlerEvent *e = new handlerEvent(ret,drawing,0);
//       QApplication::postEvent(this,e);
//       return 0;
//       }
   else if(action > A_INPUT && action < A_INPUT_END)
       pigaleThread->run(action);
   else if(action > A_TRANS && action < A_TRANS_END)
       pigaleThread->run(action);
  else if(action > A_SET)
      {settingsHandler(action);return 0;}
  else
      return 0;
  return 0;
  }
void pigaleWindow::postHandler(int action,int drawingType,int saveType)
  {//action   0:(No-Redraw,No-Info) 1:(Redraw,No-Info) 2:(Redraw,Info) 20:(Redraw_nocompute,Info)
  // 3:(Drawing) 4:(3d) 5:symetrie 6-7-8:Springs Embedders
  //10:png to client
  //Tprintf("action:%d drawing:%d",action,drawingType);
  qApp->processEvents();
  double Time = timer.elapsed()/1000.;
  if(action < 0) // error
      {blockInput(false);
      if(getPigaleError())
          {Tprintf("Handler Error:%s",(const char *)getPigaleErrorString().toLatin1());
          setPigaleError(0);
          }
      else
          Tprintf("Unknown error:%d",action);
      pigaleThread->run(0); 
      return;
      }
 
  if(saveType == 1)
      UndoSave();
  else if(saveType == 2)
      UndoTouch(false);


  // In case we called the orienthandler
  chkOrient->setCheckState(staticData::ShowOrientation() ? Qt::Checked : Qt::Unchecked);
  if(action == 1)
      //{if(!MacroExecuting )gw->editor->update(1);}
      {gw->editor->update(1);}
  else if(action == 2)
      {if(MacroExecuting)information(false);
      //else if(MacroRecording)information();
      else information();
      if(!MacroExecuting )gw->editor->update(1);
      }
  else if(action == 20) // Remove handler
      {if(!MacroRecording)information();
      if(!MacroExecuting ) gw->editor->update(0);
      }
  else if(action == 3)
      mypaint->update(drawingType); 
  else if(action == 4) //3d drawings
      graphgl->update(); 
  else if(action == 5) //symetrie
      {graphgl->update(); 
      graphsym->update();
      }
  // cases 6-7-8 we need a canvas with the current graph loaded
  else if(action == 6)
      {gw->editor->update(1);
      mouse_actions->ButtonFitGrid->setChecked(false);
      gw->editor->Spring();
      }
  else if(action == 7)
      {gw->editor->update(1);
      mouse_actions->ButtonFitGrid->setChecked(false);
      gw->editor->SpringPreservingMap();
      }
  else if(action == 8)
      {gw->editor->update(1);
      mouse_actions->ButtonFitGrid->setChecked(false);
      gw->editor->SpringJacquard();
      }

  //cases 10 create a png 11 create a ps
  else if(action == 10)
      image();
  else if(action == 11)
      print();
 
  blockInput(false);
  double TimeG = timer.elapsed()/1000.;
  if(!MacroLooping && !MacroRecording && !ServerExecuting)
      {Tprintf("Used time:%3.3f (G+I:%3.3f)",Time,TimeG);
      if(getPigaleError())
          {Tprintf("Handler Error:%s",(const char *)getPigaleErrorString().toLatin1());
          setPigaleError(0);
          if(debug())Twait((const char *)getPigaleErrorString().toLatin1());
          }
      }
  qApp->processEvents();
  pigaleThread->run(0); 
  return;
  }
int & pigaleWindow::getResultHandler()
  {static int _value = 0;
  return _value;
  }

/************************************************************/
PigaleThread::PigaleThread(QObject *parent) 
    : QThread (parent),abort(false),previous_action(0),action(0)
  { }
PigaleThread::~PigaleThread()
  {stop();
  }
void PigaleThread::stop()
  {mutex.lock();
  abort = true;
  condition.wakeOne();
  mutex.unlock();
  wait();
  }
void PigaleThread:: run(int _action,int _N,int _N1,int _N2,int _M,int _delay)
  {QMutexLocker locker(&mutex);
  previous_action = action;
  action = _action;
  N  = _N;
  N1 = _N1;
  N2 = _N2;
  M  = _M;
  delay = _delay;
  if(!isRunning()) 
      start();
  else
      condition.wakeOne();
  }
void PigaleThread::run()
  {int ret,saveType,drawingType;
  
  for(;;)
      {QMutexLocker locker(&mutex);
      int action = this->action;
      int N      = this->N;
      int N1     = this->N1;
      int N2     = this->N2;
      int M      = this->M;
      //delay      = this->delay;

      if(abort)return;
      if(action && previous_action)
          Tprintf("Warning: %d %d",action,previous_action);
      
      if(action)
          {ret = saveType = drawingType = 0;
          mw->timer.start();
          
          if(action < A_AUGMENT_END)
              ret = AugmentHandler(mw->GC,action);
          else if(action < A_EMBED_END)
              ret = EmbedHandler(mw->GC,action,drawingType); 
          else if(action < A_GRAPH_END)
              {ret = DualHandler(mw->GC,action); 
              saveType = 1;
              }
          else if(action < A_REMOVE_END)
              {ret = RemoveHandler(mw->GC,action);
              saveType = 2;
              }
          else if(action < A_GENERATE_END)
              {ret = GenerateHandler(mw->GC,action,N1,N2,M);
              saveType = 1;
              }
          else if(action < A_ALGO_END)
              ret = AlgoHandler(mw->GC,action,N);
          else if(action < A_ORIENT_END)
              {ret = OrientHandler(mw->GC,action);
              staticData::ShowOrientation() = true;
              }
          else if(action < A_TEST_END)
              ret = Test(mw->GC,action - A_TEST,drawingType);
          else if(action == A_TRANS_GET_CGRAPH || action == A_INPUT_READ_GRAPH)
              {mw->publicLoad(mw->GraphIndex1);
              ret = 2;
              }
           else if(action == A_TRANS_SEND_PNG)
               ret = 10;
           else if(action == A_TRANS_SEND_PS)
               ret = 11;
          // post an event to execute the graphics
          mw->pigaleThreadRet = ret;
          emit handlerSignal(ret,drawingType,saveType);
          }
      condition.wait(&mutex);
      // ne s'execute que quand: condition.wakeOne();
      if(action)
          mw->getResultHandler() = mw->pigaleThreadRet;
      else
          continue;
      if(mw->ServerExecuting)// will execute only when posthandler has finished
          {if(getPigaleError())
              {mw->threadServer->writeClientEvent(":ERROR "+getPigaleErrorString()+" : "+mw->getActionString(action));
              setPigaleError();
              mw->threadServer->writeClientEvent("!!");
              mw->threadServer->serverReady();
              }
          else
              {if(action ==  A_TRANS_SEND_PNG)
                  {
// #ifdef _WIN32
//                   QString PngFileName =  QString("/tmp/server%1.png").arg(mw->ServerClientId);
//                   PngFileName = universalFileName(PngFileName);
//                   uint size = QFileInfo(PngFileName).size();
//                   int retry = 0;
//                   while((size = QFileInfo(PngFileName).size()) < 1000)
//                       {msleep(100);
//                       Tprintf("-> png size:%d retry:%d",size,retry);
//                       if(++retry > 20)break;
//                       }
//                   if(retry)Tprintf("---> png size:%d retry=%d",size,retry);
// #endif
                  mw->threadServer->Png();
                  }
              else if(action ==  A_TRANS_SEND_PS)
                  {

// #ifdef _WIN32
//                   QString PngFileName =  QString("/tmp/server%1.ps").arg(mw->ServerClientId);
//                   PngFileName = universalFileName(PngFileName);
//                   uint size = QFileInfo(PngFileName).size();
//                   int retry = 0;
//                   while((size = QFileInfo(PngFileName).size()) < 1000)
//                       {msleep(100);
//                       Tprintf("-> pdf size:%d retry:%d",size,retry);
//                       if(++retry > 20)break;
//                       }
//                   if(retry)Tprintf("---> pdf size:%d retry=%d",size,retry);
// #endif
                  mw->threadServer->Ps();
                  }
              else
                  {mw->threadServer->writeClientEvent(QString("!%1 SSS").arg(mw->getActionString(action)));
                  mw->threadServer->serverReady();
                  }
              }
          }
      }
  }
