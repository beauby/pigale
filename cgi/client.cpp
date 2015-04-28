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

 
#include "client.h"

using namespace std;

QString  universalFileName(QString const & fileName)
  {QString filename = fileName;
  filename.replace('/', QDir::separator());
  return filename;
  }
  
Client::Client(const QString &_host, quint16 _port,QList <QString>  *_todo
               ,int id, QString _outDir, QString *_result)
    :todo(_todo)
    ,host(_host),port(_port),ActionsToDo(0),dbg(false),warning(0),numFiles(0)
    ,identificateur(id),outDir(_outDir),result(_result)
  {
  // create the socket and connect various of its signals
  socket = new QTcpSocket(this);
  socket->connectToHost(host,port);
  clo.setDevice(socket); clo.setVersion(QDataStream::Qt_4_0);
  connect(socket,SIGNAL(connected()),SLOT(socketConnected()));
  connect(socket,SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()));
  connect(socket,SIGNAL(readyRead()),SLOT(socketReadyRead()));
  connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(socketError(QAbstractSocket::SocketError)));
  }
void Client::stop()
  {ThreadRead.terminate();ThreadRead.wait();
  }
void Client::exit()
  {
  QCoreApplication::quit();
  } 
void Client::socketConnected()
  {//ActionsToDo = 0;
  stack.push("Server Ready");
  ChangeActionsToDo(1);
  ThreadRead.pclient = this;
  ThreadRead.todo = todo;
  ThreadRead.start();
  }
void Client::socketConnectionClosed()
  {
  stop();
  }
void Client::socketClosed()
  {//infoText->append("Connection closed");
  }
void Client::closeConnection()
  {socket->close();
   if(socket->state() == QAbstractSocket::ClosingState )
       connect(socket,SIGNAL(delayedCloseFinished()),SLOT(socketClosed()));
   else
       socketClosed();
  }
void Client::socketError(QAbstractSocket::SocketError )
  {
  }
bool Client::event(QEvent * ev)
  {if(ev->type() >=  QEvent::User)
      {customEvent(ev);
      return TRUE;
      }
  return FALSE;
  }
void Client:: customEvent(QEvent * e ) 
  {if( e->type() == (int)TEXT_EVENT )
      {;
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
  {textEvent *e = new textEvent(str);
  QCoreApplication::postEvent(this,e);
  }
void Client::writeServerEvent(QString str)
  {writeEvent *e = new writeEvent(str);
  QCoreApplication::postEvent(this,e);
  }
void Client::writeServerEvent(char * buf,uint size)
  {writeBufEvent *event = new writeBufEvent(buf,size);
  QCoreApplication::postEvent(this,event);
  }
void Client::writeServer(QString str)
  {QWriteLocker locker(&lock);
  QString t = str+'\n';
  clo.writeBytes(t.toAscii(),t.length()); 
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
          mutex.lock();
          ActionsToDo = 0;
          mutex.unlock();
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
  {mutex.lock(); dbg=b; mutex.unlock();
  QString str = QString("S_DEBUG;%1").arg(b);
  sendToServer(str);
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
      {fields[i].simplified();
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
  QStringList fields = data.split(PARAM_SEP);
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
  return size;
  }
void Client::socketReadyRead()
  {while(socket->canReadLine())
      {QString str = socket->readLine();
      str = str.simplified();
      if(str.at(0) == ':')
          writeClient(str.mid(1));
      else if(str.at(0) == '?') // answer to a question
          {*result += str.mid(1);
          *result += " ";
          }
      else if(str.contains("!PNGREADY"))// receiving a png image
          {char * buffer = NULL;
          lock.lockForRead();
          uint size = readBuffer(buffer);
          lock.unlock();
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PngFile = outDir + QString("tmp%1.png").arg(identificateur);
          QFile file(PngFile);          
          file.open(QIODevice::WriteOnly |QIODevice::Truncate);
          QDataStream stream(&file);
          stream.writeRawData(buffer,size);
          file.close();
          delete [] buffer;
          if(debug())writeClient("!PNG");
          ChangeActionsToDo(-1);
          }
      else if(str.contains("!PSREADY"))// receiving a ps image
          {char * buffer = NULL;
          uint size = readBuffer(buffer);
          if(size == 0){delete [] buffer;ChangeActionsToDo(-1);return;}
          QString PsFile = QString("image%1.ps").arg(++numFiles);
          QFile file(PsFile);          
          file.open(QIODevice::WriteOnly |QIODevice::Truncate);
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
  {QString str;
   int numAction = todo->size();
   int Action = 0;
  while(Action < numAction)
      {int retry = 0;
      while(pclient->ChangeActionsToDo(0) > 0)
          {msleep(10);// milliseconds
          if(++retry %200 == 0 && pclient->debug())
              pclient->writeClient(QString("Waiting %1s (%2:%3)").arg(retry/100)
                  .arg(pclient->stack.top()).arg(pclient->ChangeActionsToDo(0)));
          }
      str = todo->at(Action++); 
      //cout <<"action:"<<Action<<" "<< (const char *)str.toLatin1()<<endl;
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
