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
\file Tgf.h
\brief The main Pigale file format to save graphs
*/

#ifndef _TGF_H_INCLUDED_
#define _TGF_H_INCLUDED_

#include <TAXI/Tbase.h>
#include <TAXI/Tsvector.h>

typedef int LongInt;

//! \brief TGF uses a very primitive dynamic array:
template<class T> class TSArray
{private:
    T *ary;
    int size;
    T init;

    void extend(int idx)
        {int new_size = size;
        T *new_ary;
        if (new_size <= 0) new_size = 1;
        while (new_size <= idx) new_size *= 2;
        new_ary = new T[new_size];
        for (int i = 0; i < size; i++) new_ary[i] = ary[i];
        for (int i = size; i < new_size; i++) new_ary[i] =  init;
        size = new_size;
        if(ary)delete [] ary;
        ary = new_ary;
        }

public:
    TSArray(T _init,int _size = 0) : size(_size),init(_init)
        {if(size == 0){ary = NULL;return;}
        ary = new T[size];
        for(int i = 0; i < size; i++) ary[i] = init;
        }

    ~TSArray(){if(ary)delete [] ary;}


    T & operator[] (int idx)
        {return ary[idx];
        }

    const T & operator[] (int idx) const
        {return ary[idx];
        }

    T & operator() (int idx)
        {if(idx >=size)extend(idx);
         return ary[idx];
        }

};

const int BADFILE = -1;
const int NRECORDS = 200;

union UnionLongWord
    {double d;
    float f;
    LongInt l;
    int i;
    short s;
    char c;
    UnionLongWord() : d(.0) {}
    };

// HEADER
struct StructHeader
    {char ID[4];            //!< "TGF" to recognize the file
    short Version;          //!< version number 
    short IfdNum;           //!< number of Ifd
    short RecordNum;        //!< number of records
    short LenSubHeader;     //!< size of SubHeader (16 by default)
    LongInt FstIfdOffs;        //!< offset of the first Ifd 
    };

// FIELD
struct StructField
    {short tag;             //2 bytes
    short attrib;           //2 bytes
    LongInt len;               //4 bytes
    UnionLongWord word;     //8 bytes
    StructField() : tag(0),attrib(0),len(0),word(){}
    };

// IFD HEADER
struct  StructIfdHeader
    {short tag;             //2 bytes
    short FieldNumTotal;    //2 bytes
    short FieldNum;         //2 bytes      # de champs de l'Ifd
    short unused;           //2 bytes      unused
    LongInt NextIfd;           //4 bytes
    LongInt NextRecord;        //4 bytes
    StructIfdHeader() : tag(0),FieldNumTotal(0),FieldNum(0),unused(0),NextIfd(0),NextRecord(0) {}
    };

struct StructIfd :  StructIfdHeader
    {StructIfdHeader Header;
      TSArray<StructField> field;
      //svector<StructField> field;
      StructIfd() : Header(),field(StructField()) {}
      //StructIfd() : Header(),field(0,0,StructField()) {}
    };

struct StructTagList
    {short number;
    TSArray<short> tag;
    TSArray<LongInt>  len;
    StructTagList() : number(0),tag(0),len(0) {}
    };

int IsFileTgf(const char *name);
inline int NumPadding(int n){return(3 - (n+3)%4);}


/*! \class Tgf
Ce type de fichier permet d'ecrire un certain nombre (RecordNum) d'objets (record),
chacun etant defini par un nombre variable (FieldNum) de tags (TOUS DIFFERENTS).
Chaque record a un numero compris entre 1 et RecordNum

Utilisation Ecriture:
- Eventuellement definir le SubHeader
- Open()
- CreateRecord()
- FieldWrite()
- FieldWrite()  ou SeekWrite()
- FieldWrite()
- ............
- CreateRecord()

Remarque: Pendant la definition d'un nouveau record, on n'a pas le droit
d'utiliser d'autres fonctions.
Utilisation en Lecture:
- Open           retourne le nombre de records
- SetRecord()
- FieldRead()    ou SeekRead()
- SetRecord()
- FieldRead()    ou SeekRead()
- ...........

Remarque: On peut lire un tag dans un ordre different de l'ecriture


Un fichier TGF contient: header,subheader,des IFD,des records
- Header          -> ID,Version,#champs,#record,Offset du 1 IFD:
- SubHeader       -> Identificateur du type de Fichier
- IFDheader       -> Descripteur d'un record
- Field           -> donne d'un record
*/
class Tgf {
    private:
        enum ifd_tags {TAG_FIRST = 1,TAG_NEXT};
        StructHeader Header;
        StructIfd Ifd;                     //header et pointeur sur des champs
        int       IsOpen;
        int       IsGood;
        int       new_ifd;
        int       new_data;                //utiliser seulement por SeekWrite
        int       seek;
        LongInt CurrentIfdOffset;
        TSArray<LongInt> IfdOffset;   //IfdOffset[i] = offset du record i
        LongInt offset_new_data;

