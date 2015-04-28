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
\file Settings.cpp
\ingroup pigaleWindow
\brief Settings
*/

#include <config.h>
#include "pigaleWindow.h"
#include "mouse_actions.h"
#include "gprop.h"
#include <QT/Misc.h>
#include <QT/Handler.h>

#include <QTextBrowser>
#include <QSpinBox>
#include <QColorDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QStatusBar>

#include "GraphGL.h"
#include "GraphSym.h"

#ifdef _WIN32
#undef PACKAGE_PATH
#define PACKAGE_PATH ".."
#elif defined(_WINDOWS)
#undef PACKAGE_PATH
#define PACKAGE_PATH "c:\\Program Files\\Pigale"
#endif

// staticData definitions
bool staticData::showArrow = true;
bool staticData::showOrientation = false;
bool staticData::showExtTbrin = false;
int staticData::showVertex = 0;
int staticData::useDistance = 4;
bool staticData::IsUndoEnable = true;
bool staticData::schnyderRect = false;
bool staticData::schnyderColor= false;
bool staticData::schnyderLongestFace = false;
bool staticData::randomSeed = false;
bool staticData::randomEraseMultipleEdges = false;
bool staticData::randomUseGeneratedCir = true;
int staticData::macroRepeat = 100;
int staticData::macroDelay  = 10;
int staticData::Gen_N1  = 10;
int staticData::Gen_N2  = 10;
int staticData::Gen_M   = 30;
int staticData::MaxNS   = 500;
int staticData::MaxND   = 500;
int staticData::nCut    = 2;
int staticData::sizeImage = 500;
QString staticData::dirImage = QString(PACKAGE_PATH)+QDir::separator()+"images";
QString staticData::fileImage = dirImage + QDir::separator() +"image.png";

void pigaleWindow::SetDocumentationPath() // slot
  {DirFileDoc = QFileDialog::
  getExistingDirectory(this
                       ,tr("Choose the documentation directory Doc")
                       ,"."
                       ,QFileDialog::ShowDirsOnly);
  QFileInfo fi = QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
  if(!DirFileDoc.isEmpty() && fi.exists())
      {browser->setSource(QString("file:///")+DirFileDoc+"/manual.html");
      SaveSettings();
      }
  else
      QMessageBox::information(this,"Pigale Editor"
                               ,tr("I cannot find the inline manual: <em>manual.html</em>")
                               ,QMessageBox::Ok);

  }  
void pigaleWindow::CheckDocumentationPath()
  {QFileInfo fdoc,fi;
#if defined(Q_OS_MAC)
 // we look for the standart location
   QDir dir(QApplication::applicationDirPath()); 
   dir.cdUp();
   DirFileDoc = dir.absolutePath() + QDir::separator() + QString("Resources") ;
   fdoc = QFileInfo(DirFileDoc);
   fi = QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
   if(fdoc.exists() && fdoc.isDir() && fi.exists())
      {browser->setSource(QString("file:///")+DirFileDoc+"/manual.html");
      return;
      }
   /*   
   else
      {dir.cdUp();dir.cdUp();dir.cdUp();
      DirFileDoc = dir.absolutePath() + QDir::separator() + QString("Doc") ;
         fdoc = QFileInfo(DirFileDoc);
         fi = QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
         if(fdoc.exists() && fdoc.isDir() && fi.exists())
            {browser->setSource(QString("file:///")+DirFileDoc+"/manual.html");
            return;
            }    
      }
   */   
   DebugPrintf("doc:%s",(const char *)DirFileDoc.toLatin1());
   QMessageBox::warning(this,"Pigale Editor"
                           ,tr("I cannot find the file <b>manual.html"));
   return;
#else 
  fdoc = QFileInfo(DirFileDoc);
  fi = QFileInfo(DirFileDoc + QDir::separator() + QString("manual.html"));
  if(fdoc.exists() && fdoc.isDir() && fi.exists())
      browser->setSource(QString("file:///")+DirFileDoc+"/manual.html");
  else
      {if(QMessageBox::question(this,"Pigale Editor"
                               ,tr("I cannot find the directory <b>Doc<br>"
                                   "Load manually ?")
                               ,QMessageBox::Ok
                               ,QMessageBox::Cancel) == QMessageBox::Cancel)return;
      SetDocumentationPath(); 
      }
#endif
  }
void pigaleWindow:: ResetSettings()
  {QSettings setting("EHESS-CNRS","Pigale");
  setting.clear();
  LoadSettings();
  UpdatePigaleColors();  
  }
