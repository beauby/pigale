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


#include <config.h> 
#include <QT/graphml.h>



MLKEY(V, PROP_COORD, Tpoint, "Coordinates");
MLKEY(V, PROP_COLOR, short, "Color");
MLKEY(V, PROP_LABEL, int, "Label");
MLKEY(E, PROP_COLOR, short, "Color");
MLKEY(E, PROP_WIDTH, int, "Width");

static key_desc key_tab[]={
  MLKEY2(V, PROP_COORD, Tpoint, "Coordinates"),
  MLKEY2(V, PROP_COLOR, short, "Color"),
  MLKEY2(V, PROP_LABEL, int, "Label"),
  MLKEY2(E, PROP_COLOR, short, "Color"),
  MLKEY2(E, PROP_WIDTH, int, "Width")
};

void GraphmlReader::ProcessNode(int v)
{ GraphAccess GA(G);
  for (unsigned int ind=0; ind<sizeof(key_tab)/sizeof(key_desc);ind++)
    { if (key_tab[ind].pnum!=0) continue;
      if (current.data.contains(key_tab[ind].id()))
	{ (*(key_tab[ind].init))(GA);
	  PAccess pa=(*(key_tab[ind].access))(GA);
      pa.vp->fromstr(pa.v,v,(const char *)current.data[key_tab[ind].id()].toLatin1());
	}
    }
}

void GraphmlReader::ProcessEdge(int e)
{GraphAccess GA(G);
  for (unsigned int ind=0; ind<sizeof(key_tab)/sizeof(key_desc);ind++)
    { if (key_tab[ind].pnum!=1) continue;
      if (current.data.contains(key_tab[ind].id()))
	{ (*(key_tab[ind].init))(GA);
	  PAccess pa=(*(key_tab[ind].access))(GA);
      pa.vp->fromstr(pa.v,e,(const char *)current.data[key_tab[ind].id()].toLatin1());
	}
    }
}

static bool CallParse(tstring fname, GraphmlParser &parser)
  {    
    QFile xmlFile(~fname);
    QXmlInputSource source(&xmlFile);
    QXmlSimpleReader reader;
    MyErrorHandler err;
    reader.setErrorHandler(&err);
    reader.setContentHandler(&parser);
    bool res=reader.parse(source);
    return res;
  }

int Taxi_FileIOGraphml::IsMine(tstring fname)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    bool res=CallParse(fname,xreader);
    delete &xreader;
    return (res ? 1 : 0);
  } 

int Taxi_FileIOGraphml::GetNumRecords(tstring fname)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    if (!CallParse(fname,xreader)) return 0;
    int n=xreader.ng();
    delete &xreader;
    return n;
  }

tstring Taxi_FileIOGraphml::Title(tstring fname,int)
  {
    GraphmlReaderInfo &xreader= *new GraphmlReaderInfo();
    if (!CallParse(fname,xreader)) return "???";
    QString title=xreader.Title();
    delete &xreader;
    return (const char *)title.toLatin1();
  }

int Taxi_FileIOGraphml::Read(GraphContainer& G,tstring fname,int& NR,int& index)
  {
    if (index <= 0) index=1;
    GraphmlReaderInfo &inforeader= *new GraphmlReaderInfo(index);
    if (!CallParse(fname,inforeader)) return 1;
    NR=inforeader.ng();
    int nv=inforeader.n();
    int ne=inforeader.m();
    QString title=inforeader.Title();
    QString version=inforeader.Version();
    delete &inforeader;
    if (index > NR)
      { GraphmlReaderInfo &inforeader2= *new GraphmlReaderInfo(index);
	if (!CallParse(fname,inforeader2)) return 1;
	NR=inforeader2.ng();
	nv=inforeader2.n();
	ne=inforeader2.m();
	title=inforeader2.Title();
	version=inforeader.Version();
	delete &inforeader2;
      }
    if (version!="") LogPrintf("Graphml: Pigale version=%s\n",(const char *)version.toLatin1());
    G.clear();
    G.setsize(nv,ne);
    Prop1<tstring> Gtitle(G.Set(),PROP_TITRE);
    Gtitle() = tstring((const char *)title.toLatin1());
    GraphmlReader &graphreader=*new GraphmlReader(G,index);
    if (!CallParse(fname,graphreader)) return 1;
    return 0;
  }

