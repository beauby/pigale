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

           
#include <TAXI/Tpoint.h>
#include <TAXI/Tbase.h>
#include <QT/graphml.h>
#include <QString>


bool GraphmlParser::startDocument()
{
  level = 0;
  k="";
  return true;
}
bool GraphmlParser::endDocument()
{
  return true;
}
bool GraphmlParser::startElement( const QString& , const QString& , 
				  const QString& qName, 
				  const QXmlAttributes& attributes)
{ 
  if (level>0) {level++; return true;}
  if (qName=="graphml") recognized=true;
  else if (qName=="graph") 
    { top.clear();
      for (int i=0; i<attributes.length();i++)
	top.att.insert(attributes.qName(i),attributes.value(i));
      EnterGraph(); if (raised_error) return false;
      if(level==0) inside=graph; 
    }
  else if ((qName=="node") || (qName=="edge"))
    {
      current.clear();
      for (int i=0; i<attributes.length();i++)
	current.att.insert(attributes.qName(i),attributes.value(i));
      if (qName=="node") {EnterNode(); if (raised_error) return false; if(level==0) inside=node;}
      else if (qName=="edge") {EnterEdge(); if (raised_error) return false; if(level==0) inside=edge;}
    }
  else if (qName=="key")   
    { k=attributes.value("id"); 
      if (attributes.value("for")=="all") 
	    {record_key=rall; graph_keys.insert(k,""); node_keys.insert(k,""); edge_keys.insert(k,"");}
      else if (attributes.value("for")=="graph") {record_key=rgraph; graph_keys.insert(k,"");}
      else if (attributes.value("for")=="node") {record_key=rnode; node_keys.insert(k,"");}
      else if (attributes.value("for")=="edge") {record_key=redge; edge_keys.insert(k,"");}
      else 
          {LogPrintf("no 'for' valid attributes for key (%s)\n"
                     ,(const char *)attributes.value("for").toLatin1());
          return false;
	      }
      inside=key;
    }
  else if (qName=="data")  
    { k=attributes.value("key");
      if (inside==graph)
	    {if (!graph_keys.contains(k)) 
          { LogPrintf("Graph key %s not declared\n",(const char *)k.toLatin1());
          return false;
	      }
	     else top.data.insert(k, graph_keys[k]);
	    }
      else if (inside==node)
	    {if (!node_keys.contains(k))
           { LogPrintf("Node key %s not declared\n",(const char *)k.toLatin1());
           return false;
	       }
	    else current.data.insert(k, node_keys[k]);
	    }
      else if (inside==edge)
	     {if (!edge_keys.contains(k))
            { LogPrintf("Edge key %s not declared\n",(const char *)k.toLatin1());
            return false;
	        }
	     else current.data.insert(k, edge_keys[k]);
	     }
    }
  else if (qName=="default") ;
  else {level++; full_skip=true;}
  if (!recognized) 
    { LogPrintf("<graphml> tag not found before %s\n",(const char *)qName.toLatin1());
      return false;
    }
  return true;
}

bool GraphmlParser::endElement( const QString&, const QString& , const QString& qName )
{ if (level>0 && (--level>0 || full_skip)) return true;
  full_skip=false;
  if (qName=="graph") {inside=none; ExitGraph(); if (raised_error) return false;}
  else if (qName=="node") {inside=graph; ExitNode(); if (raised_error) return false;}
  else if (qName=="edge") {inside=graph; ExitEdge(); if (raised_error) return false;}
  else if (qName=="key")  
    { k=""; record_key=rnone; inside=none;
    }
  else if (qName=="data") 
    { k="";}
  else if (qName=="default") ;
  else if (qName=="graphml") ;
  else 
    { LogPrintf("Unskipped unknown tag %s closed\n", (const char *)qName.toLatin1());
      return false;
    }
  return true;
}

bool GraphmlParser::characters ( const QString & ch )
{  QString _data = ch.simplified();
  if (level>0) return true;
  if (_data.length()>0) 
    {if (inside==key)
      { if ((record_key==rgraph) || (record_key==rall))
	  graph_keys.insert(k,_data);
	if ((record_key==rnode) || (record_key==rall))
	  node_keys.insert(k,_data);
	if ((record_key==redge) || (record_key==rall))
	  edge_keys.insert(k,_data);
      }
    else if (record_data) 
      {if (inside==node || inside==edge) current.data.insert(k,_data);
      else if (inside==graph) top.data.insert(k,_data);
      }
    }
  return true;
}

