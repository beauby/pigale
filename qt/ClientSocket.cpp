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
\file 
\brief PigaleServer and ClientSocket class implementation
*/

#include "ClientSocket.h"
#include "GraphWidget.h"
#include "GraphGL.h"
#include "GraphSym.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <QT/Misc.h>
#include <QT/Handler.h>
#include <QT/pigalePaint.h>
#include <QT/clientEvent.h> 

#include <QStringList>


PigaleServer::PigaleServer(pigaleWindow *_mw)
    :QTcpServer(_mw),mw(_mw)
  {nconnections = 0;
  mw->ServerClientId = 0;
  listen(QHostAddress::Any,4242); 
  connect(this,SIGNAL(newConnection()),this,SLOT(OneClientOpened()));
  }
void PigaleServer::OneClientOpened()
  {if(++nconnections == 1)createNewServer();
  }
void PigaleServer::OneClientClosed()
  {//mw->threadServer->quit();
  delete mw->threadServer;
  --nconnections;
  if(hasPendingConnections())createNewServer();
  }
void PigaleServer::createNewServer()
  {QTcpSocket *socket = nextPendingConnection();
  ClientSocket *th = new ClientSocket(mw,socket);
  connect(socket,SIGNAL(disconnected()),SLOT(OneClientClosed()));
  connect(socket,SIGNAL(disconnected()),socket,SLOT(deleteLater()));
  connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),th,SLOT(socketError(QAbstractSocket::SocketError)));
  mw->threadServer = th;
  }

//! ClientSocket (non thread)
ClientSocket::ClientSocket(pigaleWindow *_mw,QTcpSocket *_socket)
    :mw(_mw)
    ,sdebug(0),line(0)
  {socket = _socket;
  clo.setDevice(socket);   clo.setVersion(QDataStream::Qt_4_3);
  mw->postMessageClear();
  mw->ServerExecuting = true;      mw->blockInput(true);
  Tprintf("Server: New connection %d",++mw->ServerClientId);
  writeClientEvent(":Server Ready");
  writeClientEvent("!Server Ready C");
  serverReady(); 
  }
ClientSocket:: ~ClientSocket()
  {Tprintf("Server: Client disconnects %d",mw->ServerClientId);
  mw->ServerExecuting = false;   mw->blockInput(false);
  }
void ClientSocket::socketError(QAbstractSocket::SocketError )
  {;//cout <<"socket error : "<<e<<endl;
  }
void  ClientSocket::customEvent(QEvent * ev)
  {ev->accept();
  if(ev->type() == (int)CLIENT_EVENT) 
      {clientEvent  *event  =  (clientEvent  *)ev;
      handlerInput(event->getAction(),event->getParamString());
      }
  else if(ev->type() == (int)SERVER_READY_EVENT) 
      executeAction();
  else if(ev->type() == (int)WRITE_EVENT) 
      {writeEvent  *event  =  (writeEvent  *)ev;
      writeClient(event->getString());
     }
  else if(ev->type() == (int)WRITEB_EVENT) 
      {writeBufEvent  *event  =  (writeBufEvent  *)ev;
      writeClient(event->getPtr(),event->getSize());
      }
  }
void ClientSocket::executeAction()
//reads messages from client
  {QString str =  readLine();
  if(str.size() == 0){serverReady();return;}// never happens
  ++line;
  if(str.at(0) == '#')
      {writeClientEvent("!"+str);
      serverReady();
      }
  else if(str.at(0) == '!')
      {writeClientEvent(":END OF FILE");
      writeClientEvent("!!");
      }
  else if(str.at(0) == '|')
      {writeClientEvent(":QUIT");
      writeClientEvent("!|");
      }
  else 
      xhandler(str);
  }
