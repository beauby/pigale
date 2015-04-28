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

#ifndef CLIENT_H 
#define CLIENT_H
#include <QCoreApplication> 
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QMutex>
#include <QStack>
#include <QDir> 
#include <QEvent>
#include <QtNetwork>
#include <QReadWriteLock>
#include <QT/clientEvent.h> 
#include <QT/Action_def.h>
#include <TAXI/Tbase.h> 


/* 
In the input:
- lines starting by '#' are treated as comments
- lines starting by ':' are treated by the client
- line  starting by ':!' signals the end of file
- line  starting by ':D' signals the client to echo the comments
- line  starting by ':d' signals the client not to echo the comments
- line  starting by ':X' stops the client

- otherwise a line contains commands  separated by ':'
- commands may contain arguments separated by ';'

When reading from the server
- lines starting with ! are commands
- lines starting by : are diplayed in the text window
otherwise they are output to the terminal 
*/

class Client;

class threadRead : public QThread 
{
public:
  virtual void run();
  QList<QString>  *todo;
  Client* pclient; 
};

class Client: public QObject 
{Q_OBJECT
public:
 Client(const QString &host,quint16 port,QList<QString>  *_todo,int id
        ,QString outDir,QString *result);
  ~Client(){}
  void sendToServer(QString& str);
  bool debug() {bool b; mutex.lock(); b=dbg; mutex.unlock(); return b;}
  void debug(bool b);
  void closeConnection();
private slots:
  void socketReadyRead();
  void socketConnected();
  void socketConnectionClosed();
  void socketClosed();
  void socketError(QAbstractSocket::SocketError e);
  void stop();
  void exit();

public:
  int ChangeActionsToDo(int delta);
  void writeServerEvent(QString str);
  void writeServerEvent(char * buf,uint size);
  void writeClient(QString str);
  QStack<QString > stack;
  QList<QString>  *todo;

private:
  int sendToServerGraph(QString &str);
  uint readBuffer(char *  &buff);
  void writeServer(QString str);
  void writeServer(char * buff,quint32 size);
  
  bool event(QEvent * ev);
  void customEvent(QEvent * e );
  QString host;
  quint16 port;
  QTcpSocket *socket;
  int ActionsToDo;
  bool dbg;
  int warning;
  QMutex mutex;
  QReadWriteLock lock;
  QDataStream clo;
  threadRead ThreadRead;
  int numFiles;
  int identificateur;
  QString outDir;
  QString *result;

};


#endif 