static QString xlabel(GraphAccess &G,tvertex v)
  {QString id;
    if (G.Set().exist(PROP_VSLABEL) && G.Set(tvertex()).exist(PROP_SLABEL))
      { Prop1<svector<tstring *> >vslabel(G.Set(),PROP_VSLABEL);
	Prop<int> slabel(G.Set(tvertex()),PROP_SLABEL,0);
	id=~(*(vslabel()[slabel[v]]));
      }
 //    else if (G.Set(tvertex()).exist(PROP_LABEL))
//     {Prop<int> label(G.Set(tvertex()),PROP_LABEL);
//     id.sprintf("%ld",label[v]);
//     }
    else id.sprintf("n%d",v());
    return id;
  }

int  Taxi_FileIOGraphml::Save(GraphAccess& G,tstring fname)
  {
    QFile xmlFile(~fname);
    if (!xmlFile.open(QIODevice::WriteOnly))
      return 1;
    QTextStream stream( &xmlFile );
    Prop1<tstring> title(G.Set(),PROP_TITRE);
    Prop<tvertex> vin(G.Set(tbrin()),PROP_VIN);
    bool oriented1=G.Set(tedge()).exist(PROP_ORIENTED);
    bool oriented = false; 
    if (oriented1)
      {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	eoriented.getinit(oriented);
      }
    QString otherorient="true";
    if (oriented) otherorient="false";

    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    stream << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << endl;  
    stream << "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<endl;
    stream << "    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns"<<endl;
    stream << "     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << endl;
    stream << "  <key id=\"Pigale/version\" for=\"graph\"";
    stream << " attr.name=\"Pigale version\" attr.type=\"string\">"<<endl;
    stream << "    <default>"<<PACKAGE_VERSION<<"</default>"<<endl;
    stream << "  </key>"<<endl;
    for (unsigned int ind=0; ind<sizeof(key_tab)/sizeof(key_desc); ind++)
      { if ((*(key_tab[ind].isdef))(G)) {
	  stream << "  <key id=\"" << key_tab[ind].id() << "\" ";
	  stream << "for=\"" << key_tab[ind].xsetid() << "\" ";
	  stream << "attr.name=\"" << key_tab[ind].desc << "\" ";
	  stream << "attr.type=\"string\"/>" << endl;
	}
      }
    stream << "  <graph id=\""<<(const char *)title()<<"\" edgedefault=\"";
    if (!oriented) stream << "un";
    stream << "directed\">" << endl;
    for (tvertex v=1; v<=G.nv(); v++)
      {
	stream << "    <node id=\""<<xlabel(G,v)<<"\">"<<endl;
	for (unsigned int ind=0; ind<sizeof(key_tab)/sizeof(key_desc); ind++) {
	  if (key_tab[ind].pnum!=0 || !(*(key_tab[ind].isdef))(G)) continue;
	  PAccess pa=(*(key_tab[ind].access))(G);
	  stream << "      <data key=\"" << key_tab[ind].id() << "\">";
	  stream <<  pa.vp->tostr(pa.v,v()) << "</data>" << endl;
	}
        stream << "    </node>" << endl;
      }
    for (tedge e=1; e<=G.ne(); e++)
      {stream << "     <edge source=\""<<xlabel(G,vin[e])<<"\" target=\""<<xlabel(G,vin[-e])<<"\"";
	if (oriented1)
	  {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	    if (eoriented[e]!=oriented)
	      stream << " directed=\"" << otherorient <<"\"";
	  }
	stream << ">" <<endl;
	for (unsigned int ind=0; ind<sizeof(key_tab)/sizeof(key_desc); ind++) {
	  if (key_tab[ind].pnum!=1 || !(*(key_tab[ind].isdef))(G)) continue;
	  PAccess pa=(*(key_tab[ind].access))(G);
	  stream << "      <data key=\"" << key_tab[ind].id() << "\">";
	  stream <<  pa.vp->tostr(pa.v,e()) << "</data>" << endl;
	}
        stream << "    </edge>" << endl;	
      }
    stream << "   </graph>"<<endl;
    stream << "</graphml>"<<endl;
    xmlFile.close();
    return 0;
  }

void Init_IOGraphml() {Taxi_FileIO::reg(new Taxi_FileIOGraphml);}
