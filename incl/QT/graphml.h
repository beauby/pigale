#ifndef XGRAPHML_H
#define XGRAPHML_H   

#include <QFile>
#include <QMap>
#include <QtXml>

#include <TAXI/graphs.h>
#include <TAXI/Tfile.h>

class QString;

#define PSET_V 0
#define PSET_E 1
#define PSET_B 2

struct PAccess {
  _svector *v;
  vProp *vp;
  PAccess(): v(0),vp(0) {}
};

struct key_desc {
  QString desc;
  int pnum;
  int prop;
  void (*init)(GraphAccess &);
  bool (*isdef)(GraphAccess &);
  PAccess (*access)(GraphAccess &);

  QString setid() { if (pnum==0) return "V"; else if (pnum==1) return "E"; else return "B";}
  QString xsetid() { if (pnum==0) return "node"; else if (pnum==1) return "edge"; else return "???";}
  QString id() { 
    QString num; num.sprintf("%d",prop);
    return "Pigale/"+setid()+"/"+num;
  }
  public:
  key_desc(int s, int n, QString d, void (*_init)(GraphAccess &), 
	   bool (*_isdef)(GraphAccess &), PAccess (*_access)(GraphAccess &))
    :desc(d),pnum(s),prop(n),init(_init),isdef(_isdef),access(_access) {}
};



#define MLKEY(_pset, _prop_num, _prop_type, _prop_desc)	\
  static void mlkey_init##_pset##_prop_num(GraphAccess &G) {Prop<_prop_type>v(G.P##_pset(),_prop_num);}\
  static bool mlkey_isdef##_pset##_prop_num(GraphAccess &G) {return G.P##_pset().defined(_prop_num);} \
  static PAccess mlkey_access##_pset##_prop_num(GraphAccess &G) { \
    PAccess pa;\
    if (mlkey_isdef##_pset##_prop_num(G)) {pa.v=G.P##_pset()[_prop_num];pa.vp=G.P##_pset()(_prop_num);} \
    return pa;} //