void pigaleWindow:: SaveSettings()
  {QSettings setting("EHESS-CNRS","Pigale");
  QString version = setting.value("/pigale/version","unknown").toString();
  if(version != PACKAGE_VERSION)setting.clear();
  // pigale
  setting.setValue("/pigale/version",PACKAGE_VERSION);
  setting.setValue("/pigale/debugEnable",debug());
  setting.setValue("/pigale/undoEnable",staticData::IsUndoEnable);
  // directories
  setting.beginGroup("directories");
  setting.setValue("TgfFile/input",InputFileName);
  setting.setValue("TgfFile/output",OutputFileName);
  setting.setValue("docDir",DirFileDoc);
  setting.endGroup();
  // geometry
  setting.beginGroup("geometry");
  setting.setValue("size",size());
  setting.setValue("pos", pos());
  setting.endGroup();
  // other
  setting.beginGroup("other");
  setting.setValue("arrowEnable",staticData::ShowArrow());
  setting.setValue("orientationEnable",staticData::ShowOrientation());
  setting.setValue("showExtTbrin",staticData::ShowExtTbrin());
  setting.setValue("randomSeedEnable",staticData::RandomSeed());
  setting.setValue("randomSeed",(int)randomSetSeed());
  setting.endGroup();
  // limits
  setting.beginGroup("limits");
  setting.setValue("slow_algo",staticData::MaxNS);
  setting.setValue("display",staticData::MaxND);
  setting.endGroup();
  // embed
  setting.beginGroup("embed");
  setting.setValue("label/show",staticData::ShowVertex());
  setting.setValue("schnyder/rect",staticData::SchnyderRect());
  setting.setValue("schnyder/longestface",staticData::SchnyderLongestFace());
  setting.setValue("schnyder/color",staticData::SchnyderColor());
  setting.setValue("distance",staticData::UseDistance());
  setting.endGroup();
  // generate
  setting.beginGroup("generators");
  setting.setValue("N1",staticData::Gen_N1);
  setting.setValue("N2",staticData::Gen_N2);
  setting.setValue("M",staticData::Gen_M);
  setting.setValue("EraseMultiple",staticData::RandomEraseMultipleEdges());
  setting.setValue("GeneratedCir",staticData::RandomUseGeneratedCir());
  setting.endGroup();
  // macro
  setting.beginGroup("macro");
  setting.setValue("macroRepeat",staticData::macroRepeat);
  setting.setValue("macroDelay", staticData::macroDelay);
  setting.setValue("macroDir",DirFileMacro);
  setting.endGroup();
  // Printer
//   setting.beginGroup("printer");
//   setting.endGroup();
  // Png 
  setting.beginGroup("image");
  setting.setValue("dirImage",staticData::dirImage);
  setting.setValue("width",staticData::sizeImage);
  setting.endGroup();
   // Font
  setting.beginGroup("font");
  setting.setValue("family",QApplication::font().family());
  setting.setValue("size",QApplication::font().pointSize());
  setting.endGroup();
  // Custom colors
  setting.beginGroup("colors");
  setting.setValue("Background",QColor(QColorDialog::customColor(0)));
  setting.setValue("Base",QColor(QColorDialog::customColor(1)));
  setting.setValue("LightBackground",QColor(QColorDialog::customColor(2)));
  setting.setValue("GreenBackground",QColor(QColorDialog::customColor(3)));
  setting.endGroup();
  }
