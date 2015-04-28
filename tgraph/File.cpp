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


#include  <TAXI/Tbase.h>
#include  <TAXI/Tstring.h>
#include  <TAXI/graph.h>
#include  <TAXI/Tpoint.h>
#include  <TAXI/Tgf.h>
#include  <TAXI/Tproptgf.h>
#include  <TAXI/Tfile.h>
#include  <TAXI/Tsmap.h>
#include  <TAXI/color.h>
#include  <TAXI/Tmessage.h>

static Taxi_FileIOHandler *FileIOHandler=0;
int IO_WhoseIs(tstring fname) {return FileIOHandler->WhoseIs(fname);}
int IO_IsMine(int d, tstring fname) {return  FileIOHandler->IsMine(d,fname);}
int IO_Save(int d, GraphAccess& G,tstring fname) {return  FileIOHandler->Save(d,G,fname);}
int IO_Read(int d, GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex) 
{return  FileIOHandler->Read(d,G,fname,NumRecords,GraphIndex);}
int IO_Read(int d, GraphContainer& G,tstring fname) {return  FileIOHandler->Read(d,G,fname);}
int IO_GetNumRecords(int d, tstring fname) {return  FileIOHandler->GetNumRecords(d,fname);}
int IO_DeleteRecord(int d, tstring fname,int index) {return  FileIOHandler->DeleteRecord(d,fname,index);}
int IO_Capabilities(int d) {return  FileIOHandler->Capabilities(d);}
const char *IO_Name(int d) {return  FileIOHandler->Name(d);}
const char *IO_Ext(int d) {return  FileIOHandler->Ext(d);}
tstring IO_Title(int d, tstring fname, int index) {return  FileIOHandler->Title(d,fname,index);}
int IO_n() {return  FileIOHandler->n();}


// tags for TGF file format
#define  TAG_NAME         512
#define  TAG_N            513
#define  TAG_M            514
#define  TAG_ELIST        515
#define  TAG_COORDLAB     516
#define  TAG_VCOLOR       517
#define  TAG_LEDA         518
#define  TAG_VERSION	  519

#define  TAG_VCOORD	      520
#define  TAG_VLABEL	      521
#define  TAG_VIN	      522
#define  TAG_ECOLOR	      523
#define  TAG_ELABEL	      524
#define  TAG_EWIDTH	      525
// TAG 526 reserved

// reserve TAG de 4096 a 8191

#pragma pack(4)
// pragma is needed on 64 bits compiler to read old tgf files where sizeof(coord)=20
// otherwise sizeof(coord)=24
struct coord   {int label; double x,y ;};
struct e_struct{LongInt v1,v2,color,width;};

int IsFileAscii(const char *name)
  {char ID[4];
  fstream stream;

  stream.open(name,ios::in|ios::binary); 
  if(!stream.is_open())return -1;
  stream.read(ID,4);
  stream.close();
  if(strncmp(ID,"PIG:",4) != 0)return 0;
  return 1;
  }

int DeleteTgfRecord(tstring filename,int index)
  {if(!IsFileTgf(~filename))return -1;
  Tgf file;
  if(file.open(~filename, Tgf::old) == 0)return -1;
  return file.DeleteRecord(index);
  }

int GetTgfNumRecords(tstring fname)
  { if(!IsFileTgf(~fname))
      {ifstream  str(~fname);
      if(!str)return -2;
      return 1;
      }

  Tgf file;
  if(file.open(~fname, Tgf::old) == 0)return -1;
  return file.RecordsNumber();
  }
int ReadGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex)
  {
    int d=IO_WhoseIs(fname);
    if (d<0) return d;
    return IO_Read(d,G,fname,NumRecords,GraphIndex);
    
// if(IsFileTgf(~fname))
//       return ReadTgfGraph(G,fname,NumRecords,GraphIndex);
//   NumRecords = 1; GraphIndex = 1;
//   return ReadGraphAscii(G,fname);
  }
int ReadGeometricGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex)
  {int rc;
  if ((rc=ReadGraph(G,fname,NumRecords,GraphIndex))!=0)
     return rc;
  if (! G.Set(tvertex()).exist(PROP_COORD))
     { Prop<Tpoint> vcoord(G.PV(),PROP_COORD);
     int i;
     // Calcul des coordonnes
     double angle = 2.*acos(-1.)/G.nv();
     vcoord[0]=Tpoint(0,0);
     for (i=1; i<=G.nv(); i++)
         vcoord[i] = Tpoint(cos(angle*i),sin(angle*i));
     }
  return 0;
  }

