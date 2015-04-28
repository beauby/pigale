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
\brief  Client and threadRead  class definition
*/

#ifndef CLIENT_H 
#define CLIENT_H
#include <config.h>
#include <QApplication>
#include <QThread>
#include <QFile>
#include <QBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <qstack.h>
#include <QDir> 
#include <QtNetwork>
#include <QT/clientEvent.h> 
#include <QT/Action_def.h>
#include <TAXI/Tbase.h> 
#include <QtNetwork>

class Client;
//! Thread whose task is to read the input data
class threadRead : public QThread 
{
public:
  virtual void run();
  Client* pclient; 
};


/*! 
\brief Creates a Thread to read input data, and a QTcpSocket to dialog with the server.
 \section usage Usage
The default port used for connection is 4242

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
class Client : public QWidget
{Q_OBJECT
public:
  Client( const QString &host, quint16 port);
  ~Client(){}
  void sendToServer(QString& str);
  bool debug() {bool b; mutex.lock(); b=dbg; mutex.unlock(); return b;}
  void debug(bool b);

private slots:
  void closeConnection();
  void askCloseConnection();
  void askConnection();
  void sendToServer();
  void socketReadyRead();
  void socketConnected();
  void socketConnectionClosed();
  void socketError(QAbstractSocket::SocketError e);
  void stop();
  void exit();

public:
  int ChangeActionsToDo(int delta);
  void writeServerEvent(QString str);
  void writeServerEvent(char * buf,uint size);
  void writeClient(QString str);
  QStack<QString > stack;

protected:
  void customEvent(QEvent *e);

private:
  int sendToServerGraph(QString &str);
  uint readBuffer(char *  &buff);
  void writeServer(QString str);
  void writeServer(char * buff,quint32 size);
  
  QString host;
  quint16 port;
  QPushButton *send,*close,*quit,*connection; 
  QTcpSocket *socket;
  int ActionsToDo;
  bool dbg;
  int warning;
  
  QMutex mutex;
  QReadWriteLock lock;
  QDataStream clo;
  QTextEdit *infoText;
  QLineEdit *inputText;
  threadRead ThreadRead;
  int numFiles;
};


#endif 

