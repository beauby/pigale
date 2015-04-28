/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU
** Public License appearing in the file LICENSE.HTML
** included in the packaging of this file.
**
*****************************************************************************/

/*!
\file Tgf.cpp
\brief The main Pigale file format to save graphs
*/


#include <TAXI/Tbase.h>
#include <TAXI/Tgf.h>

int IsFileTgf(const char *name)
  {char ID[4];
  fstream stream;

  stream.open(name,ios::in|ios::binary); 
  if(!stream.is_open())return -1;
  stream.read(ID,4);
  stream.close();
  if(strncmp(ID,"TGF",3) != 0)return 0;
  return 1;
  }
int Tgf::open(const char *name,open_mode mode)
  {if(mode == old)
      //stream.open(name,ios::in|ios::out|ios::nocreate|ios::binary); 
      {stream.open(name,ios::in|ios::binary); 
      IsOpen = stream.is_open();
      if(!IsOpen)return 0;
      stream.close();
      stream.open(name,ios::in|ios::out|ios::binary); 
      IsOpen = stream.is_open();
      if(!IsOpen)cout<<name<<" is write  protected"<<IsOpen<<endl;
      if(!IsOpen)return 0;
      }
  else
      {stream.open(name,ios::in|ios::out|ios::binary|ios::trunc); 
      IsOpen = stream.is_open();
      if(!IsOpen)return 0;
      }
  
  if(mode == create)
      {if(!WriteHeader())return 0;
      WriteSubHeader();
      }
  else
      {if(!ReadHeader())return 0;
      ReadSubHeader();
      }
  IsGood = 1;
  
  //Il y a toujours un header  (eventuellement juste initialise)
  if(ReadOffsets() == BADFILE)return(-1);
  
  stream.seekp(0, ios::end);
  offset_new_data  = (LongInt)stream.tellp();
  if(mode == create) return 1;
  return Header.RecordNum;
  }
int Tgf::ReadHeader()
  {if(!IsOpen) return 0;
  stream.seekg(0, ios::beg);
  stream.read(Header.ID,4);
  if(strcmp(Header.ID, "TGF") != 0)return 0;
  stream.read((char *)&Header.Version, 2);
  stream.read((char *)&Header.IfdNum,2);
  stream.read((char *)&Header.RecordNum,2);
  stream.read((char *)&Header.LenSubHeader,2);
  stream.read((char *)&Header.FstIfdOffs,4);
  IsGood = stream.good();
  if(!IsGood)return(0);
  return Header.RecordNum;
  }
int Tgf::WriteHeader()
  {if(!IsOpen)return(0);
  stream.seekp(0, ios::beg);
  stream.write(Header.ID,4);
  stream.write((char *)&Header.Version, 2);
  stream.write((char *)&Header.IfdNum, 2);
  stream.write((char *)&Header.RecordNum, 2);
  stream.write((char *)&Header.LenSubHeader, 2);
  stream.write((char *)&Header.FstIfdOffs, 4);
  IsGood = stream.good();
  if(!IsGood)return(0);
  return(1);
  }
int Tgf::ReadOffsets()    //Lecture des Offset de tous les records
  {LongInt offset;
  int nrecord;  

  nrecord = 1;
  offset = IfdOffset(1) = Header.FstIfdOffs;
  //En creation offset = 0;
  while(offset > 0)
      {//Lecture du Ifd.Header
      stream.seekg(offset, ios::beg);
      stream.read((char *)&(Ifd.Header), sizeof(StructIfdHeader));
      CurrentIfdOffset = offset;
      if(Ifd.Header.tag == TAG_FIRST)++nrecord;
      offset = IfdOffset(nrecord) = Ifd.Header.NextRecord;
      }
  nrecord--;
  if(nrecord != Header.RecordNum)return(0);
  return(nrecord);
  }
int Tgf::IfdRead(LongInt offset)    //Lecture d'un record
  {if(!IsGood) return(0);
  stream.seekg(offset, ios::beg);
  stream.read((char *)&(Ifd.Header), sizeof(StructIfdHeader));

  for(int i = 0;i < Ifd.Header.FieldNum;i++)
      {stream.read((char *)&Ifd.field(i).tag,2);    //resize
      stream.read((char *)&Ifd.field[i].attrib,2);
      stream.read((char *)&Ifd.field[i].len,4);
      stream.read((char *)&Ifd.field[i].word,8);
      }
  CurrentIfdOffset = offset;
  if(!stream.good()){stream.clear();IsGood = 0;return(0);}
  return(1);
  }
