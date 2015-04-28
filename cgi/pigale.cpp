#include <cstdlib> 
#include <cstdio> 
#include <iostream>
#include <qstring.h>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDir>
#include "client.h"

using namespace std; 
 
const char * html1 = "\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n    \
<html>\n                                                                \
<head>\n                                                               \
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n \
<META http-equiv=\"pragma\" content=\"no-cache\">\n                     \
<title>Pigale</title>\n                                                 \
<script language=\"JavaScript\" type=\"text/javascript\">\n             \
<!--\n                                                                 \
function minimum(a,b)\n                                                \
  {if(a < b) return a;\n                                               \
  return b;\n                                                          \
  }\n                                                                  \
function largeur()\n                                                   \
 {if(self.innerWidth)//Konqueror, Firefox\n                            \
    return minimum(self.innerWidth,self.outerWidth); \n                \
  else // explorer\n                    \
    return document.body.offsetWidth;\n \
}\n                                     \
function hauteur()\n                       \
{if(self.innerWidth)//Konqueror, Firefox\n                      \
  return minimum(self.innerHeight,self.outerHeight); \n \
 else // explorer\n                     \
   return document.body.offsetHeight;\n \
}\n                                     \
                                        \
function fill_form()\n                                                  \
  {\n                                                                   \
  document.getElementsByName(\"maxx\")[0].value=largeur();\n            \
  document.getElementsByName(\"maxy\")[0].value=hauteur();\n            \
  }\n                                                                   \
-->\n                                                                   \
  </script>\n                                                           \
  </head>\n                                                             \
  <BODY TEXT=\"#00AAAA\" BGCOLOR=\"#501520\">\n                         \
  <FORM method = \"POST\" action=\"pigale.cgi\" onsubmit=\"fill_form()\">\n \
  <INPUT type=hidden name=\"maxx\" value=0>\n                           \
  <INPUT type=hidden name=\"maxy\" value=0>\n                           \
  <CENTER>\n                                                            \
  <H1>Pigale Server Demo</H1>\n                                         \
  </CENTER>\n                                                           \
  <TABLE WIDTH=400>\n                                                   \
  <TR>\n                                                                \
  <TH ALIGN=RIGHT COLSPAN=\"2\">Generator:</TH>\n                       \
  <TD COLSPAN=\"2\"><select name=\"xx_gen\">\n                          \
  \n";

const char *html3 = "\
  </select>\n                                                   \
  </TD></TR>\n                                                  \
  <TR>\n                                                                \
  <TH ALIGN=RIGHT COLSPAN=\"2\">Execute:</TH>\n                         \
  <TD COLSPAN=\"2\"><input type=submit  id=\"xx\"value=\"Go\">\n        \
  </TD></TR>\n                                                          \
  </TABLE>\n                                                            \
  </form>\n                                                             \
  \n";

QString findValue(QString data,QString field)
  {QString str= data.section(field+"=",1);
  return  str.left(str.indexOf('&'));
  }

int findInteger(QString str)
  {bool ok;
  int m = findValue(str,"x_m").toInt(&ok);
  if(!ok)return 30;
  else m = Min(m,100);
  return  Max(3,m);
  }

void printOption(QString & lastCommand,const char *txt)
  {if(lastCommand.size() && lastCommand.contains(txt))
      cout <<"<option selected value=\""<<txt<<"\">"<<txt<<endl;
  else
      cout <<"<option value=\""<<txt<<"\">"<<txt<<endl;
  }