int ReadTgfGraph(GraphContainer& G,tstring fname,int& NumRecords,int& GraphIndex)
  {if(!IsFileTgf(~fname))return 1;
  Tgf file;
  int i,j,n, m;

  if(file.open(~fname, Tgf::old) == 0)return 2;
  NumRecords = file.RecordsNumber();
  if(GraphIndex > NumRecords)GraphIndex = NumRecords;
  if(GraphIndex <= 0)GraphIndex = 1;
  file.SetRecord(GraphIndex);

  G.clear();
  short version;
  if(!file.FieldRead(TAG_VERSION,version))version = 0;

  if(version >= 1)
      {ReadTGF(G.Set(),file,0);
      Prop1<int> n(G.Set(),PROP_N);
      Prop1<int> m(G.Set(),PROP_M);
      G.setsize(n(),m());
      ReadTGF(G.Set(tvertex()),file,0);
      ReadTGF(G.Set(tedge()),file,1);
      ReadTGF(G.Set(tbrin()),file,2);
      if(version == 1)// as PROP_NLOOPS has changed
          G.Set().erase(PROP_NLOOPS);
      return 0;
      }

  file.FieldRead(TAG_N, n);
  file.FieldRead(TAG_M, m);
  int Titlesize = file.GetTagLength(TAG_NAME);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  if(Titlesize)
      {char *tmp = new char[Titlesize +1];
      file.FieldRead(TAG_NAME,tmp);
      tmp[Titlesize] = 0;
      title() = tstring(tmp);
      delete [] tmp;
      }
  else
      title() = "No Name";
            
  G.setsize(n,m);

  smap<int> map_label;                   //label -> index

  // Creation des aretes
  Prop<int> elabel(G.Set(tedge()),PROP_LABEL);
  Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR,Black);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH,1);
  Prop<tvertex> vin(G.Set(tbrin()),PROP_VIN);

  e_struct *es = new e_struct[m+1];
  int bytes =  file.FieldRead(TAG_ELIST, (char *)&es[1]);
  int a;
  int nloops = 0;
  for (j = 1,i=1; j <= m; j++)
      {if (es[j].v1 == es[j].v2){++nloops;continue;}   // skip loops
      if (map_label.ExistingIndexByKey(es[j].v1) < 0)
          {vlabel[i]=es[j].v1; 
          map_label[es[j].v1]=i++;
          }
      if (map_label.ExistingIndexByKey(es[j].v2) < 0)
          {vlabel[i]=es[j].v2; 
          map_label[es[j].v2]=i++;
          }         
          
      vin[j] = map_label[es[j].v1];
      vin[-j] = map_label[es[j].v2];
      elabel[j] = j;
      a =  (es[j].color  <= 0) ? 1 : ((es[j].color >= 256) ? 256 :(short)es[j].color);
      ecolor[j] =  (short)a;
      a =  (es[j].width  <= 0) ? 1 : ((es[j].width > 3) ? 1 :(short)es[j].width);
      ewidth[j] =  a;
      }
  vin[0]=0;
  delete [] es;
  if(nloops)
      {DebugPrintf("Graph have %d loops",nloops);
      m -= nloops;
      G.setsize(tedge(),m);
      }

  // Creation des sommets et Lecture des coordonnes.
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  vcoord.SetName("TFILE:vcoord");
  coord *coords = new coord[n+1];
   
  bytes= file.FieldRead(TAG_COORDLAB, (char *)&coords[1]);
  
  for(j = 1;j <= n;j++)
      {if(Abs(coords[j].x) < DBL_EPSILON )coords[j].x=.0;
      if(Abs(coords[j].y) < DBL_EPSILON )coords[j].y=.0;
      // isolated vertices have no map_label
      if(map_label.ExistingIndexByKey(coords[j].label) < 0) map_label[coords[j].label] = i++; 
      vcoord[map_label[coords[j].label]] = Tpoint(coords[j].x,coords[j].y);
      }
      
  delete [] coords;

  short LedaGraph = file.FieldRead(TAG_LEDA,LedaGraph) ? 1 : 0;
  if(!LedaGraph)
      {double xmax = 0.; double ymax = .0;
      for(j = 1; j <= n;j++)
          {xmax = Max(xmax,vcoord[j].x());
          ymax = Max(ymax,vcoord[j].y());
          }
      double xx = 90./xmax;
      double yy = 70./ymax;
      for(j = 1; j <= n;j++)
          {vcoord[j].x() = xx*vcoord[j].x() + 5.;
          vcoord[j].y() = yy*vcoord[j].y() + 5.;
          }
      }

  // Lecture des couleurs des sommets
  if(file.GetTagLength(TAG_VCOLOR) == (LongInt)(n*sizeof(short)))
      {Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR,Yellow);
      file.FieldRead(TAG_VCOLOR, (char *)&vcolor[1]);
      }
  return 0;
  }