void ClientSocket::serverReady()
  {serverReadyEvent *event = new serverReadyEvent();
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClientEvent(QString str)
  {writeEvent *event = new writeEvent(str);
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClientEvent(char * buf,uint size)
  {writeBufEvent *event = new writeBufEvent(buf,size);
  QApplication::postEvent(this,event);
  }
void ClientSocket::writeClient(QString str)
// send a message to the client
  {QWriteLocker locker(&lock);
  QString t = str+'\n'; 
  clo.writeRawData(t.toLatin1(),t.length());
  socket->waitForBytesWritten(-1); 
  }
void ClientSocket::writeClient(char * buff,uint size)
// send a buffer to the client
  {QWriteLocker locker(&lock);
  if(socket->state() != QAbstractSocket::ConnectedState)
      {delete [] buff;return;}
  clo.writeBytes(buff,size);
  socket->waitForBytesWritten(-1);  
  delete [] buff;
  }

uint ClientSocket::readBuffer(char * &buffer)
  {QReadLocker locker(&lock);
  quint32 size = 0; 
  uint nb = 0;
  
  while((nb = socket->bytesAvailable()) < (int)sizeof(quint32))
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setPigaleError(-1,"client not connected");return 0;}
       socket->waitForReadyRead(100);
      }

  clo >>  size;
  if(size <= 0){setPigaleError(-1,"empty file");return 0;}

  buffer  = new char[size+1];
  char *pbuff = buffer;
  while((nb = socket->bytesAvailable()) < size)
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setPigaleError(-1,"client not connected");return 0;}
      socket->waitForReadyRead(100);
      }
  //clo.readRawData(pbuff,nb);
  clo.readRawData(pbuff,size);
  return size;
  }

/*
uint ClientSocket::readBuffer(char * &buffer)
  {QReadLocker locker(&lock);
  quint32 size = 0; 
  uint nb = 0;
  while((nb = socket->bytesAvailable()) < (int)sizeof(quint32))
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setPigaleError(-1,"client not connected");return 0;}
       socket->waitForReadyRead(100);
      }
  clo >>  size;
  if(size <= 0){setPigaleError(-1,"empty file");return 0;}
  buffer  = new char[size+1];
  char *pbuff = buffer;
  int retry = 0;
  uint nread = 0; 
  uint size0 = 0;
  while(nread  < size)
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setPigaleError(-1,"client not connected");return 0;}
      nb = socket->bytesAvailable();
      if(nb == 0)
          {if(++retry > 1000){setPigaleError(-1,"TIMEOUT");return 0;}
          socket->waitForReadyRead(100);
          continue;
          }
      retry = 0;
      if(nb > size-nread)nb = size-nread;
      nread += nb;
      clo.readRawData(pbuff,nb);
      pbuff += nb;
      if(nread >= size0 && debug())
          {int percent = (int)(nread*100./size + .5);
          size0 = nread + size/10; // we write when at least 10% more  is read
          QString t = QString("%1 % (%2 / %3)").arg(percent).arg(nread).arg(size);
          Tprintf("%s",(const char*)t.toAscii());
          }
      }
  return size;
  }
*/
QString  ClientSocket::readLine()
  {QReadLocker locker(&lock);
  while(!socket->canReadLine())
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {setPigaleError(-1,"client not connected");return 0;}
      socket->waitForReadyRead(100);
      }
  uint len;  char * buffer = NULL;
  clo.readBytes(buffer,len);
  QString str(buffer); str = str.trimmed();
  delete [] buffer;
  return str;
  }