        int ReadHeader();
        int WriteHeader();
        int IfdRead(LongInt offset);
        int IfdReadAll(LongInt offset);
        int IfdWrite(LongInt offset);
        int ReadOffsets();                 //Lecture des Offset de tous les records
        int Flush();
    public:
        enum open_mode {old=0,create};
        StructTagList TagList;            //tous les tags du record sur lequel on est
        char SubHeader[17];
        T_STD  fstream stream;

        Tgf() : IfdOffset(0L)
            {IsGood = IsOpen = 0;
            seek = new_ifd = new_data = 0;
            CurrentIfdOffset = offset_new_data = 0;
            strcpy(SubHeader,"................");
            // initialise header
            Header.Version = 1;
            Header.RecordNum = 0;
            Header.IfdNum = 0;
            Header.LenSubHeader = 16;
            Header.FstIfdOffs = 0;
            strcpy(Header.ID,"TGF");
            }
        ~Tgf()
            {if(!IsOpen)return;
            close();
            }
        int open(const char *name, open_mode mode = old);
        void close()
            {if(IsOpen)
                {Flush();
                stream.flush();stream.close();
                IsOpen = 0;
                }
            }
        int LengthSubHeader()           {return(Header.LenSubHeader);}
        void WriteSubHeader()           {stream.write((char *)SubHeader,Header.LenSubHeader);}
        void ReadSubHeader()            {stream.read((char *)&SubHeader[0],16);}
        int good()                      {return(IsGood);}
        int RecordsNumber()             {return(Header.RecordNum);}
        int CreateRecord();
        int SetRecord(int num);
        int DeleteRecord(int num);
        
        LongInt GetTagLength(int Tag);

        int SeekWrite(short t,LongInt NumberBytes);
        int FieldWrite(short t,const char *pointeur,LongInt NumberBytes);

        int FieldWrite(short t,const char c)    {return FieldWrite(t, &c, sizeof(char));}
        int FieldWrite(short t,const short i)   {return FieldWrite(t, (char *)&i, sizeof(short));}
        int FieldWrite(short t,const int i)     {return FieldWrite(t, (char *)&i, sizeof(int));}
        //int FieldWrite(short t,const LongInt l)    {return FieldWrite(t, (char *)&l, sizeof(LongInt));}
        int FieldWrite(short t,const float f)   {return FieldWrite(t, (char *)&f, sizeof(float));}
        int FieldWrite(short t,const double d)  {return FieldWrite(t, (char *)&d, sizeof(double));}
        int FieldWrite(short t,const char *str) {return FieldWrite(t, str, strlen(str)+1);}

        // FieldRead renvoie la longueur de ce qu'on a lu
        int SeekRead(short t,LongInt NumberBytes);
        int FieldRead(short t,char *pointeur,LongInt NumberBytes);

        int SeekRead(short t)                   {return SeekRead(t,GetTagLength(t));}
        int FieldRead(short t,char *pointeur)   {return FieldRead(t,pointeur,GetTagLength(t));}
        int FieldRead(short t,char &c)          {return FieldRead(t, &c, sizeof(char));}
        int FieldRead(short t,short &i)         {return FieldRead(t, (char *)&i, sizeof(short));}
        int FieldRead(short t,int &i)           {return FieldRead(t, (char *)&i, sizeof(int));}
        //int FieldRead(short t,LongInt &l)          {return FieldRead(t, (char *)&l, sizeof(LongInt));}
        int FieldRead(short t,float &f)         {return FieldRead(t, (char *)&f, sizeof(float));}
        int FieldRead(short t,double &d)        {return FieldRead(t, (char *)&d, sizeof(double));}
    };

#endif