tstring ReadTgfGraphTitle(tstring fname,int index)
  {if(!IsFileTgf(~fname))return 1;
  Tgf file;

  if(file.open(~fname, Tgf::old) == 0)return 2;
  if ((index > file.RecordsNumber()) || (index <= 0)) return "";
  file.SetRecord(index);

  short version;
  if(!file.FieldRead(TAG_VERSION,version))version = 0;
  if(version >= 1)
    {
      PSet1 Gen;
      ReadTGF(Gen,file,0);
      Prop1<tstring> title(Gen,PROP_TITRE);
      return title();
    }
  else
    { int Titlesize = file.GetTagLength(TAG_NAME);
      tstring title="No Name";
      if(Titlesize)
	{char *tmp = new char[Titlesize +1];
	  file.FieldRead(TAG_NAME,tmp);
	  tmp[Titlesize] = 0;
	  title=tmp;
	  delete [] tmp;
	}
      return title;
    }
  }

int SaveGraphTgf(GraphAccess& G,tstring filename,int tag)
  {if(filename == "")return 1;
  Tgf file;
  if(IsFileTgf(~filename) == 1)
      {if((file.open(~filename, Tgf::old)) == 0)
          {file.close();
          if((file.open(~filename, Tgf::create)) == 0)return 1;
          }
      }
  else
      {if((file.open(~filename, Tgf::create)) == 0)
          return 1;
      }

  file.CreateRecord();
// tag should be 2 in Pigale
  if(tag >= 1)
      {file.FieldWrite(TAG_VERSION,(short)tag);
      Prop1<int> n(G.Set(),PROP_N); n()= G.nv();
      Prop1<int> m(G.Set(),PROP_M); m()= G.ne();
      WriteTGF(G.Set(),file,0);
      WriteTGF(G.Set(tvertex()),file,0);
      WriteTGF(G.Set(tedge()),file,1);
      WriteTGF(G.Set(tbrin()),file,2);
      return 0;
      }
// Old Format not used anymore
// write name, N and M
  int n = G.nv();
  int m = G.ne();
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  if(title().length() == 0)title() = "No Name";
  file.FieldWrite(TAG_NAME, ~title());
  file.FieldWrite(TAG_N, n);
  file.FieldWrite(TAG_M, m);
  file.FieldWrite(TAG_LEDA,(short)1);

  Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);
  Prop<short> ecolor(G.Set(tedge()),PROP_COLOR);
  Prop<int> ewidth(G.Set(tedge()),PROP_WIDTH);
  Prop<tvertex> vin(G.Set(tbrin()),PROP_VIN);

  // write edge list
  e_struct *elist = new e_struct[m+1];
  int i;
  for(i=1;i<=m;i++)
      {elist[i].v1 = vlabel[vin[i]];
      elist[i].v2 = vlabel[vin[-i]];
      elist[i].color = (LongInt)ecolor[i];
      elist[i].width = (LongInt)ewidth[i];
      }
  file.FieldWrite(TAG_ELIST,(char *)&elist[1],m * sizeof(e_struct));
  delete [] elist;
  // write coordinates
  Prop<Tpoint> vcoord(G.Set(tvertex()),PROP_COORD);
  coord *coords = new coord[n+1];
  for (i=1; i<=n;i++)
      {coords[i].label = vlabel[i];
      coords[i].x = vcoord[i].x();
      coords[i].y = vcoord[i].y();
      }
  file.FieldWrite(TAG_COORDLAB,(char *)&coords[1],n * sizeof(coord));
  delete [] coords;
  // write color vertices
  Prop<short> vcolor(G.Set(tvertex()),PROP_COLOR);
  file.FieldWrite(TAG_VCOLOR,(char *)&vcolor[1],n * sizeof(short));
  return 0;
  }