void ClientSocket::xhandler(const QString& dataAction)
  {int pos = dataAction.indexOf(PARAM_SEP);
  QString dataParam = dataAction.mid(pos+1);
  int action = mw->getActionInt( dataAction.left(pos));
  if(sdebug)Tprintf("%s ",(const char *)dataAction.toLatin1());
  //cout<<(const char *)dataAction.toAscii()<<endl;
  // call the right handler
  if(action == 0)
      setPigaleError(-1,"unknown action !!");
  else if(action > A_INFO && action < A_INFO_END)
      handlerInfo(action);
  else if(action > A_INPUT && action < A_INPUT_END)
      {if(action ==  A_INPUT_READ_GRAPH)
          {if(readServerGraph(dataParam) == 0)
              setPigaleError(-1,"Could not read graph");
          }
      else
          {clientEvent *event = new clientEvent(action,dataParam);
          QApplication::postEvent(this,event);
          }
      }
  else if(action > A_AUGMENT && action < A_TEST_END)
      {if(mw->graph_properties->actionAllowed(action))
          {mw->handler(action); // when finished pigale will write to the client
          return;
          }
      else 
          writeClientEvent(":ACTION NOT ALLOWED:"+mw->getActionString(action));
      }
  else if (action > A_PROP_DEFAULT && action < A_PROP_DEFAULT_END)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(pos == -1||fields.count() != 3)
          setPigaleError(-1,"Bad number of parameters");

      bool ok =true;
      int setnum = fields[0].toInt(&ok); if (ok && (setnum<0 || setnum>2)) ok=false;
      if(!ok)setPigaleError(-1,"Wrong set number");
      PSet *set=(PSet *)0;
      switch(setnum) 
          {case 0:
              set=&mw->GC.Set(tvertex());
              break;
          case 1:
              set=&mw->GC.Set(tedge());
              break;
          case 2:
              set=&mw->GC.Set(tbrin());
              break;
          }
      int pnum=fields[1].toInt(&ok); if (ok && (pnum<0 || pnum > 255)) ok=false;
      if(!ok)setPigaleError(-1,"Wrong property number");
      switch(action)
         {case A_PROP_DEF_SHORT:
             {Prop<short> x(*set,pnum);
             x.definit(fields[2].toShort(&ok));
             }
             break;
         case A_PROP_DEF_INT:
             {Prop<int> x(*set,pnum);
             x.definit(fields[2].toInt(&ok));
             }
             break;
         default:
             setPigaleError(-1,"unknown command");
             ok=true;
         }
      if(!ok)setPigaleError(-1,"Wrong second parameter");
      }
  else if(action > A_TRANS && action < A_TRANS_END)
      {if(action == A_TRANS_SEND_PNG || action ==  A_TRANS_SEND_PS)
          {int size = 500;
          QStringList fields = dataParam.split(PARAM_SEP);
          if(fields.count())
              {bool ok = true;
              size = fields[0].toInt(&ok);
              if(!ok)size = 500;
              }
          staticData::sizeImage = size;
          mw->handler(action);return;
          }
      else if(action == A_TRANS_GET_CGRAPH) 
          // get a graph form client, read a record and display it
          {QStringList fields = dataParam.split(PARAM_SEP);
          int index = 1;
          bool ok =true;
          if(fields.count() > 1)index = fields[1].toInt(&ok);
          if(!ok)setPigaleError(-1,"Wrong parameters");
          else   readClientGraph(index);
          return;
          }
      else  if(action == A_TRANS_SEND_GRAPH_SAVE) 
          // save the graph and send it to the client
          {QStringList fields = dataParam.split(PARAM_SEP);
          if(pos == -1||fields.count() != 1)
              setPigaleError(-1,"Wrong parameters");
          else 
              {sendSaveGraph(fields[0]);
              return;
              }
          }
      }
  else if(action > A_SET_GEN && action < A_SET_GEN_END)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(fields.count() < 1)
          {setPigaleError(-1,"Missing  parameter");
          writeClientEvent(":ERROR "+ getPigaleErrorString()+ "action:"+mw->getActionString(action));
          setPigaleError();
          writeClientEvent(QString("!%1 G").arg(action));
          serverReady();
          return;
          }
      bool ok =true;
      int value  = fields[0].toInt(&ok);
      if(!ok)setPigaleError(-1,"Wrong parameters");
      else
          switch(action)
              {case A_SET_GEN_N1:
                  //mw->Gen_N1 = value;
                  staticData::Gen_N1 = value;
                  break;
              case A_SET_GEN_N2:
                  staticData::Gen_N2 = value;
                  break;
              case A_SET_GEN_M:
                  staticData::Gen_M = value;
                  break;
              case A_SET_GEN_SEED:
                  randomSetSeed() = value;
		  break;
              default:
                  setPigaleError(-1,"unknown command");
                  break;
              }
      }
  else if(action == SERVER_DEBUG)
      {QStringList fields = dataParam.split(PARAM_SEP);
      if(fields.count() != 1)
          setPigaleError(-1,"Bad number of parameters");
      else
          {bool ok =true;
          int setnum = fields[0].toInt(&ok); 
          if(!ok)setPigaleError(-1,"Wrong parameter");
          else sdebug = setnum;
          }
      }
  else
      setPigaleError(-1,"unknown command");
 
  if(getPigaleError())
      {if(action)
          writeClientEvent(":ERROR "+ getPigaleErrorString()+" action: "+mw->getActionString(action));
      else
          writeClientEvent(":ERROR "+ getPigaleErrorString()); 
      //      writeClientEvent("!!"); // close 
      setPigaleError();
      }
  //cout <<action<<"++"<<(const char*)mw->getActionString(action).toAscii();
  // needed for some actions likeS_DEBUG
  writeClientEvent(QString("!%1 CCC").arg(mw->getActionString(action)));
  serverReady();
  }
