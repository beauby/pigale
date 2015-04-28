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
\file pigaleFile.cpp
\ingroup pigaleWindow
\brief Loading and saving a graph
*/

#include <config.h>
#include "pigaleWindow.h"
#include "GraphWidget.h"
#include <TAXI/Tgf.h>
#include <QT/Misc.h> 

#include <QFileDialog>
#include <QInputDialog>
#include <QStatusBar>
#include <QMessageBox>


void Init_IO();

void pigaleWindow::load()
  {QFileInfo fi =  QFileInfo(InputFileName);
  QString extension = fi.suffix();
  QStringList formats;
  QString filter, selfilter;
  selfilter = "All (*)";
  LogPrintf("dir:%s  \n",(const char*)fi.path().toLatin1());

  for (int  i = 0; i< IO_n();i++)
      {filter = IO_Name(i);
      filter += "(*";
      if (QString(IO_Ext(i)) != "!") {filter += "."; filter += IO_Ext(i);}
      filter += ")";
      formats += filter;
      if(filter.contains(extension))selfilter=filter;
      }
    formats += "All (*)";
    QString FileName = QFileDialog::getOpenFileName(this
                                                    ,tr("Choose a file to open")
                                                    ,fi.filePath() //,fi.path()
                                                    ,formats.join(";;")
                                                    ,&selfilter);

    if(!FileName.isEmpty())
      {InputFileName = FileName;
      int i = 0;
      for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++i ) 
          {if (selfilter==*it) break;
          }
      if (i == IO_n()) // All
          {i = IO_WhoseIs((const char *)FileName.toLatin1());
          if (i<0) 
              {QString m;
              m.sprintf("Could not read:%s",(const char *)InputFileName.toLatin1());
              statusBar()->showMessage(m,2000);
              LogPrintf("%s: unrecognized format\n",(const char *)InputFileName.toLatin1());
              return;
              }	  
          }
          
      InputDriver = i;
      *pGraphIndex = 1;
      int NumRecords = IO_GetNumRecords(i,(const char *)InputFileName.toLatin1());
      if(NumRecords > 1)
          {bool ok = false;
          QStringList titles;
          QString item;
          for (int j=1; j<=NumRecords; j++)
              {item.sprintf("%d: %s",j,(const char *)IO_Title(i,(const char *)InputFileName.toLatin1(),j));
              titles+=item;
              }
          QString res = QInputDialog::getItem(this,
                                              "Graph Selection",
                                              "Select a record:",
                                              titles,
                                              0,
                                              false,
                                              &ok);
          if(ok) 
              {*pGraphIndex=res.left(res.indexOf(':')).toInt();
              load(0);
              }
          }
      else load(0);
      }
  }
int pigaleWindow::publicLoad(int pos)
// Only called by the server
  {QFileInfo fi(InputFileName);
  QString m;
   if(!fi.exists() || fi.size() == 0)
       {m = QString("file -%1- does not exist").arg(InputFileName);
       LogPrintf("%s\n",(const char *)m.toLatin1());
       setPigaleError(-1,"Non existing file");
       return -1;
      } 
   int i = IO_WhoseIs((const char *)InputFileName.toLatin1());
   if(i < 0) 
       {m = QString("%1: unrecognized format").arg(InputFileName);
     LogPrintf("%s\n",(const char *)m.toLatin1());
     setPigaleError(-1,"unrecognized format");
     return -1;
     }
   InputDriver = i;
  int NumRecords =IO_GetNumRecords(i,(const char *)InputFileName.toLatin1());
  *pGraphIndex = pos;
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(i,GC,(const char *)InputFileName.toLatin1(),NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      LogPrintf("%s\n",(const char *)m.toLatin1());
      return -2;
      }
  QFile file(InputFileName);
  file.remove();
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName.toLatin1(),*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  return *pGraphIndex;
  }
int pigaleWindow::load(int pos)
  {setPigaleError();
  QString m;
  QFileInfo fi(InputFileName);
  if(!fi.exists() || fi.size() == 0)
      {m = QString("file -%1- does not exist").arg(InputFileName);
      if(!ServerExecuting)statusBar()->showMessage(m,2000);
      LogPrintf("%s\n",(const char *)m.toLatin1());
      return -1;
    }      
  if (!IO_IsMine(InputDriver,(const char *)InputFileName.toLatin1()))
    {m.sprintf("file -%s- is not a valid %s",(const char *)InputFileName.toLatin1(),IO_Name(InputDriver));
    if(!ServerExecuting)statusBar()->showMessage(m,2000);
    LogPrintf("%s\n",(const char *)m.toLatin1());
      return -1;
    }
  UndoClear();UndoSave();
  int NumRecords =IO_GetNumRecords(InputDriver,(const char *)InputFileName.toLatin1());
  if(pos == 1)++(*pGraphIndex);
  else if(pos == -1)--(*pGraphIndex);
  if(*pGraphIndex > NumRecords)*pGraphIndex = 1;
  else if(*pGraphIndex < 1)*pGraphIndex += NumRecords;
  if(IO_Read(InputDriver,GC,(const char *)InputFileName.toLatin1(),NumRecords,*pGraphIndex) != 0)
      {m = QString("Could not read:%1").arg(InputFileName);
      if(!ServerExecuting)statusBar()->showMessage(m,2000);
      return -2;
    }
  if(debug())DebugPrintf("\n**** %s: %d/%d",(const char *)InputFileName.toLatin1(),*pGraphIndex,NumRecords);
  Prop<bool> eoriented(GC.Set(tedge()),PROP_ORIENTED,false);
  TopologicalGraph G(GC);
  UndoSave();
  banner();
  information(); 
  gw->editor->update(1);
  return *pGraphIndex;
  }
