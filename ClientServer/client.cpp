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
\brief  Client and threadRead class implementation
*/


#include "client.h"
#include <qtextstream.h>
#include <qevent.h>
 
using namespace std;


QString  universalFileName(QString const & fileName)
  {QString filename = fileName;
  filename.replace('/', QDir::separator());
  return filename;
  }

Client::Client(const QString &_host, quint16 _port)
    :host(_host),port(_port),ActionsToDo(0),dbg(false),warning(0),numFiles(0)
  {infoText = new QTextEdit( this );
  inputText = new QLineEdit( this );
  send = new QPushButton("Send",this);
  connection = new QPushButton("Connect",this);
  close = new QPushButton("Close connection",this);close->hide();
  quit = new QPushButton("Quit",this);
  QVBoxLayout *layoutV = new QVBoxLayout;
  setLayout(layoutV);
  layoutV->addWidget(infoText);
  QHBoxLayout *layoutH = new QHBoxLayout();
  layoutH->addWidget(inputText);
  layoutH->addWidget(send);
  layoutV->addLayout(layoutH);
  layoutV->addWidget(connection);
  layoutV->addWidget(close);
  layoutV->addWidget(quit);
  connect(send,SIGNAL(clicked()),SLOT(sendToServer()));
  connect(connection,SIGNAL(clicked()),SLOT(askConnection()));
  connect(close,SIGNAL(clicked()),SLOT(askCloseConnection()));
  connect(quit,SIGNAL(clicked()),SLOT(exit()));
  connect(qApp,SIGNAL(aboutToQuit()),SLOT(stop()));
  // create the socket and connect various of its signals
  socket = new QTcpSocket(this);
  socket->connectToHost(host,port);
  clo.setDevice(socket); clo.setVersion(QDataStream::Qt_4_3);
  connect(socket,SIGNAL(connected()),SLOT(socketConnected()));
  connect(socket,SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()));
  connect(socket,SIGNAL(readyRead()),SLOT(socketReadyRead()));
  connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(socketError(QAbstractSocket::SocketError)));
  // connect to the server
  infoText->append("Trying to connect to the server" );
  inputText->setFocus();
  }
void Client::stop()
  {ThreadRead.terminate();ThreadRead.wait();
  }
void Client::exit()
  {qApp->quit();
  } 
void Client::socketConnected()
  {infoText->append("Connected to server");
  stack.push("Server Ready");
  ChangeActionsToDo(1);
  ThreadRead.pclient = this;
  ThreadRead.start();
  connection->hide();quit->hide();close->show();inputText->show();send->show();
  }
void Client::socketConnectionClosed()
  {infoText->append("Connection closed by the server");
  stop();
  connection->show();
  }
void Client::askConnection()
  {socket->connectToHost(host,port);
  }
void Client::askCloseConnection()
  {if(socket->state() != QAbstractSocket::ConnectedState)return;
  QString str = "!";
  sendToServer(str);
  stop();
  }
void Client::closeConnection()
  {if(socket->state() == QAbstractSocket::ClosingState )
      connect(socket,SIGNAL(delayedCloseFinished()),SLOT(socketClosed()));
  send->hide();close->hide();inputText->hide();quit->show();
  }
void Client::socketError(QAbstractSocket::SocketError e)
  {if(e == QAbstractSocket::ConnectionRefusedError)
      infoText->append(QString("Connection refused"));
  else if(e  == QAbstractSocket::HostNotFoundError) 
      infoText->append(QString("Host not found")); 
  else if(e == QAbstractSocket::SocketTimeoutError)
      ;//infoText->append(QString("Socket timeout"));
  else 
      infoText->append(QString("Socket error:%1").arg(e)); 
  }

void Client::customEvent(QEvent * e) 
  {if( e->type() == (int)TEXT_EVENT )
      {textEvent *event  =  (textEvent  *)e;
      infoText->append(event->getString());
      infoText->ensureCursorVisible();
      }
  else if( e->type() == (int)WRITE_EVENT )
      {writeEvent *event  =  (writeEvent  *)e;
      writeServer(event->getString());
      }
  else if(e->type() == (int)WRITEB_EVENT) 
    {writeBufEvent  *event  =  (writeBufEvent  *)e;
    writeServer(event->getPtr(),event->getSize());
    }
  }
void Client::writeClient(QString str)
//messages in the client window
  {textEvent *e = new textEvent(str);
  //cout<<"write:"<<(const char*)str.toAscii()<<endl;
  QApplication::postEvent(this,e);
  }
void Client::writeServerEvent(QString str)
  {writeEvent *e = new writeEvent(str);
  QApplication::postEvent(this,e);
  }
