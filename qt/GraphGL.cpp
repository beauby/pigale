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
/*! \file GraphGL.cpp
\brief 3D embedding class methods
\ingroup qt */


#include "pigaleWindow.h"
#include "GraphGL.h"
#include "glcontrolwidget.h"
#include "mouse_actions.h"
#include <QT/GLWindow.h>
#include <QT/Misc.h>
#include <QT/pigaleQcolors.h>
#include <TAXI/netcut.h>

#ifdef Q_OS_DARWIN
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <QButtonGroup>
#include <QBoxLayout>
#include <QPainter>
#include <QPrinter>
#include <QSvgGenerator>

class GraphGLPrivate
{public:
  GraphGLPrivate()
      {is_init = false;
      isHidden = true;
      pSG = 0;
      pGC = 0;
      editor = 0;
      idelay = 1;
      edge_width = 2.;  // -> *2 
      vertex_width = 1.;// -> *5
      }
  ~GraphGLPrivate()
      {delete editor; delete pSG; delete pGC;}
  bool is_init; 
  bool isHidden; 
  int delay;
  int idelay;
  bool rX,rY,rZ;
  double edge_width;
  double vertex_width;
  QCheckBox* bt_facet;
  QCheckBox* bt_label;
  QCheckBox* bt_color;
  QSlider *Slider;
  QGroupBox* bt_group;
  pigaleWindow *mw;
  RnEmbedding *pSG;
  GraphContainer *pGC;
  GraphGL  *GL;
  GLWindow *editor;
  RnEmbedding &embed() {return *pSG;}
  GraphContainer &GC() {return *pGC;}
};

//*****************************************************
GraphGL::GraphGL(QWidget *parent,pigaleWindow *mw)
    : QWidget(parent)
  {d = new GraphGLPrivate;
  d->mw = mw;
  d->GL = this;
  d->delay = -1;
  d->rX = d->rY = false;
  d->rZ = true;
  }
GraphGL::~GraphGL()
{ delete d;}
void GraphGL::image(QPrinter *printer,QString suffix)
  {if(!d->is_init)return;
  d->editor->image(printer,suffix);
  }
void GraphGL::print(QPrinter* printer)
  {if(!d->is_init)return;
  d->editor->print(printer);
  }