int Tgf::IfdReadAll(LongInt offset)    //Lecture d'un record
  {if(!IsGood) return(0);
  stream.seekg(offset, ios::beg);
  CurrentIfdOffset = offset;
  int ii = 0;

  for(;;)
      {stream.read((char *)&(Ifd.Header), sizeof(StructIfdHeader));
      for(int i = 0;i < Ifd.Header.FieldNum;i++)
          {stream.read((char *)&Ifd.field(ii).tag,2);   //resize
          stream.read((char *)&Ifd.field[ii].attrib,2);
          stream.read((char *)&Ifd.field[ii].len,4);
          stream.read((char *)&Ifd.field[ii].word,8);
          ++ii;
          }
      if((offset = Ifd.Header.NextIfd) == 0)
          {Ifd.Header.FieldNumTotal = (short)(ii + 1);
          if(!stream.good()){stream.clear();IsGood = 0;return(0);}
          return(1);
          }
      stream.seekg(offset,ios::beg);
      stream.read((char *)&(Ifd.Header), sizeof(StructIfdHeader));
      }
  }
LongInt Tgf::GetTagLength(int Tag)
  {for(int i = 1;i <= TagList.number;i++)
      if(Tag == TagList.tag[i])return TagList.len[i];
  return 0;
  }
int Tgf::CreateRecord()
  {if(new_data)Flush();
  if(NRECORDS < Header.RecordNum)return 0;
  // Find the end of data (!= end of file if records have been deleted)
  // so that the tgf file behave like a stack
  // The end of data is just after the last Ifd
  if(Header.RecordNum)//hf  modified 15/05/2002 before always else case
      {LongInt OffsetLast = IfdOffset[Header.RecordNum];
      stream.seekg(OffsetLast, ios::beg);
      StructIfdHeader header0;
      stream.read((char *)&(header0), sizeof(StructIfdHeader));
      offset_new_data = OffsetLast + sizeof(StructIfdHeader) + header0.FieldNum * 16; 
      stream.seekg(offset_new_data, ios::beg);
      }
  else
      {stream.seekp(0, ios::end); 
      offset_new_data  = (LongInt)stream.tellp();
      }
  ++Header.RecordNum;
  ++Header.IfdNum;
  //Preparation d'un Ifd vide (modified 08/03/2005)
  Ifd.Header.tag = TAG_FIRST;
  Ifd.Header.FieldNum = 0;
  Ifd.Header.FieldNumTotal = 0;
  Ifd.Header.unused = 0;
  Ifd.Header.NextIfd = 0;
  Ifd.Header.NextRecord = 0;
  new_ifd = 1;
  return(1);
  }
int Tgf::SeekWrite(short t,LongInt NumberBytes)
//On n'ecrit effectivement que les grands enregistrements (avant l'Ifd correspondant)
  {int field,align,ii;

  //TEST SI LE TAG EXISTE ou POSITION LIBRE
  short tag;
  for(field = 0; field < Ifd.Header.FieldNum;field++)
      {tag = Ifd.field(field).tag;                      //resize
      if(!tag || tag == t) break;
      }

  //RESIZE
  if(field == Ifd.Header.FieldNum)
      {++Ifd.Header.FieldNum;
      ++Ifd.Header.FieldNumTotal;                      //resize
      Ifd.field(field).len = NumberBytes;
      Ifd.field[field].tag = (short)t;
      stream.seekg(offset_new_data,ios::beg);
      if(seek)                         //Rajouter des 0
          {ii = 0;
          for(align = (offset_new_data - 1) % 4 + 1; align < 4; align++)
              {stream.put('0');++ii;}
          offset_new_data += ii;
          }
      Ifd.field[field].word.l = (LongInt)offset_new_data;
      offset_new_data = (LongInt)stream.tellp() + NumberBytes;
      seek = 1;
      }
  //LE TAG EXISTE DEJA ou est 0
  else
      {//Update de l'Ifd
      Ifd.field[field].tag = (short)t;
      if (NumberBytes <= Ifd.field[field].len)
          { // on ecrit au meme endroit
          Ifd.field[field].len = NumberBytes;
          stream.seekg(Ifd.field[field].word.l, ios::beg);
          }
      else
          {//  on ecrit ailleurs
          stream.seekg(offset_new_data,ios::beg);
          Ifd.field[field].len = NumberBytes;
          if(seek)                         //Rajouter des 0
              {ii = 0;
              for(align = (offset_new_data - 1) % 4 + 1; align < 4; align++)
                  {stream.put('0');++ii;}
              offset_new_data += ii;
              }
          Ifd.field[field].word.l = (LongInt)offset_new_data;
          offset_new_data = (LongInt)stream.tellp() + NumberBytes;
          seek = 1;
          }
      }
  new_data = 1;
  return 1;
  }