void Client::writeServerEvent(char * buf,uint size)
  {writeBufEvent *event = new writeBufEvent(buf,size);
  QApplication::postEvent(this,event);
  }
void Client::writeServer(QString str)
  {QWriteLocker locker(&lock);
  QString t = str+'\n';
  clo.writeBytes(t.toLatin1(),t.length());
  socket->waitForBytesWritten(-1); 
  }
void Client::writeServer(char * buff,quint32 size)
  {QWriteLocker locker(&lock);
  clo.writeBytes(buff,size);
  socket->waitForBytesWritten(-1); 
  delete [] buff;
  }
/*******************************************************/
int Client::ChangeActionsToDo(int delta)
  {int i;
  mutex.lock();
  i=(ActionsToDo += delta);
  mutex.unlock();
  if(delta == 0)return i;

  if(delta < 0)
      {if(stack.isEmpty())
          {writeClient(QString("ERROR:-STACK EMPTY %1 ").arg(i));
          QMutexLocker locker(&mutex);
          ActionsToDo = 0;
          return 0;
          }
      stack.pop(); 
      }
  else //if(debug())
      {if(stack.isEmpty()){writeClient(QString("ERROR:+STACK EMPTY %1 ").arg(i));return i;}
      writeClient(QString("%1 (%2)").arg(stack.top()).arg(i));
      }
  return i; 
  } 
void Client:: debug(bool b) 
  { 
  mutex.lock(); 
  dbg=b; 
  mutex.unlock();
  
  QString str = QString("S_DEBUG;%1").arg(b);
  sendToServer(str);
  }
void Client::sendToServer() // only for intercative action
  {if(socket->state() != QAbstractSocket::ConnectedState)return;
  QString str = inputText->text();
  sendToServer(str);
  inputText->setText(""); 
  }
void Client::sendToServer(QString &str)
  {if(str.at(0) == '#' || str.at(0) == '!') //split str -> 1 command per line if not a comment
      {stack.push(str);
      ChangeActionsToDo(1);
      writeServerEvent(str);
      return;
      }
  QStringList fields = str.split(ACTION_SEP);
  for(int i = 0; i < (int)fields.count();i++)
      {fields[i]= fields[i].simplified();
      if(fields[i].contains("RC_GRAPH"))
          sendToServerGraph(fields[i]); 
      //if the file exists sendToServerGraph will add the command on the stack
      else
          {stack.push(fields[i]);
          ChangeActionsToDo(1);
          writeServerEvent(fields[i]);
          }
      }
  }
int Client::sendToServerGraph(QString &data)
  {if(socket->state() != QAbstractSocket::ConnectedState)return -1;
  QStringList fields =data.split(PARAM_SEP);
  if(fields.count() < 2){writeClient("MISSING ARGUMENT");return -1;}
  QString FileName = fields[1].simplified();
  QString GraphFileName = universalFileName(FileName);
  QFileInfo fi = QFileInfo(GraphFileName);
  if(GraphFileName.isEmpty() || !fi.isFile() || !fi.size())
      {writeClient(QString("NO FILE:%1").arg(GraphFileName));
      return -1;
      }
  stack.push("RC_GRAPH");
  ChangeActionsToDo(1);
  quint32 size = fi.size();
  if(debug()) writeClient(QString("Client sending:%1 %2 bytes").arg(GraphFileName).arg(size));
  writeServerEvent(data);
  QFile file(GraphFileName);
  file.open(QIODevice::ReadOnly);
  QDataStream stream(&file);
  char *buff = new char[size];
  stream.readRawData(buff,size); 
  writeServerEvent(buff,size);
  return 0;
  }
/*
uint Client::readBuffer(char*  &buffer)
  {uint nb;
  while((nb = socket->bytesAvailable()) < (int)sizeof(uint))
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {writeClient("client not connected");return 0;}
      socket->waitForReadyRead(10);
      }
  uint size;
  clo >> size;
  buffer = new char[size+1];
  char *pbuff = buffer;
  int retry = 0;
  uint nread = 0; 
  uint size0 = 0;
  while(nread  < size)
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {writeClient("client not connected");return 0;}
      nb = socket->bytesAvailable();
      if(nb == 0)
          {if(++retry > 1000){writeClient("TIMEOUT");ChangeActionsToDo(-1);return 0;}
          socket->waitForReadyRead(10);
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
          writeClient(QString("%1 % (%2 / %3)").arg(percent).arg(nread).arg(size));
          }
      }
  return size;
  }
*/
uint Client::readBuffer(char*  &buffer)
  {uint nb;
  while((nb = socket->bytesAvailable()) < (int)sizeof(uint))
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {writeClient("client not connected");return 0;}
      socket->waitForReadyRead(10);
      }
  uint size;
  clo >> size;
  buffer = new char[size];
  char *pbuff = buffer;
  while((nb = socket->bytesAvailable()) < size)
      {if(socket->state() != QAbstractSocket::ConnectedState)
          {writeClient("client not connected");return 0;}
      socket->waitForReadyRead(100);
      }
  clo.readRawData(pbuff,size);

  if(nb == size)return size;
  cout<<"++++++++++++++nb:"<<nb<<" size:"<<size<<endl;
  return size;
  }