void ClientSocket::readClientGraph(int indexRemoteGraph)
  {char *buffer = NULL;
  uint size = readBuffer(buffer);
  if(getPigaleError()){delete [] buffer;return;}
  QString  GraphFileName;
  GraphFileName.sprintf("/tmp/graph%d.tmp",mw->ServerClientId);
  GraphFileName = universalFileName(GraphFileName);
  QFile file(GraphFileName);  //file.remove();
  file.open(QIODevice::WriteOnly  | QIODevice::Truncate);
  QDataStream stream(&file);
  stream.writeRawData(buffer,size);
  file.close();
  mw->InputFileName = GraphFileName;
  mw->GraphIndex1 = indexRemoteGraph;
  mw->handler(A_TRANS_GET_CGRAPH);
  }
int ClientSocket::readServerGraph(QString &dataParam)
  {QStringList fields = dataParam.split(PARAM_SEP);
  int nfield = (int)fields.count();
  if(nfield == 0){setPigaleError(-1,"Wrong parameters");return -1;}
  bool ok = true;
  int num = 1;
  if(nfield > 1)
      {num = fields[1].toInt(&ok);
      if(!ok){setPigaleError(-1,"Wrong parameters");return -1;}
      }
  mw->InputFileName = universalFileName(fields[0]);
  mw->GraphIndex1 = num;
  mw->handler(A_INPUT_READ_GRAPH);
  return 0;
  }
