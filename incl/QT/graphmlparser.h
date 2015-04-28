
#ifndef GRAPHMLPARSER_H
#define GRAPHMLPARSER_H   

#include <TAXI/graphs.h>
#include <QtXml>
#include <QMap>

class QString;

#define PARSE_NUMRECORDS  0
#define PARSE_TITLE       1
#define PARSE_GRAPHINFO   2
#define PARSE_GRAPH       3

class StructureParser : public QXmlDefaultHandler
{
  int action;
  int index;
  int N;
  QString T;
  GraphContainer *GC;
  int nv,ne;
  QMap<QString,int> V;
public:
  bool startDocument();
  bool endDocument();
  bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
  bool characters ( const QString & ch ); 
  StructureParser(int _action,int _index=-1, GraphContainer *pGC=(GraphContainer *)0): 
    action(_action),index(_index),N(0),T(""),GC(pGC),nv(0),ne(0),has_coord(0),context(0),data_dest(0) {}
    int NumRecords() {return N;}
    QString Title() {return T;}
    int n() {return nv;}
    int m() {return ne;}
 private:
  QString indent;
  int level;
  int has_coord;
  Tpoint coord;
  int context;
  QString data;
  int data_dest;
};  
                 
class StructureParserCheck : public QXmlDefaultHandler
{
public:
  bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
private:
    QPtrStack<QString > stack;
};  

#endif 