tstring ReadAsciiGraphTitle(tstring filename)
  { 
  ifstream stream(~filename,ios::binary);
  if(!stream)return -1;
  char titre[80];
  int i,ch;

  // Lecture du header (1 ligne)
  for(i = 0;i < 80;i++)
      {ch = stream.get();
      if(ch == EOF)
          return 1;
      else if(ch == 10 || ch == 13)
          break;
      }
  // Lecture du titre
  for(i = 0;i < 80;i++)
      {ch = stream.get();
      if(ch == EOF)
          return 1;
      else if(ch == 10 || ch == 13)
          break;
      else
          titre[i] = (char)ch;
      }
  titre[i] = (char) 0;
  return tstring(titre);
  }


int ReadGraphAscii(GraphContainer& G,tstring filename)
  { // Lecture d'une liste d'aretes
  ifstream stream(~filename,ios::binary);
  if(!stream)return -1;
  char titre[80];
  int i,ch,iv1,iv2;
  G.clear();
  // Lecture du header (1 ligne)
  for(i = 0;i < 80;i++)
      {ch = stream.get();
      if(ch == EOF)
          return 1;
      else if(ch == 10 || ch == 13)
          break;
      }
  // Lecture du titre
  for(i = 0;i < 80;i++)
      {ch = stream.get();
      if(ch == EOF)
          return 1;
      else if(ch == 10 || ch == 13)
          break;
      else
          titre[i] = (char)ch;
      }
  titre[i] = (char) 0;
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  title() = titre;

  smap<int>  map_label;      //label -> index
  svector<int> map_index;   //index -> LongInt

  streampos pos = stream.tellg();
  int m = 0;
  int n = 0;
  // Lecture des aretes
  for(;;)
      {if(!stream.good())return 1;
      stream >> iv1 >> iv2;
      if(!iv1 && !iv2)break;
      if(iv1 == iv2)continue;
      if(map_label.ExistingIndexByKey(iv1)< 0)
          {map_label[iv1] = ++n;
          map_index(n) = iv1;
          }
      if(map_label.ExistingIndexByKey(iv2)< 0)
          {map_label[iv2] = ++n;
          map_index(n) = iv2;
          }
      ++m;
      }
  G.setsize(n,m);
  Prop<int> vlabel(G.PV(),PROP_LABEL);
  Prop<int> elabel(G.PE(),PROP_LABEL);
  Prop<tvertex> vin(G.PB(),PROP_VIN);
  stream.seekg(pos, ios::beg);

  // Re-lecture des aretes
  tbrin b = 0;
  int m1=0;
  for(;;)
      {stream >> iv1 >> iv2;
      if(!iv1 && !iv2)break;
      if(iv1 == iv2)continue;
      ++b;
      vin[b] = (tvertex)map_label[iv1];
      vin[-b] = (tvertex)map_label[iv2];
      ++m1;
      }
  vin[0]=0;
  for (i=0; i<=m;i++) elabel[i] = i;
  for (i=0; i<=n;i++) vlabel[i] = map_index[i];

  Prop<Tpoint> vcoord(G.PV(),PROP_COORD);
  // Calcul des coordonnes
  double angle = 2.*acos(-1.)/n;
  vcoord[0]=Tpoint(0,0);
  for (i=1; i<=n; i++)
      vcoord[i] = Tpoint(cos(angle*i),sin(angle*i));
  return 0;
  }

int SaveGraphAscii(GraphAccess& G,tstring filename)
  {ofstream out(~filename,ios::binary|ios::trunc);
  Prop1<tstring> title(G.Set(),PROP_TITRE);
  out << "PIG:0" << endl;
  if(title().length() == 0)title() = "No Name";
  out << title() <<endl;
  Prop<tvertex> vin(G.Set(tbrin()),PROP_VIN);
  bool existVlabel = G.Set(tvertex()).exist(PROP_LABEL);
 Prop<int> vlabel(G.Set(tvertex()),PROP_LABEL);
 if(!existVlabel)for(int i = 0;i <= G.nv();i++)vlabel[i] = i;
  // write edge list
  for(int e = 1;e <= G.ne();e++)
      //      out << vin[(tbrin)e]() << " " << vin[(tbrin)-e]() << endl;
      out << vlabel[vin[(tbrin)e]()] << " " << vlabel[vin[(tbrin)-e]()] << endl;
  out << "0 0" << endl;
  return 0;
  }

void Init_IO() {
  if (FileIOHandler==(Taxi_FileIOHandler *)0)
    {FileIOHandler = new Taxi_FileIOHandler;
       Taxi_FileIO::reg(new Taxi_FileIO_Tgf,0);
       Taxi_FileIO::reg(new Taxi_FileIO_ASCII,1);
    }
}

void Taxi_FileIO::reg(Taxi_FileIO *p,int where) {Init_IO(); FileIOHandler->add(p,where);}