#define MLKEY2(_pset, _prop_num, _prop_type, _prop_desc)	\
  key_desc(PSET_##_pset,_prop_num,_prop_desc, \
	   mlkey_init##_pset##_prop_num, mlkey_isdef##_pset##_prop_num, mlkey_access##_pset##_prop_num)

#define PARSE_NUMRECORDS  0
#define PARSE_TITLE       1
#define PARSE_GRAPHINFO   2
#define PARSE_GRAPH       3


#if QT_VERSION >= 0x40000
class MyErrorHandler : public QXmlErrorHandler {
    QXmlParseException *exception;
    QString type;
protected:
    bool warning ( const QXmlParseException & e) 
    {  delete exception;
       exception=new QXmlParseException(e.message(),e.columnNumber(),e.lineNumber(),e.publicId(),e.systemId());
       type="Warning"; return true;}
    bool error ( const QXmlParseException & e)
    {  delete exception;
       exception=new QXmlParseException(e.message(),e.columnNumber(),e.lineNumber(),e.publicId(),e.systemId());
       type="Error";return false;}
    bool fatalError ( const QXmlParseException & e)
    {  delete exception;
       exception=new QXmlParseException(e.message(),e.columnNumber(),e.lineNumber(),e.publicId(),e.systemId());
       type="Fatal error";return false;}
public:
    MyErrorHandler() : exception(0) {}
      ~MyErrorHandler() { delete exception;}

  QString errorString () const
  { QString s; 
    s.sprintf("%s %s : line %d, column %d",
              (const char *)type.toLatin1(), (const char *)exception->message().toLatin1(),
	      exception->lineNumber(), exception->columnNumber()); return s;
  }
};
#else

class MyErrorHandler : public QXmlErrorHandler {
    QXmlParseException exception;
    QString type;
public:
  bool warning ( const QXmlParseException & e) {exception=e; type="Warning"; return TRUE;}
  bool error ( const QXmlParseException & e) {exception=e; type="Error";return FALSE;}
  bool fatalError ( const QXmlParseException & e) {exception=e; type="Fatal error";return FALSE;}
  virtual ~MyErrorHandler() {}

  QString errorString () 
  { QString s; 
    s.sprintf("%s %s : line %d, column %d",
	      (const char *)type, (const char *)exception.message(),
	      exception.lineNumber(), exception.columnNumber()); return s;
  }
};



#endif
class Taxi_FileIOGraphml : public Taxi_FileIO
{
 public:
  int IsMine(tstring fname);
  int Save(GraphAccess& G,tstring fname);
  int Read(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex);
  tstring Title(tstring fname, int index);
  int GetNumRecords(tstring fname);
  int Capabilities(void) { return TAXI_FILE_MINI|TAXI_FILE_RECORD_NUM;}
  const char *Name(void) {return "Graphml file";}
  const char *Ext(void) {return "graphml";}
};

struct GraphmlElmt
{
  QMap<QString,QString> att;
  QMap<QString,QString> data;
  void clear() {att.clear(); data.clear();}
};

class GraphmlParser : public QXmlDefaultHandler
{
 protected:
  int level;                     // Levels to skip
  bool recognized;
  bool record_data;
  bool raised_error;
  bool full_skip;
  enum {rnone, rall, rgraph, rnode, redge} record_key;
  enum {none, key, graph, node, edge } inside;
  GraphmlElmt top;
  GraphmlElmt current;
  QMap<QString,QString> graph_keys;
  QMap<QString,QString> edge_keys;
  QMap<QString,QString> node_keys;
  QString k;

 public:

  bool startDocument();
  bool endDocument();
  bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
  bool characters ( const QString & ch ); 
  
  GraphmlParser() : level(0), recognized(false), record_data(false), raised_error(false),
    full_skip(true), record_key(rnone), inside(none) {}

 protected:

  void skip() {level=1;}
  void record(bool rec) {record_data=rec;}
  void read_error() {raised_error=true;}


  virtual void EnterGraph() {}
  virtual void ExitGraph() {}
  virtual void EnterNode() {}
  virtual void ExitNode() {}
  virtual void EnterEdge() {}
  virtual void ExitEdge() {}

  bool parseTpoint(QString s,Tpoint &p)
  { QStringList coords = s.split(",");
    bool ok;
    QStringList::Iterator it = coords.begin();
    if (it==coords.end() || (p.x()=(*it).toDouble(&ok),!ok) 
	|| ++it==coords.end() || (p.y()=(*it).toDouble(&ok),!ok)
	|| ++it!=coords.end())
      { LogPrintf("Badly formed coordinates: %s\n",(const char *)s.toLatin1());
	read_error(); return false;
      }
    return true;
  }
};


class GraphmlReaderInfo : public GraphmlParser {
  int ngraph;
  int nv;
  int ne;
  int index;
  QString _Title;
  QString Pigale_version;

 public:
  GraphmlReaderInfo(int _index=0) :  ngraph(0), nv(0), ne(0), index(_index), 
    _Title("No name"), Pigale_version("")
    {record(false);}  
  int ng() {return ngraph;}
  int n() {return nv;}
  int m() {return ne;}
  QString Version() {return Pigale_version;}
  QString Title() {return _Title;}
 
 
  void EnterGraph() 
  { if (inside!=none) skip();
    else if (++ngraph!=index) skip(); 
    else _Title=top.att["id"];
  }
  void ExitGraph()
  { if (graph_keys.contains("Pigale/version")) Pigale_version=graph_keys["Pigale/version"];
    if (top.data.contains("Pigale/version")) Pigale_version=top.data["Pigale/version"];
  }
  void EnterNode() {if (inside!=graph) skip(); else ++nv;}
  void EnterEdge() {if (inside!=graph) skip(); else ++ne;}
};

class GraphmlReader : public GraphmlParser {
  int ngraph;
  QString Pigale_version;
  GraphContainer &G;
  Prop<tbrin> vin;
  svector<Tpoint> vcoord;
  svector<bool> has_coord;
  int index;
  int nv;
  int ne;
  QMap<QString,int> V;
  bool has_coords;

  int vindex(QString l) {if (!V.contains(l)) V.insert(l,++nv); return V[l];}

 public:
  GraphmlReader(GraphContainer& _G,int _index) : ngraph(0), Pigale_version(""), G(_G), 
    vin(G.Set(tbrin()),PROP_VIN), vcoord(0,G.nv()), has_coord(0,G.nv(),false),
    index(_index), nv(0), ne(0), has_coords(false)
    {record(true); vin[0]=0;}
  

    void ProcessNode(int v);
    void ProcessEdge(int e);
  
  void EnterGraph() 
  { if (inside!=none) skip(); // nested graph
    else if (++ngraph!=index) skip();
    else if (top.att.contains("edgedefault"))
      {	if (top.att["edgedefault"]=="directed") 
	  {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,true);}
	else if  (top.att["edgedefault"]=="undirected") 
	  {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED,false);}
	else
	  { LogPrintf("unknown value for graph/edgedefault : %s\n",
              (const char *)top.att["edgedefault"].toLatin1());
	    read_error();
	  }
      }
  }
  void EnterNode() {if (inside!=graph) skip();}
  void EnterEdge() {if (inside!=graph) skip();}
  void ExitNode() 
  {
    int v=vindex(current.att["id"]);
    if (current.data.contains("Pigale/V/16"))
      { has_coords=true;
	if (!parseTpoint(current.data["Pigale/V/16"],vcoord[v])) return;
	has_coord[v]=true;
      }
    ProcessNode(v);
  }
  void ExitEdge() 
  {
    int from=vindex(current.att["source"]);
    int to=vindex(current.att["target"]);
    ++ne;
    vin[ne]=from; vin[-ne]=to;
    if (current.att.contains("directed"))
      {Prop<bool> eoriented(G.Set(tedge()),PROP_ORIENTED);
	if (current.att["directed"]=="true")
	  eoriented[ne]=true;
	else
	  eoriented[ne]=false;
      }
    ProcessEdge(ne);
  }
  void ExitGraph() 
  {
    if (nv!=G.nv()) {LogPrintf("Bad vertex number (%d vs %d)\n",nv,G.nv()); read_error(); return;}
    if (ne!=G.ne()) {LogPrintf("Bad edge number (%d vs %d)\n",ne,G.ne()); read_error(); return;}
    if (graph_keys.contains("Pigale/version")) Pigale_version=graph_keys["Pigale/version"];
    if (top.data.contains("Pigale/version")) Pigale_version=top.data["Pigale/version"];    
    if (node_keys.contains("Pigale/V/16"))
      { has_coords=true;
	if (Pigale_version=="")	{LogPrintf("File contains Pigale properties but no version ?!\n");}
	if (node_keys["Pigale/V/16"]!="")
	  { Tpoint p;
	    if (!parseTpoint(node_keys["Pigale/V/16"],p)) return;
	    Prop<Tpoint> _pcoord(G.Set(tvertex()),PROP_COORD,p);
	  }
	Prop<Tpoint> pcoord(G.Set(tvertex()),PROP_COORD);
	for (int i=1; i<=nv;i++)
	  if (has_coord[i]) pcoord[i]=vcoord[i];
      }
    else if (!has_coords) 
      {
	// Calcul des coordonnes
	Prop<Tpoint> pcoord(G.Set(tvertex()),PROP_COORD);
	double angle = 2.0*acos(-1.0)/nv;
	pcoord[0]=Tpoint(0,0);
	for (int i=1; i<=nv; i++)
	  pcoord[i] = Tpoint(cos(angle*i),sin(angle*i));
      }
    else
      { LogPrintf("key \"Pigale/V/16\" not declared");
	read_error();
	return;
      }
    Prop1<svector<tstring *> > vslabel(G.Set(),PROP_VSLABEL);
    bool has_labels = G.Set(tvertex()).exist(PROP_LABEL);
    Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL,0);
    Prop<int> slabel(G.Set(tvertex()),PROP_SLABEL,0);
    vslabel().resize(0,nv);
    vslabel().definit(*(new (tstring *) (0)));
    vlabel[0]=0;
    slabel[0]=0;
    vslabel()[0]=(tstring *)0;
    for (QMap<QString,int>::Iterator it=V.begin(); it!=V.end(); ++it)
        {if(!has_labels)
            vlabel[it.value()]=it.value();
      slabel[it.value()]=it.value();
      vslabel()[it.value()]=new tstring((const char *)it.key().toLatin1());
      }
  }
};

#endif 