int GraphGL::update()
  {if(!d->is_init)
      {QVBoxLayout* vb = new QVBoxLayout(this);
      d->editor = new GLWindow(d,this);
      vb->addWidget(d->editor);
      QHBoxLayout* hb = new QHBoxLayout();       vb->addLayout(hb);
      QHBoxLayout* hb2 = new QHBoxLayout();      vb->addLayout(hb2);

      d->Slider = new QSlider(Qt::Horizontal);
      d->Slider->setRange(2,100);d->Slider->setValue(2);
      d->Slider->setMaximumHeight(10);
      connect(d->Slider,SIGNAL(valueChanged(int)),SLOT(delayChanged(int)));
      hb2->addWidget(d->Slider);

      d->bt_facet = new QCheckBox("Facet",this); d->bt_facet->setChecked(false);
      d->bt_label = new QCheckBox("Label",this); d->bt_label->setChecked(true);
      d->bt_color = new QCheckBox("Color",this); d->bt_color->setChecked(false);
      hb->addWidget(d->bt_facet); hb->addWidget(d->bt_label); hb->addWidget(d->bt_color);

      QSpinBox *spin_Edge = new QSpinBox();
      spin_Edge->setRange(1,10);
      spin_Edge->setValue((int)d->edge_width*2); spin_Edge->setPrefix("Ew: ");
      QSpinBox *spin_Vertex = new QSpinBox();
      spin_Vertex->setRange(1,20);
      spin_Vertex->setValue((int)d->vertex_width*5); spin_Vertex->setPrefix("Vw: ");
      hb->addWidget(spin_Edge);hb->addWidget(spin_Vertex);

      d->bt_group = new QGroupBox(this);
      d->bt_group->setFlat(true);
      d->bt_group->setPalette(d->mw->LightPalette);
      d->bt_group->setMaximumHeight(30); 
      d->bt_group->setMinimumWidth(120); 
      QPoint pos = d->bt_group->pos();
      QRadioButton* rb_x = new QRadioButton("X",d->bt_group);
      rb_x->setGeometry(pos.x()+5,pos.y(),35,25);
      QRadioButton* rb_y = new QRadioButton("Y",d->bt_group);
      rb_y->setGeometry(pos.x()+45,pos.y(),35,25);
      QRadioButton* rb_z = new QRadioButton("Z",d->bt_group);
      rb_z->setGeometry(pos.x()+85,pos.y(),35,25);
      QButtonGroup *group1 = new QButtonGroup(d->bt_group); 
      group1->setExclusive(true);
      group1->addButton(rb_x);group1->setId(rb_x,0);
      group1->addButton(rb_y);group1->setId(rb_y,1);
      group1->addButton(rb_z);group1->setId(rb_z,2);
      rb_z->setChecked(true);
      hb->addWidget(d->bt_group);

      spin_X = new QSpinBox();    spin_X->setRange(1,100);
      spin_Y = new QSpinBox();    spin_X->setRange(1,100);
      spin_Z = new QSpinBox();    spin_X->setRange(1,100);
      spin_X->setValue(1);     spin_X->setPrefix("X: ");
      spin_Y->setValue(2);     spin_Y->setPrefix("Y: ");
      spin_Z->setValue(3);     spin_Z->setPrefix("Z: ");
      hb->addWidget(spin_X); hb->addWidget(spin_Y); hb->addWidget(spin_Z);

      d->is_init = true;
      connect(spin_Edge,SIGNAL(valueChanged(int)),SLOT(EdgeWidth(int)));
      connect(spin_Vertex,SIGNAL(valueChanged(int)),SLOT(VertexWidth(int)));
      connect(spin_X,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(spin_Y,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(spin_Z,SIGNAL(valueChanged(int)),SLOT(Reload(int)));
      connect(d->bt_facet,SIGNAL(clicked()),SLOT(Reload()));
      connect(d->bt_color,SIGNAL(clicked()),SLOT(Reload()));
      connect(d->bt_label,SIGNAL(clicked()),SLOT(Reload()));
      connect(group1,SIGNAL(buttonClicked(int)),SLOT(axisChanged(int)));
      }
  else
      {delete d->pSG; delete d->pGC; d->pSG=0; d->pGC=0;}

  this->setPalette(d->mw->LightPalette);
  d->Slider->setPalette(d->mw->LightPalette);

  //Steal the embedding
  if (!d->mw->GC.Set().exist(PROP_RNEMBED)) return -1;
  {Prop1<RnEmbeddingPtr> embedp(d->mw->GC.Set(),PROP_RNEMBED);
  if (embedp().ptr==0) return -1;
  if (d->pSG!=0)
    delete d->pSG;
  d->pSG=embedp().ptr;
  embedp().ptr=0; // to avoid the delete
  }
  d->mw->GC.Set().erase(PROP_RNEMBED);
  GraphContainer &GC= *new GraphContainer(d->mw->GC);
  d->pGC=&GC;
  //int res=ComputeFactEmbed();
  //if (res!=0) return res;
  d->bt_facet->setEnabled(d->embed().facets);
  d->mw->tabWidget->setCurrentIndex(d->mw->tabWidget->indexOf(this));
  d->mw->tabWidget->setTabText(d->mw->tabWidget->indexOf(this),"3-d Embedding");

  spin_X->setRange(0,d->embed().dmax);
  spin_Y->setRange(0,d->embed().dmax);
  spin_Z->setRange(0,d->embed().dmax);
  spin_X->setValue(1);  spin_Y->setValue(2);  spin_Z->setValue(3);
  Reload(1);
  return 0;
  }
void GraphGL:: EdgeWidth(int i)
  {d->edge_width = i /2.;
  d->editor->initialize(false);
  }
void GraphGL:: VertexWidth(int i)
  {d->vertex_width = i/5.;
  d->editor->initialize(false);
  }
void GraphGL::Reload()
  {//if Qt < 3.0 Reload(int i =0) does not work
  d->editor->initialize(false);
  }
void GraphGL::Reload(int )
  {//if(i == 0){d->editor->initialize(false);return;}
  RnEmbedding &em = d->embed();
  int i1 = spin_X->value(); i1 = Min(i1,em.dmax);
  int i2 = spin_Y->value(); i2 = Min(i2,em.dmax);
  int i3 = spin_Z->value(); i3 = Min(i3,em.dmax);
  em.SetAxes(i1,i2,i3);
  d->editor->initialize(true);
  }
void GraphGL::resizeEvent(QResizeEvent* e)
  {if(d->editor)d->editor->initialize(false);
  QWidget::resizeEvent(e);
  }
void GraphGL::delayChanged(int i)
  {if(!d->is_init || d->isHidden)return;
  d->idelay = i;
  d->delay = (d->idelay <= 2) ? -1 : 102 - d->idelay;
  d->editor->setAnimationDelay(d->delay);
  }
void GraphGL::axisChanged(int i)
  {if(i == 0)    // Rotate around X
      {d->rX = true;d->rY = d->rZ = false;}
  else if(i == 1)// Rotate around Y
      {d->rY = true;d->rX = d->rZ = false;}
  else           // Rotate around Z
      {d->rZ = true;d->rX = d->rY = false;}
  }
//*********************************************************

GLWindow::GLWindow(GraphGLPrivate *g,QWidget * parent)
    : GLControlWidget(parent)
    ,glp(g)
  {is_init = false;
  object = 0;
  setAnimationDelay(-1);
  xRot = yRot = zRot = .0;
  scale = 1.;	
  }
GLWindow::~GLWindow()
  {makeCurrent();
  glDeleteLists(object,1);
  }
void GLWindow::print(QPrinter *printer)
  {QRect geo = geometry();
  resize(printer->width(),printer->width());
  //QPixmap pixmap = renderPixmap();
  QImage image = grabFrameBuffer();
  QPainter pp(printer);
  //pp.drawPixmap(0,0,pixmap);
  pp.drawImage(0,0,image);
  setGeometry(geo);
  }
void GLWindow::image(QPrinter *printer,QString suffix)
  {QRect geo = geometry();
  resize(staticData::sizeImage,staticData::sizeImage);
  qApp->processEvents();
  //QPixmap pixmap = renderPixmap();
  QImage image = grabFrameBuffer();
  if(suffix == "png" || suffix == "jpg")
      image.save(staticData::fileImage);
  else if(suffix == "svg") 
      {QSvgGenerator *svg = new QSvgGenerator();
      svg->setFileName(staticData::fileImage);
      svg->setResolution(90); 
      svg->setSize(QSize(width(),height()));
      QPainter pp(svg);
      pp.drawImage(0,0,image);
      //pp.drawPixmap(0,0,pixmap);
      }
  else if(suffix == "pdf" || suffix == "ps")
      {QPainter pp(printer);
      pp.drawImage(0,0,image);
      //pp.drawPixmap(0,0,pixmap);
      }
  setGeometry(geo);
  }
void GLWindow::initializeGL()
  {
  setAutoBufferSwap(true);
  glDepthFunc(GL_LESS);//hide features
  glEnable(GL_DEPTH_TEST);//hide features
  glEnable(GL_LINE_SMOOTH);//pas grand chose

  //qglClearColor(color[Black]);
  GLfloat fog_color[] = {0.25, 0.25, 0.25, 1.0};
  qglClearColor(color[White]);
  glShadeModel(GL_FLAT);
  //fog
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  //glFogi(GL_FOG_MODE, GL_EXP);glFogf(GL_FOG_DENSITY,.15);
  glFogf(GL_FOG_START,8.);  glFogf(GL_FOG_END,10.5);
  //glFogf(GL_FOG_START,8.);  glFogf(GL_FOG_END,12.);GLfloat fog_color[] = {1.0,1.0,1.0, 1.0};
  glFogfv(GL_FOG_COLOR,fog_color);
  // CharSize = glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char *)"0");
  if(object){makeCurrent();glDeleteLists(object,1);}
  object = load(false);
  }
void GLWindow::initialize(bool init) 
  {if(!is_init){is_init=true;initializeGL();return;}
  if(object){makeCurrent();glDeleteLists(object,1);}
  object = load(init);
  updateGL();
  }
GLuint GLWindow::load(bool init)
  {GLfloat ds = (1./40.)*glp->vertex_width;
  if(init) // angles when first display
      {if(glp->mw->ServerExecuting)
          {xRot =  zRot = 15.0;
          yRot = - xRot;
          }
      else
          xRot =  yRot =  zRot = 0.0;
      xTrans = yTrans = 0.0;
      zTrans = -9.0;// -10
      scale = (GLfloat)1.41;
      }
  RnEmbedding &embed = glp->embed();
  GeometricGraph G(glp->GC());
  GLuint list;
  list = glGenLists(1);
  glNewList(list,GL_COMPILE);

  bool WithFaces = G.TestPlanar() && glp->bt_facet->isChecked();
  glLineWidth(glp->edge_width);
  glBegin(GL_LINES);
  tvertex v0,v1;
  GLfloat x0,y0,z0,x1,y1,z1;
  for(tedge e = 1;e <= G.ne();e++)
      {v0 = G.vin[e]; v1 = G.vin[-e];
      x0 = (GLfloat)embed.rx(v0);y0 = (GLfloat)embed.ry(v0);z0 = (GLfloat)embed.rz(v0);
      x1 = (GLfloat)embed.rx(v1);y1 = (GLfloat)embed.ry(v1);z1 = (GLfloat)embed.rz(v1);

      if(G.ecolor[e] == Black && WithFaces)glColor3f(.5,.5,.5);
      //else if(G.ecolor[e] == Black)glColor3f(1.,1.,.8);
      else qglColor(color[bound(G.ecolor[e],1,16)]);
      glVertex3f(x0,y0,z0);      glVertex3f(x1,y1,z1);
      }
  glEnd();
  glColor3f(1.,1.,.0);
  if(WithFaces)loadFaces();
  double dss = ds;
  drawCube(.0,.0,.0, .5*dss);

  if(glp->bt_color->isChecked())
      for(tvertex  v = 1;v <= G.nv();v++)
          drawCube((GLfloat)embed.rx(v),(GLfloat)embed.ry(v),(GLfloat)embed.rz(v),dss,color[G.vcolor[v]]);
  else
      for(tvertex  v = 1;v <= G.nv();v++)
          drawCube((GLfloat)embed.rx(v),(GLfloat)embed.ry(v),(GLfloat)embed.rz(v),dss);

  if(glp->bt_label->isChecked())
      {glLineWidth(1.0);
      for(tvertex  v = 1;v <= G.nv();v++)
          drawLabel(v,(GLfloat)embed.rx(v),(GLfloat)embed.ry(v),(GLfloat)embed.rz(v),ds);
      } 
      
     
 glLineWidth(1.0);
 glBegin(GL_LINES);
 qglColor(Qt::red);   glVertex3f(.0,.0,.0);   glVertex3f(.25,0.,0.);
 qglColor(Qt::green); glVertex3f(.0,.0,.0);   glVertex3f(.0,.25,0.);
 qglColor(Qt::blue);  glVertex3f(.0,.0,.0);   glVertex3f(.0,0.,.25);
 glEnd();
 glEndList();
 return list;
  }
void GLWindow::loadFaces()
  {RnEmbedding &embed = glp->embed();
  GeometricGraph G(glp->GC());
  int m = G.ne();
  G.ZigZagTriangulate();
  svector<tbrin> & Fpbrin = G.ComputeFpbrin();
  GLfloat x1,y1,z1,x2,y2,z2,x3,y3,z3;
  tbrin b;
  for (int i=1; i <= Fpbrin.n(); i++)
      {if(G.FaceWalkLength(Fpbrin[i]) != 3)continue;
      // add a triangle
      b = Fpbrin[i];
      x1 = (GLfloat)embed.rx(G.vin[b]);
      y1 = (GLfloat)embed.ry(G.vin[b]);
      z1 = (GLfloat)embed.rz(G.vin[b]);
      b = -b;
      x2 = (GLfloat)embed.rx(G.vin[b]);
      y2 = (GLfloat)embed.ry(G.vin[b]);
      z2 = (GLfloat)embed.rz(G.vin[b]);
      b = -G.cir[b];
      x3 = (GLfloat)embed.rx(G.vin[b]);
      y3 = (GLfloat)embed.ry(G.vin[b]);
      z3 = (GLfloat)embed.rz(G.vin[b]);
      
      glBegin(GL_TRIANGLES);
      glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2); glVertex3f(x3,y3,z3);
      glEnd();
      }
  delete &Fpbrin;
  for(tedge e = G.ne();e >m;e--)
      G.DeleteEdge(e);
  }
void GLWindow::setAnimationDelay(int ms)
  {GLControlWidget::setAnimationDelay(ms);
  }
void GLWindow::animate()
  {
  if(glp->rX)xRot -= 1.5;
  if(glp->rY)yRot -= 1.5;
  if(glp->rZ)zRot -= 1.5;
//   if(glp->rX){xRot -= 1.5;yRot = zRot = .0;}
//   if(glp->rY){yRot -= 1.5;zRot = xRot = .0;}
//   if(glp->rZ){zRot -= 1.5;yRot = xRot = .0;}
  updateGL();
  }
void GLWindow::paintGL()
  {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//hide features
  if(!object)return;
  glLoadIdentity();
  transform();
  glCallList(object);
  //cout <<"x:"<< xRot <<" y:"<<yRot<<" z:"<<zRot<<endl;
//   cout << "z: "<<zTrans<<endl;
  }
void GLWindow::paintEvent(QPaintEvent *e)
  {//initialize the QLWindow
  if(!is_init){is_init = true;initialize(true);}
  QGLWidget::paintEvent(e);
  } 
void GLWindow::hideEvent(QHideEvent*)
  {
  glp->isHidden = true;
  }
void GLWindow::showEvent(QShowEvent*)
  {if(glp->isHidden)
      {glp->isHidden = false;
      //as now we may load a graph while this window is active
      // we have to reset the speed
//       glp->idelay = 2;
//       glp->mw->mouse_actions->LCDNumber->display(glp->idelay);
//       glp->mw->mouse_actions->Slider->setValue(glp->idelay);
      }
  }
QSize GLWindow::sizeHint() const
  {return QSize(glp->GL->width(),glp->GL->height());
  }
void GLWindow::resizeGL(int w,int h)
  {glViewport(0,0,(GLint)w,(GLint)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //left,right,bottom,top,near,far
  glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
  glMatrixMode(GL_MODELVIEW);
  }
void GLWindow::drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size)
  {GLfloat sat = (GLfloat).1  ;

  glColor3f(sat,sat,1.);
  glBegin(GL_POLYGON);//bas
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x+size,y+size,z-size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  glColor3f(sat,sat,(GLfloat).7);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glEnd();
  glColor3f(1.,sat,sat);//sides
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x-size,y+size,z-size);
  glEnd();
  glColor3f((GLfloat).7,sat,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x+size,y-size,z-size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  glColor3f(sat,(GLfloat)1.,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  glColor3f(sat,(GLfloat).7,sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  }
void GLWindow::drawCube(GLfloat x,GLfloat y,GLfloat z,GLfloat size,const QColor &col)
  {int r,g,b;
  col.getRgb(&r,&g,&b);
  GLfloat xr = (GLfloat)r/255.;
  GLfloat xg = (GLfloat)g/255.;
  GLfloat xb = (GLfloat)b/255.;
  GLfloat sat =1.;
  glColor3f(xr*sat,xg*sat,xb*sat);
  //qglColor(col);
  glBegin(GL_POLYGON);//bas
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x+size,y+size,z-size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  sat = 1.;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glEnd();
  sat = .5;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x-size,y+size,z-size);
  glEnd();
  sat =.5;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x+size,y-size,z-size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  sat = (GLfloat).8;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y-size,z-size);
  glVertex3f(x-size,y-size,z+size);
  glVertex3f(x+size,y-size,z+size);
  glVertex3f(x+size,y-size,z-size);
  glEnd();
  sat =(GLfloat).8;
  glColor3f(xr*sat,xg*sat,xb*sat);
  glBegin(GL_POLYGON);
  glVertex3f(x-size,y+size,z-size);
  glVertex3f(x-size,y+size,z+size);
  glVertex3f(x+size,y+size,z+size);
  glVertex3f(x+size,y+size,z-size);
  glEnd();
  }

void GLWindow::drawLabel(tvertex  v,GLfloat x,GLfloat y,GLfloat z,GLfloat size)
  {QString t =  getVertexLabel(glp->GC(),v);
  if(t.isEmpty())return;
  int len = glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char *)((const char *)t.toLatin1()));
  if(t.length() == 1)len *= 2;
  qglColor(Qt::red);
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y-size*.8,z+size*1.01);
  double xm = .9*size*2/len;
  glScalef(xm,xm*1.2,1);
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();

  //Second cube face
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y+size*.8,z-size*1.01);
  glScalef(xm,xm*1.2,1);
  glRotatef(180., 1.0, 0.0, 0.0 );
  
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();
  
  //Third cube face
  qglColor(Qt::blue);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*1.01,y-size*.8,z-size*.85);
  glScalef(xm,xm*1.2,xm);
  glRotatef(-90., 0.0, 1.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Fourth cube face
  qglColor(Qt::blue);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x+size*1.01,y-size*.8,z+size*.85);
  glScalef(xm,xm*1.2,xm);
  glRotatef(90., 0.0, 1.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Fith cube face
  qglColor(Qt::black);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y-size*1.1,z-size*.8);
  glScalef(xm,xm,1.2*xm);
  glRotatef(90., 1.0, 0.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();

  //Sith cube face
  qglColor(Qt::yellow);
  glPushMatrix();
  glPushMatrix();
  glPushMatrix();
  glTranslatef(x-size*.85,y+size*1.1,z+size*.8);
  glScalef(xm,xm,1.2*xm);
  glRotatef(-90., 1.0, 0.0, 0.0 );
  drawText(GLUT_STROKE_ROMAN,(const char *)t.toLatin1());
  glPopMatrix();
  glPopMatrix();
  glPopMatrix();
  }

void GLWindow::drawText(void * font,const char *txt)
  {for(int i = 0; txt[i];i++)
      glutStrokeCharacter(font,txt[i]);
  }
  
// void GLWindow::drawLabel(tvertex v,GLfloat x,GLfloat y,GLfloat z,GLfloat size)
//   {QFont fnt = QFont("sans");
//   fnt.setStyleStrategy(QFont::OpenGLCompatible); 
//   fnt.setPixelSize(70);
//   QString t =  getVertexLabel(glp->GC(),v);
//   if(t.isEmpty())return;
//   qglColor(Qt::red);
//   //glPushMatrix();
//   //glTranslatef(x-size*.85,y-size*.8,z+size*1.01);
//   renderText(x,y,e,t,fnt);
//   //renderText(.0,.0,.0,t,fnt);
//   //glPopMatrix();
//   }
// #endif