void pigaleWindow::LoadSettings()
  {QSettings setting("EHESS-CNRS","Pigale");
  //pigale
  debug() = setting.value("/pigale/debugEnable",false).toBool();
  staticData::IsUndoEnable = setting.value("/pigale/undoEnable",true).toBool();
  //directories
  setting.beginGroup("directories");
  InputFileName = setting.value("TgfFile/input",QString(PACKAGE_PATH)+ QDir::separator()
                                +"tgf"+QDir::separator()+ "a.txt").toString();
  OutputFileName = setting.value("TgfFile/output",InputFileName).toString();
  DirFileDoc = setting.value("docDir",QString(PACKAGE_PATH)+QDir::separator()+"Doc").toString();
  setting.endGroup();
  //geometry
  setting.beginGroup("geometry");
  QSize size = setting.value("size", QSize(800, 600)).toSize();  resize(size);
  QPoint pos = setting.value("pos", QPoint(200, 200)).toPoint();  move(pos);
  setting.endGroup();
  // other
  setting.beginGroup("other");
  staticData::ShowArrow() = setting.value("arrowEnable",true).toBool();
  staticData::ShowOrientation() = setting.value("orientationEnable",true).toBool();
  staticData::ShowExtTbrin() = setting.value("showExtTbrin",false).toBool();
  staticData::RandomSeed() = setting.value("randomSeedEnable",false).toBool();
  randomSetSeed() = (int) setting.value("randomSeed",1).toInt();
  setting.endGroup();
  // limits to display and execute slow algos
  setting.beginGroup("limits");
  staticData::MaxNS = setting.value("slow_algo",500).toInt();
  staticData::MaxND = setting.value("display",500).toInt();
  setting.endGroup();
  // Embed
  setting.beginGroup("embed");
  staticData::ShowVertex() = setting.value("label/show",0).toInt();
  staticData::SchnyderRect() = setting.value("schnyder/rect",false).toBool();
  staticData::SchnyderLongestFace() = setting.value("schnyder/longestface",true).toBool();
  staticData::SchnyderColor() = setting.value("schnyder/color",false).toBool();
  staticData::UseDistance() = setting.value("distance",4).toInt();
  setting.endGroup();
  // Macro
  setting.beginGroup("macro");
  staticData::macroDelay = setting.value("macroDelay",5).toInt();
  staticData::macroRepeat = setting.value("macroRepeat",100).toInt();
  DirFileMacro = setting.value("macroDir",QString(PACKAGE_PATH)+QDir::separator()+"macro").toString();
  setting.endGroup();
  // Generator
  setting.beginGroup("generators");
  staticData::Gen_N1 = setting.value("N1",10).toInt();
  staticData::Gen_N2 = setting.value("N2",10).toInt();
  staticData::Gen_M  = setting.value("M",30).toInt();
  staticData::RandomEraseMultipleEdges() = setting.value("EraseMultiple",true).toBool();
  staticData::RandomUseGeneratedCir() = setting.value("GeneratedCir",true).toBool();
  setting.endGroup();
  // Printer
//   setting.beginGroup("printer");
//   setting.endGroup();
  // Png
  setting.beginGroup("image");
  staticData::sizeImage =  setting.value("width",500).toInt();
  staticData::dirImage = setting.value("dirImage",QString(PACKAGE_PATH)+QDir::separator()+"images").toString();
  staticData::fileImage = staticData::dirImage+QDir::separator() + "image.png";
  QDir dir = QDir(staticData::dirImage);
  if(!dir.exists())
      {QFileInfo fi(staticData::dirImage);
      if(QDir(fi.path()).exists())QDir(fi.path()).mkdir(fi.fileName());
      }
  setting.endGroup();
  // Font
  setting.beginGroup("font");
  QDesktopWidget *desktop = QApplication::desktop();
  int h = desktop->height();
  int fontSize = (h > 600) ? 8 : 7;
  QString family = setting.value("family","Helvetica").toString();
  fontSize = setting.value("size",fontSize).toInt();
  QFont font = QFont(family,fontSize);
  QApplication::setFont(font);
  setFont(font);
  setting.endGroup();
  // colors
  setting.beginGroup("colors");
  QColor col = setting.value("Background",QColor(163,163,127)).value<QColor>();
  QColorDialog::setCustomColor(0,col.rgb());
  col = setting.value("Base",QColor(248,238,224)).value<QColor>();
  QColorDialog::setCustomColor(1,col.rgb());
  col = setting.value("LightBackground",QColor(189,187,154)).value<QColor>();
  QColorDialog::setCustomColor(2,col.rgb());
  col = setting.value("GreenBackground",QColor(165,210,180)).value<QColor>();
  QColorDialog::setCustomColor(3,col.rgb());
  setting.endGroup();
  QPalette Palette = QApplication::palette();
  Palette.setColor(QPalette::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QPalette::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QPalette::Button    ,QColor(QColorDialog::customColor(2)));
  QApplication::setPalette(Palette);
  setPalette(Palette);
}

void pigaleWindow::EditPigaleColors()
  {QColor  initial = QColor(248,238,224);
  QColorDialog::getColor(initial,this); 
  UpdatePigaleColors();
  }