int Tgf::FieldWrite(short t,const char *pointeur,LongInt NumberBytes)
//On n'ecrit effectivement que les grands enregistrements (avant l'Ifd correspondant)
  {int field,align,ii;

  //TEST SI LE TAG EXISTE ou POSITION LIBRE
  short tag;
  for(field = 0; field < Ifd.Header.FieldNum;field++)
      {tag = Ifd.field(field).tag;                     //resize
      if(!tag || tag == t) break;
      }

  //RESIZE
  if(field == Ifd.Header.FieldNum)
      {++Ifd.Header.FieldNum;
      ++Ifd.Header.FieldNumTotal;

      Ifd.field(field).len = NumberBytes;             //resize
      Ifd.field[field].tag = (short)t;

      if(NumberBytes <= 8)
          memcpy((void *)&(Ifd.field[field].word), pointeur, NumberBytes);
      else
          {stream.seekg(offset_new_data,ios::beg);
          if(seek)                         //Rajouter des 0
              {ii = 0;
              for(align = (offset_new_data - 1) % 4 + 1; align < 4; align++)
                  {stream.put('0');++ii;}
              offset_new_data += ii;
              seek = 0;
              }
          Ifd.field[field].word.l = (LongInt)offset_new_data;
          stream.write((char *)pointeur, NumberBytes);
          for(align = (NumberBytes - 1) % 4 + 1; align < 4; align++)
              stream.put('0');
          offset_new_data = (LongInt)stream.tellp();
          }
      }
  //LE TAG EXISTE DEJA ou est 0
  else
      {//Update de l'Ifd
      Ifd.field[field].tag = (short)t;
      if(NumberBytes <= 8)
          {Ifd.field[field].len = NumberBytes;
          memcpy((void *)&(Ifd.field[field].word), pointeur, NumberBytes);
          }
      else if (NumberBytes <= Ifd.field[field].len)
          { // on ecrit au meme endroit
          Ifd.field[field].len = NumberBytes;
          stream.seekg(Ifd.field[field].word.l, ios::beg);
          stream.write((char *)pointeur, NumberBytes);
          for(align = (NumberBytes - 1) % 4 + 1; align < 4; align++)
              stream.put('0');
          }
      else
          {//  on ecrit ailleurs
          Ifd.field[field].len = NumberBytes;
          stream.seekg(offset_new_data,ios::beg);
          if(seek)                         //Rajouter des 0
              {ii = 0;
              for(align = (offset_new_data - 1) % 4 + 1; align < 4; align++)
                  {stream.put('0');++ii;}
              offset_new_data += ii;
              seek = 0;
              }
          Ifd.field[field].word.l = (LongInt)offset_new_data;
          stream.write((char *)pointeur, NumberBytes);
          for(align = (NumberBytes - 1) % 4 + 1; align < 4; align++)
              stream.put('0');
          offset_new_data = (LongInt)stream.tellp();
          }
      }
  new_data = 1;
  //DebugPrintf("wrote field tag:%d field:%d (%d %d)",t,field,Ifd.Header.FieldNum,Ifd.Header.FieldNumTotal);
  return(1);
  }
int Tgf::Flush()     //MODIFIE HEADER ou PRECEDENT  et ECRIT IFD
  {if(!new_data)return 1;

  if(!new_ifd){new_data = 0;return(IfdWrite(CurrentIfdOffset));}
  LongInt offset;
  short tag;

  //Padding si seek
  stream.seekg(offset_new_data,ios::beg);
  if(seek)                         //Rajouter des 0
      {int ii = 0;
      for(int align = (offset_new_data - 1) % 4 + 1; align < 4; align++)
          {stream.put('0');++ii;}
      offset_new_data += ii;
      seek = 0;
      }
  //Modification du Header et du precedent Ifd
  //offset_new_data est l'endroit ou l'on va ecrire l'Ifd
  if(Header.IfdNum == 1)
      Header.FstIfdOffs = offset_new_data;
  else
      {offset = IfdOffset(Header.RecordNum - 1);
      stream.seekg(offset, ios::beg);
      stream.read((char *)&tag, 2);
      stream.seekp(offset+12, ios::beg);
      stream.write((char *)&offset_new_data, 4);
      }
  IfdOffset(Header.RecordNum) = offset_new_data;
  WriteHeader();
  new_ifd = new_data = 0;
  //Ecriture du nouvel ifd
  return(IfdWrite(offset_new_data));
  }