void Client::socketReadyRead()
//reads messages from server
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.simplified();
      //cout<<"read:"<<(const char*) str.toLatin1()<<endl;
      if(str.at(0) == ':' || str.at(0) == '?' )
          writeClient(str.mid(1));

      else if(str.contains("!PNGREADY"))// receiving a png image
          {char * buffer = NULL;
          lock.lockForRead();
          uint size = readBuffer(buffer);
          lock.unlock();
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PngFile = QString("image%1.png").arg(++numFiles,3,10,QLatin1Char('0'));
          QFile file(PngFile);          file.open(QIODevice::WriteOnly | QIODevice::Truncate);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeClient("!PNG");
          ChangeActionsToDo(-1);
          }
      else if(str.contains("!PSREADY"))// receiving a ps image
          {char * buffer = NULL;
          lock.lockForRead();
          uint size = readBuffer(buffer);
          lock.unlock();
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PsFile = QString("image%1.ps").arg(++numFiles);
          QFile file(PsFile);          file.open(QIODevice::WriteOnly | QIODevice::Truncate);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeClient("!PS");
          ChangeActionsToDo(-1);
          }
      else if(str.contains("!RBUFFER"))// receiving a graph
          {char * buffer = NULL;
          lock.lockForRead();
          uint size = readBuffer(buffer);
          lock.unlock();
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          int pos = str.indexOf(PARAM_SEP);
          QString File = str.mid(pos+1);
          QFile file(File);  
          file.open(QIODevice::WriteOnly | QIODevice::Truncate);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeClient(QString("GOT:%1").arg(File));
          ChangeActionsToDo(-1);
          }
      else if(str == "!!")// server has finished everything
          {ChangeActionsToDo(-1);
          closeConnection();
          if(warning)writeClient(QString("warning:%1").arg(warning));
          }
      else if(str == "!|")// server ha finished everything and client wants to quit
          {closeConnection();
          exit();
          }
      else if(str.at(0) == '!')//server has finished one action
          {if(!stack.isEmpty())
              {if(str.at(1) == '#')
                  ;//writeClient("!"+stack.top()+QString(" (%1)").arg(ChangeActionsToDo(0)-1));
              else
                  {QString stt = stack.top();
                  int index = stt.indexOf(';');
                  if(index > 0)stt = stt.left(index);
                  if(str.contains(stt))
                      {if(debug())writeClient("!"+stack.top()+QString(" (%1)").arg(ChangeActionsToDo(0)-1));
                      }
                  else 
                      {writeClient("*** !"+stack.top()+QString(" (%1)").arg(ChangeActionsToDo(0)-1)+"   "+str);
                      cout << (const char *)stack.top().toLatin1()<< " != "<<(const char *)str.toLatin1()<<endl;
                      ++warning;
                      }
                  }
              }
          else
              writeClient("<- "+str+QString(" EMPTY:%1").arg(ChangeActionsToDo(0)-1));
          ChangeActionsToDo(-1);
          }
      else // comments sent by the server
          writeClient(str);
      }
  }

void threadRead::run() 
// read datas from stdin
  {QTextStream stream(stdin,QIODevice::ReadWrite);
  QString str;
  while(!stream.atEnd())
      {int retry = 0;
      while(pclient->ChangeActionsToDo(0) > 0)
          {msleep(100);// milliseconds
          if(++retry %10 == 0 && pclient->debug())
              pclient->writeClient(QString("Waiting %1s (%2:%3)").arg(retry/10)
                  .arg(pclient->stack.top()).arg(pclient->ChangeActionsToDo(0)));
          }
      str = stream.readLine(); 
      QChar ch = str.at(0);
      if(ch == ':')
          {QChar c = str.at(1);
          if(c == QChar('!'))
              {str = "!";
              pclient->sendToServer(str);
              return;
              }
          else if(c == 'D')
              pclient->debug(true);
          else if(c == 'd')
              pclient->debug(false);
          else if(c == 'X')	
              {str = "|";
              pclient->sendToServer(str);
              }
          }
      else if(pclient->debug() || ch != '#')
          pclient->sendToServer(str);
      }
}

int main( int argc, char** argv )
  {QApplication app( argc, argv );
  Client client(argc<2 ? "localhost" : argv[1], 4242);
  client.show();
  return app.exec();
  }