void printHTML(QString &lastCommand,int m)
  {cout<<html1<<endl;
  printOption(lastCommand,"GEN_PLANAR");
  printOption(lastCommand,"GEN_PLANAR_2C");
  printOption(lastCommand,"GEN_PLANAR_3C");
  printOption(lastCommand,"GEN_PLANAR_3R_3C");
  printOption(lastCommand,"GEN_PLANAR_4R_3C");
  printOption(lastCommand,"GEN_PLANAR_4R_BIP");
  printOption(lastCommand,"GEN_PLANAR_BIP_3C");
  cout <<"</select>\n</TD></TR>\n<TR>"<<endl;
  cout <<"<TH ALIGN=RIGHT COLSPAN=\"2\">M (3 <=M<= 100):</TH>"<<endl;
  cout <<"<TD COLSPAN=\"2\"><input name=\"xx_m\" value="<<endl;
  cout <<m <<endl;
  cout <<"  SIZE=4>"<<endl;
  cout <<"</TD></TR>\n<TR>\n"<<endl;
  cout <<"<TH ALIGN=RIGHT COLSPAN=\"2\">Drawing:</TH>\n"<<endl;
  cout<<"<TD COLSPAN=\"2\"><select  name=\"xx_draw\">"<<endl;
  printOption(lastCommand,"VISION");
  printOption(lastCommand,"FPP_FARY");
  printOption(lastCommand,"SCHNYDER_V");
  printOption(lastCommand,"TUTTE_CIRCLE");
  printOption(lastCommand,"POLAR");
  printOption(lastCommand,"POLREC_BFS");
  printOption(lastCommand,"POLYLINE");
  printOption(lastCommand,"T_CONTACT");
  printOption(lastCommand,"SPRING_PM");
  printOption(lastCommand,"CURVES");
  printOption(lastCommand,"EMBED-3d");
  cout<<html3<<endl;
  cout<<"<hr>"<<endl;
  }

int main( int argc, char** argv )
  {const char *CRLF ="\r\n";
  QString data;
  int m;
  cout <<"Content-type: text/html"<<CRLF;
  cout <<CRLF;
  
  char * contentLength = getenv("CONTENT_LENGTH");
  if(contentLength)
      {int dataLength = atoi(contentLength);
      char * input = new char[dataLength+1];
      fread(input, 1, dataLength,stdin);
      input[dataLength] = 0;
      data = QString(input);
      delete  [] input;
      }
  if(data.size())
      {m =  findInteger(data);
      printHTML(data,m);
      }
  else
      {printHTML(data,30);
      cout <<"</BODY>\n</HTML>\n"<<endl;
      return 0;
      }
      
  int nx = findValue(data,"maxx").toInt();
  int ny = findValue(data,"maxy").toInt();
  int sizePng = 550;
  if(nx && ny)sizePng = (int)(.95*Min(nx,ny-280));
  //cout<<"nx:"<<nx<<" ny:"<<ny<<" png:"<<sizePng<<endl;
  QList <QString>  todo;
  //todo <<":D";
  todo <<QString("GEN_M;%1").arg(m);
  todo <<findValue(data,"x_gen");
  QString drawing = findValue(data,"x_draw").toLatin1();
  if(drawing.contains("SPRING") || drawing.contains("CURVES"))
      todo << "TUTTE_CIRCLE";
  todo <<"N"<<"M";
  todo <<drawing;
  todo <<QString("PNG;%1").arg(sizePng) <<":X";
  //todo <<"PNG;550" <<":X";

  // as some navigators may not reload the image
  time_t time_seed;  
  time(&time_seed);  
  int id = (int)time_seed;
  QString outDir = "/var/www/html/images/";
  QString result = "";
  QCoreApplication app(argc,argv);
  Client client("localhost",4242,&todo,id,outDir,&result);
  app.exec();
  
  QString image = QString("tmp%1.png").arg(id);
  cout<<"<CENTER>"<<endl;
  //cout <<"<h3>" << (const char *)result.toAscii() <<"</h3>" <<endl;
  cout <<"<br><b>" << (const char *)result.toAscii() <<"</b><br>"<<endl;
  cout <<"<IMG SRC=\"/images/"<<(const char *)image.toAscii()
       <<"\" ALT=\"missing\">"<<endl;
  cout<<"</CENTER>"<<endl;
  cout <<"</BODY>\n</HTML>\n"<<endl;

  // remove all images older than 30s
  QDateTime now = QDateTime::currentDateTime();
  QDir dir(outDir);
  dir.setFilter(QDir::Files);
  QFileInfoList list = dir.entryInfoList();

  for (int i = 0; i < list.size(); ++i)
      {QFileInfo fileInfo = list.at(i);
      if(fileInfo.suffix() != "png" || 
         !fileInfo.baseName().contains("tmp")) continue;
      QDateTime fileT = fileInfo.created();
      if(fileT.secsTo(now) < 30)continue;
      QFile file(fileInfo.absoluteFilePath());
      file.remove();
      }
  return 0;
  }