int Tgf::IfdWrite(LongInt offset)
  {if(!IsGood) return(0);
  CurrentIfdOffset = offset;
  stream.seekp(offset, ios::beg);
  stream.write((char *)&(Ifd.Header), sizeof(StructIfdHeader));

  for(int i = 0;i < Ifd.Header.FieldNum;i++)
      {stream.write((char *)&Ifd.field[i].tag,2);
      Ifd.field[i].tag = 0;
      stream.write((char *)&Ifd.field[i].attrib,2);
      stream.write((char *)&Ifd.field[i].len,4);
      stream.write((char *)(void *)&Ifd.field[i].word,8);
      }
  if(!stream.good()){IsGood = 0;stream.clear();return(0);}

  if(offset == offset_new_data)      //car si l'Ifd existait on le reecrit
      offset_new_data = (LongInt)stream.tellp();
  return(1);
  }
int Tgf::SeekRead(short t,LongInt NumberBytes)
//Suppose que l'on a lu Ifd complet
  {int i;
  LongInt offset;

  for(i = 0; i < Ifd.Header.FieldNum; i++)
      if (Ifd.field[i].tag == t) break;
  if (i == Ifd.Header.FieldNum) return(0);

  if(Ifd.field[i].len > NumberBytes)return(0);
  NumberBytes = Min(Ifd.field[i].len,NumberBytes);
  offset = Ifd.field[i].word.l;
  stream.seekg(offset,ios::beg);
  return(NumberBytes);
  }
int Tgf::FieldRead(short t,char *pointeur,LongInt NumberBytes)
//Suppose que l'on a lu Ifd complet
  {int i;
  LongInt offset;
  if(!pointeur)return(0);
  for(i = 0; i < Ifd.Header.FieldNum; i++)
      if (Ifd.field[i].tag == t) break;
  if(i == Ifd.Header.FieldNum)return 0;

  if(Ifd.field[i].len > NumberBytes)return(0);
  NumberBytes = Min(Ifd.field[i].len,NumberBytes);
  if(Ifd.field[i].len <= 8)
      memcpy(pointeur, (void *)&(Ifd.field[i].word),NumberBytes);
  else
      {offset = Ifd.field[i].word.l;
      stream.seekg(offset,ios::beg);
      stream.read(pointeur,NumberBytes);
      }
  return(NumberBytes);
  }
int Tgf::SetRecord(int num) //Lecture de l'Ifd complet  et remplissage de TagList
  {if(new_data)Flush();
  if(num <= 0 || num > Header.RecordNum)return(0);
  if(!IfdRead(IfdOffset(num)))return(0);             //resize
  short number = 0;
  for(int i = 0;i < Ifd.Header.FieldNum;i++)
      {if(Ifd.field(i).tag == 0)continue;          //resize
      ++number;
      TagList.tag(number) = Ifd.field[i].tag;      //resize
      TagList.len(number) = Ifd.field[i].len;      //resize
      }
  TagList.number = number;
  return number;
  }
int Tgf::DeleteRecord(int num)
  {if(num < 1 || num > Header.RecordNum)return 0;
  if(Header.RecordNum == 1)
      {Header.RecordNum = 0;
      Header.IfdNum = 0;
      Header.FstIfdOffs = 0;
      WriteHeader();
      return 1;
      }
  //LongInt next = (num < Header.RecordNum) ?  next = IfdOffset[num+1] : 0L;
  LongInt next = (num < Header.RecordNum) ?  IfdOffset[num+1] : 0L;
  if(num > 1)
      {stream.seekp(IfdOffset[num - 1]+12, ios::beg);
      stream.write((char *)&next, 4);
      }
  else
      Header.FstIfdOffs = next;
  Header.RecordNum--;
  Header.IfdNum--; // now as many records as Ifd
  WriteHeader();
  return 1;
  }