void ClientSocket::sendSaveGraph(const QString &FileName)
  {QString graphFileName = QString("/tmp/%1").arg(FileName);
  graphFileName = universalFileName(graphFileName);
  mw->publicSave(graphFileName);
  QFileInfo fi = QFileInfo(graphFileName);
  uint size = fi.size();
  if(size == 0)
      {setPigaleError(-1,"no graph file");
      writeClientEvent("!!");serverReady();return;
      }
  Tprintf("graph size:%d",size);
  QFile file(graphFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!RBUFFER;"+FileName);
  writeClientEvent(buff,size);
  file.remove();
  serverReady();
  }
void ClientSocket::Png()
  {QString PngFileName =  QString("/tmp/server%1.png").arg(mw->ServerClientId);
  PngFileName = universalFileName(PngFileName);
  qApp->processEvents();
  uint size = QFileInfo(PngFileName).size();
  if(size == 0)
      {writeClientEvent(QString(":ERROR:%1 not found").arg(PngFileName));
      writeClientEvent("!!");serverReady();return;
      }
  Tprintf("png size:%d",size);
  QFile file(PngFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!PNGREADY");
  writeClientEvent(buff,size);
  file.remove();
  serverReady();
  }
void ClientSocket::Ps()
  {QString PsFileName =  QString("/tmp/server%1.ps").arg(mw->ServerClientId);
  PsFileName = universalFileName(PsFileName);
  QFileInfo fi = QFileInfo(PsFileName);
  uint size = fi.size();
  if(size == 0)
      {writeClientEvent(":ERROR: NO PS FILE");
      writeClientEvent("!!");serverReady();return;
      }
  Tprintf("pdf size:%d",size);
  QFile file(PsFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeClientEvent("!PSREADY");
  writeClientEvent(buff,size);
  file.remove();
  serverReady();
  }
void ClientSocket::handlerInput(int action,const QString& dataParam)
// actions graphiques -> called by event. les updates devraient etre faits par handler (id NewGraph)
  {QStringList fields = dataParam.split(PARAM_SEP);
  QString msg;
  int nfield = (int)fields.count();
  bool ok = true;
  //cout << "handlerInput:-"<<(const char*)dataParam<<"- "<<nfield<<" field"<<endl;
  switch(action)
      {case  A_INPUT_NEW_GRAPH:
          mw->NewGraph();
          break;
      case A_INPUT_NEW_VERTEX:
          {int n = 1;
          if(nfield > 0)
              {n = fields[0].toInt(&ok);
              if(!ok){setPigaleError(-1,"Wrong parameters");return;}
              }
          TopologicalGraph G(mw->GC);
          for(int i = 0;i < n;i++)G.NewVertex();
          // Labels
          Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);
          for(int i = 0;i <= n;i++)vlabel[i] = i;
          // Calcul des coordonnes
          Prop<Tpoint> vcoord(G.PV(),PROP_COORD);
          double angle = 2.*acos(-1.)/G.nv();
          vcoord[0]=Tpoint(0,0);
          for (int i=1; i<=G.nv(); i++)
              vcoord[i] = Tpoint(cos(angle*i),sin(angle*i));
          }
          break;
      case A_INPUT_NEW_EDGE:
          {if(nfield < 2){setPigaleError(-1,"need 2 vertices");return;}
          int v1 = fields[0].toInt(&ok);
          if(!ok){setPigaleError(-1,"Wrong parameters");return;}
          int v2 = fields[1].toInt(&ok);
          if(!ok){setPigaleError(-1,"Wrong parameters");return;}
          TopologicalGraph G(mw->GC);
          if(v1 > G.nv() || v2 > G.nv() || v1 == v2)
              {setPigaleError(-1,"Wrong parameters");return;}
          G.NewEdge((tvertex)v1,(tvertex)v2);
          mw->postDrawG();mw->information();
          }
          break;
      default:
          setPigaleError(-1,"unknown command");
          break;
      }
  }
void ClientSocket::handlerInfo(int action)
  {TopologicalGraph G(mw->GC);
  mw->graph_properties->updateMenu(false);
  mw->information();
  Graph_Properties *inf = mw->graph_properties;
  switch(action)
      {case A_INFO_N:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(G.nv()));
           break;
      case A_INFO_M:
            writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(G.ne()));
            break;
      case A_INFO_SIMPLE:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Simple()));
          break;
      case A_INFO_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Planar()));
          break;
      case A_INFO_OUTER_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->OuterPlanar()));
          break;
      case A_INFO_SERIE_PAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->SeriePlanar()));
          break;
      case A_INFO_MAX_PLANAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Triangulation()));
          break;
      case A_INFO_BIPAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Biparti()));
          break;
      case A_INFO_MAX_BIPAR:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->MaxBiparti()));
          break;
      case A_INFO_REGULIER:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Regular()));
          break;
      case A_INFO_CON1:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con1()));
          break;
      case A_INFO_CON2:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con2()));
          break;
      case A_INFO_CON3:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->Con3()));
          break;
      case A_INFO_MIN_D:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->DegreeMin()));
          break;
      case A_INFO_MAX_D:
          writeClientEvent("?"+mw->getActionString(action)+QString(":%1").arg(inf->DegreeMax()));
          break;
      case A_INFO_COORD:
          {if(!G.Set(tvertex()).exist(PROP_COORD)) 
              {setPigaleError(-1,"NO COORDS");
              lock.unlock();
              return;
              }
          Prop<Tpoint> coord(G.Set(tvertex()),PROP_COORD);
          writeClientEvent(mw->getActionString(action)+":");
          for(tvertex v = 1;v <= G.nv();v++)
              writeClientEvent("?"+QString("%1 %2").arg(coord[v].x()).arg(coord[v].y()));
          }
          break;
      case A_INFO_VLABEL:
          {if(!G.Set(tvertex()).exist(PROP_LABEL))
              {setPigaleError(-1,"NO LABEL");
              return;
              }
          Prop<int> label(G.Set(tvertex()),PROP_LABEL);
          writeClientEvent("?"+mw->getActionString(action)+":");
          for(tvertex v = 1;v <= G.nv();v++)
              writeClientEvent("?"+QString("%1").arg(label[v]));
          }
          break;
      default:
          writeClientEvent("?"+mw->getActionString(action)+":unknown command");
          break;
      }
  }
