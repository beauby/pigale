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


#include "pigaleWindow.h" 
#include "GraphWidget.h" 
#include <TAXI/Tmessage.h>



static pigaleWindow* mw;
pigaleWindow* GetpigaleWindow()
  {return mw;}
void DefinepigaleWindow(pigaleWindow *father) 
  {mw = father;}

QString  universalFileName(QString const & fileName)
  {QString filename = fileName;
  filename.replace('/', QDir::separator());
  return filename;
  }
QString  getPigaleErrorString()
  {QString m;
  if(getPigaleErrorMsg() &&  strlen(getPigaleErrorMsg()))
      m.sprintf("Error:%d '%s' in %s:%d",getPigaleError(),getPigaleErrorMsg(),getPigaleErrorFile(),getPigaleErrorLine());
  else
      m.sprintf("Error:%d in %s:%d",getPigaleError(),getPigaleErrorFile(),getPigaleErrorLine());
  return m;
  }

QString getVertexLabel(GraphContainer &GC,tvertex v)
  {QString t;
  int prop = staticData::ShowVertex();
  if(prop == -3)
       t = "";
  else if(prop == -2)
      t.sprintf("%2.2d",v());
  else if(prop == -1)
      {if(GC.Set().exist(PROP_VSLABEL) && GC.Set(tvertex()).exist(PROP_SLABEL))
          {Prop<int> slabel(GC.Set(tvertex()),PROP_SLABEL);
          int indexTxt = slabel[v];
          if(indexTxt != 0)
              {Prop1<svector <tstring *> > vslabel(GC.Set(),PROP_VSLABEL);
              t = ~(*(vslabel()[indexTxt]));
              }
          }
      }
  else
      t = ~GC.Set(tvertex()).Value(prop,v());
  return t;
  }
QColor OppCol(QColor & col)
// to draw text on vertices
  {int hue,sat,val;
  col.getHsv(&hue,&sat,&val);
  val = (val <= 192) ? 255 : 0;
  QColor col1;
  col1.setHsv((hue+180)%360,sat/2,val); 
  return col1;
  }
QColor Desaturate(QColor & col)
  {int hue,sat,val;
  col.getHsv(&hue,&sat,&val);
  int val0 = 192;
  val = val0 +(int)(val*((double)(255.-val0)/255.));
  QColor col1;
  col1.setHsv(hue,sat/4,val); 
  //col.setHsv((hue+180)%360,sat/4,val); 
  return col1;
  }

// Debug and message functions
class QtGraphDebug : public GraphDebug 
{ public:
  QtGraphDebug() {GraphDebug::gd=this;}
  void DrawGraph(Graph &) {}
  void DrawGraph()
    {if(!mw)return;
    mw->postDrawG();
    }
  int wait(const char *s)
    {if(!mw)return 0;
    mw->postWait( QString(s));
    return 0;
    }
  void clear()
    {if(!mw)return;
    mw->postMessageClear();
    }
  void printf(const char *fmt,...)
    {if(!mw)return;
    va_list arg_ptr;
    char texte_print[200];
    va_start(arg_ptr,fmt);
    vsprintf(texte_print,fmt,arg_ptr);
    va_end(arg_ptr);
    mw->postMessage(QString(texte_print));
    }
  void printf(QString &s)
    {if(!mw)return;
    mw->postMessage(s);
    }
};
static QtGraphDebug QtDefaultGraphDebug;
// Only for windows
void initGraphDebug()
  {GraphDebug::gd=&QtDefaultGraphDebug;
  }