void pigaleWindow::UpdatePigaleColors()
  {QPalette Palette = QApplication::palette();
  Palette.setColor(QPalette::Background,QColor(QColorDialog::customColor(0)));
  Palette.setColor(QPalette::Dark,QColor(Qt::black));
  Palette.setColor(QPalette::Base      ,QColor(QColorDialog::customColor(1)));
  Palette.setColor(QPalette::Button    ,QColor(QColorDialog::customColor(2)));
  Palette.setColor(QPalette::Disabled, QPalette::Text, QColor(Qt::darkRed));
  
  LightPalette = QPalette(QColor(QColorDialog::customColor(2)));
  LightPalette.setColor(QPalette::Base,QColor(QColorDialog::customColor(1)));
  LightPalette.setColor(QPalette::Dark,QColor(Qt::black));
  LightPalette.setColor(QPalette::ButtonText,QColor(Qt::darkGreen));
  LightPalette.setColor(QPalette::WindowText,QColor(Qt::black));
  
  QApplication::setPalette(Palette);  setPalette(Palette);
  graph_properties->setPalette(Palette); 
  gSettings->setPalette(LightPalette); gSettings->setAutoFillBackground(true); 
  mouse_actions->setPalette(LightPalette);
  tabWidget->setPalette(LightPalette);
  graphgl->setPalette(Palette);
  graphsym->setPalette(Palette);
  // statusBar 
  QPalette Palette1; 
  Palette1.setColor(QPalette::Window,QColor(QColorDialog::customColor(1)));
  Palette1.setColor(QPalette::WindowText,Qt::darkGreen);
  statusBar()->setPalette(Palette1);
  statusBar()->setAutoFillBackground(true); 
  }
void pigaleWindow::SetPigaleColorsProfile1() //gray
  {QColor col;
  //Background 
  col.setRgb(180,180,180);QColorDialog::setCustomColor(0,col.rgb());
  //LightBackground
  col.setRgb(208,208,208);QColorDialog::setCustomColor(2,col.rgb());
  //Base
  col.setRgb(248,238,224);QColorDialog::setCustomColor(1,col.rgb());
  UpdatePigaleColors();
  }
void pigaleWindow::SetPigaleColorsProfile2()//yellow
  {QColor col;
  //Background
  col.setRgb(163,163,127);QColorDialog::setCustomColor(0,col.rgb());
  //LightBackground
  col.setRgb(189,187,154);QColorDialog::setCustomColor(2,col.rgb());
  //Base
  col.setRgb(248,238,224);QColorDialog::setCustomColor(1,col.rgb());
  UpdatePigaleColors();
  } 
void pigaleWindow::ParseArguments() 
  // if pigale was called with arguments, we may modify some values
  {

//hub    if(qApp->argc() < 1)return;
    if(QCoreApplication::arguments().size())return;
  int narg = QCoreApplication::arguments().size();
  for(int i = 1; i <= narg;i++)
//      {QString argi = QString((const char *)qApp->argv()[i]);
      {QString argi = QCoreApplication::arguments()[i];
      if(argi == "-i")
          {if(i == narg)return;
//hub          InputFileName = (const char *)qApp->argv()[i+1];
          InputFileName = QCoreApplication::arguments()[i+1];
          QFileInfo fi = QFileInfo(InputFileName);
          if(!fi.exists()) 
              {qDebug("%s does not exist",(const char *)InputFileName.toLatin1());exit(0);}
          i++;
          }
      else if(argi == "-o")
          {if(i == narg)return;
          OutputFileName =  QCoreApplication::arguments()[i+1];
//hub          OutputFileName =  (const char *)qApp->argv()[i+1];
          i++;
          }
      else if(argi == "-m")
          {if(i == narg)return;
//hub          MacroFileName =  (const char *)qApp->argv()[i+1];
          MacroFileName = QCoreApplication::arguments()[i+1];
          QFileInfo fi = QFileInfo(MacroFileName);
          if(!fi.exists()) 
              {qDebug("%s does not exist",(const char *)MacroFileName.toLatin1());exit(0);}
          MacroPlay = true;
          i++;
          }
      else if(argi == "--version" || argi == "-v")
          {qDebug("%s version %s",PACKAGE_NAME,PACKAGE_VERSION);
          exit(0);
          }
      else if(argi == "-s")
          Server = true;
      else if(argi == "-h" || argi == "--help")
          {qDebug("Pigale options:\n -i input\n -o output\n -m macro\n -s server\n -h help");
          qDebug("Qt options:\n -style cde|CleanLooks|WindowsXP|MacIntosh|Motif");
          exit(0);
          }
      else
          {qDebug("%s option not recognized",(const char *)argi.toLatin1());
          qDebug("Pigale options:\n -i input\n -o output\n -m macro\n -s server");
          qDebug("Qt options:\n -style cde|CleanLooks|WindowsXP|MacIntosh|Motif");
          exit(0);
          }
      }
  if(Server || MacroPlay)staticData::IsUndoEnable = false;

  }



