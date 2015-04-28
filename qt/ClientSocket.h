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
\brief PigaleServer and ClientSocket class definition
*/

#ifndef CLIENTSOCKET_H 
#define CLIENTSOCKET_H


#include <config.h>
#include "pigaleWindow.h"
#include <QT/Misc.h>
#include <QT/Action_def.h>
#include <QT/clientEvent.h> 

#include <QDataStream>




/*!
Each time a client connects, PigaleServer creates a ClientSocket. <br>
This class derives from QTcpServer.
*/
class PigaleServer : public QTcpServer
{Q_OBJECT
public:
  PigaleServer(pigaleWindow *mw);
  void createNewServer();
  int nconnections;
  pigaleWindow *mw;
 
public slots:
  void OneClientClosed();
  void OneClientOpened();
};


/*!
ClientSocket is responsible to dialog with a client connected through a tcp socket, 
and execute the commands received.<br>
This class derives from  QOBJECT.
*/
class ClientSocket : public QObject
{Q_OBJECT
public:
  ClientSocket(pigaleWindow *mw,QTcpSocket *socket);
  ~ClientSocket();
  void executeAction();
  void customEvent(QEvent * e);
  void writeClientEvent(QString str);
  void writeClientEvent(char * buf,uint size);
  void Png();
  void Ps();
  void serverReady();
  QTcpSocket *socket;
private slots:
  void socketError(QAbstractSocket::SocketError e);
private: 
  void writeClient(char * buf,uint size);
  void writeClient(QString  str);
  QString  readLine();
  void xhandler(const QString& data);
  void sendServerId();
  void sendSaveGraph(const QString &FileName);
  int readServerGraph(QString &dataParam);
  void readClientGraph(int indexRemoteGraph);
  void handlerInfo(int action);
  void handlerInput(int action,const QString& data);
  uint readBuffer(char  *  &buff);
  pigaleWindow *mw; 
  QEvent *ev0;
  int socketDescriptor;
  PigaleServer *server;
  QDataStream clo;
  QReadWriteLock lock;
  int sdebug,line;
};


#endif