int pigaleWindow::save(bool manual)
  {TopologicalGraph G(GC);
  /* check overwite is already done
  if(manual) // check overwrite
      {QFileInfo fi =  QFileInfo(OutputFileName);
      if(!(IO_Capabilities(OutputDriver)&TAXI_FILE_RECORD_ADD) && fi.exists() )
          {if(QMessageBox::warning(this,"Pigale Editor"
                                   ,tr("This file already exixts.<br> Overwrite ?")
                                   ,QMessageBox::Ok 
                                   ,QMessageBox::Cancel) == QMessageBox::Cancel)return 0;
          }
      }
  */
  if(manual)// ask for a title
      {Prop1<tstring> title(G.Set(),PROP_TITRE);
      QString titre(~title());
      bool ok = true;
      titre = QInputDialog::getText(this,
                                    "Pigale","Enter the graph name",
                                    QLineEdit::Normal,titre, &ok);
      if(ok && !titre.isEmpty()) title() = (const char *)titre.toLatin1();
      else if(!ok) return -1;
      }
 
  if(IO_Save(OutputDriver,G,(const char *)OutputFileName.toLatin1()) == 1)
      {setPigaleError(-1,QString("Cannot open file:%1").arg(OutputFileName).toLatin1());
      return -1;
      }
  GraphIndex2 = IO_GetNumRecords(OutputDriver,(const char *)OutputFileName.toLatin1());
  banner();
  return 0;
  }
int pigaleWindow::publicSave(QString FileName)
  {TopologicalGraph G(GC);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  if(!title().length())title() = "no name";
  QFileInfo fi =  QFileInfo(FileName);
  QString fileExt =  fi.suffix();
  if(fileExt.length() < 3){setPigaleError(-1,"UNKNOWN EXTENSION");return -1;}
  int driver = -1;
  QString extName;
  for (int i=0; i<IO_n();i++)
      {extName = IO_Ext(i);
      if(fileExt.contains(extName))driver = i;
      }
  if(driver == -1)
      {setPigaleError(-1,"UNKNOWN DRIVER");return -1;}
  OutputFileName = FileName;
  OutputDriver = driver;
  save(false);
  return 0;
  }
void pigaleWindow::saveAs()
  {QFileInfo fi =  QFileInfo(OutputFileName);
    QStringList formats;
    QString filter, selfilter;
    for (int i=0; i<IO_n();i++)
      { filter = tr(IO_Name(i));
	filter += "(*";
	if (QString(IO_Ext(i))!="!") {filter += "."; filter += IO_Ext(i);}
	filter += ")";
	formats += filter;
	if (i==0) selfilter=filter;
      }
    QString FileName = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a filename to save under"),
                                                    fi.filePath(),
                                                    formats.join(";;"),
                                                    &selfilter);
    // &selfilter,QFileDialog::DontConfirmOverwrite);
    if(FileName.isEmpty())return;
    QString ext="";
    int id=0;
   
    for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it, ++id ) 
        if (selfilter==*it) break;
        
    if ((QFileInfo(FileName).suffix() != IO_Ext(id)) && (QString(IO_Ext(id))!=""))
      {FileName += ".";
      FileName += IO_Ext(id);
      }
    OutputFileName = FileName;
    OutputDriver = id;
    save();
  }
void pigaleWindow::deleterecord()
  {if (IO_Capabilities(InputDriver)&TAXI_FILE_RECORD_DEL)
      IO_DeleteRecord(InputDriver,(const char *)InputFileName.toLatin1(),*pGraphIndex);
    else
      {QString m=(const char *)IO_Name(InputDriver);
	m += " does not allow record deletion";
    Twait((const char *)m.toLatin1());
      }
    banner();
  }
void pigaleWindow::switchInputOutput()
  {Tswap(InputFileName,OutputFileName);
  Tswap(InputDriver,OutputDriver);
  Tswap(GraphIndex1,GraphIndex2);
  pGraphIndex   =  &GraphIndex1;
  load(0);
}
void pigaleWindow::NewGraph()
  {setPigaleError();
  statusBar()->showMessage("New graph");
  UndoClear();UndoSave();
  Graph G(GC);
  G.StrictReset();
  Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);
  if(debug())DebugPrintf("**** New graph");
  information();gw->editor->update(1);
  }
void pigaleWindow::previous()
  {load(-1);}
void pigaleWindow::reload()
  {load(0);}
void pigaleWindow::next()
  {load(1);}
