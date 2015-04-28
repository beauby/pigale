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


#ifndef TPROP_H
#define TPROP_H

/*! \file Tprop.h 
 * \brief Property Handling 
 */

#include <TAXI/Tbase.h>
#include <sstream>

/**********
 * Prop<T> : Propriete
 **********
 *
 *      Declaration -> exist, declared
 *
 *          Prop<T> toto(X, num);
 *
 *              - si la propriete n'existe pas, creation
 *              - si la propriete existe mais n'est pas declaree, declaration
 *              - sinon, recuperation (peut ne pas etre definie)
 *
 *      Les proprietes sont derivees de rsvector<T> (ref. sur un vector<T>)
 *
 ****************************************************************************
 *
 * Proprietes unique avec constructeur
 *
 **********
 * PSet1 : Ensemble de proprietes uniques
 **********
 *  Les proprietes uniques peuvent avoir plusieurs status
 *      int exist(num) : la propriete existe
 *      int defined(num) : la propriete est definie (vecteur non vide)
 *
 *      definition PSet1 X;
 *
 *      Chargement d'une propriete -> exist, !declared
 *
 *      Import de la valeur :   void import (int num, const void *p, int size)
 *          Fonctionnement  :   memcpy des donnes
 *                              a la declaration, creation d'un objet T
 *                              appel de import(T,buffer)
    file.FieldWrite(TAG_ELIST,(char *)&elist[1],m * sizeof(e_struct));
 *                              destruction du buffer
 *
 *      Acces "directs"
 *
 *          X[i] -> void *      Donnees non typees
 *          X(i) -> vProp1 *    Acces au type par methodes virtuelles
 *
 *      Modifications
 *
 *      Effacement d'une prop.: void erase (int num)
 *      Effacement ttes prop. : void clear ()
 *
 *      Utilitaire
 *
 *      Affichage des prop. :   void affiche()
 *
 **********
 * vProp1 : Acces au type
 **********
 *
 *      Un point d'acces est defini pour toute propriete declaree
 *      On peut acceder a ce point d'entree par X(num) -> vProp1 *
 *
 *      Taille du type :        int size_elmt();
 *      Affichage d'un
 *       element du type :      void affiche(void *p);
 *      Destruction d'un
 *       element du type :      void destroy(void *p);
 *
 **********
 * Prop1<T> : Propriete unique
 **********
 *
 *      Declaration -> exist, declared
 *
 *          Prop<T> toto(X, num, value=T());
 *
 *              - si la propriete n'existe pas, creation par copie de value
 *              - si la propriete existe mais n'est pas declaree, declaration
 *              - sinon, recuperation
 *
 *      Acces :
 *          toto() -> reference sur l'objet.
 *
 ****************************************************************************/


#include <TAXI/Tsvector.h>
#include <TAXI/Tpropio.h>
#include <TAXI/Tdebug.h>


#define PSET_SIZE 256 /**< Size of property tables */
class PSet;


//! Pure virtual class performing typed access to vector properties.
/*! To any declared property is associated an access point which belongs
 * to the \c vProp class. Through this access point, one can handle
 * type-related functionality, such as duplication, display, or get
 * the size of the objects in the vector property.
 */

class vProp
    {
    public:
      virtual int size_elmt() const = 0; //!< size of element
      virtual void affiche(T_STD  ostream& out,const _svector *v) const =0; 
      virtual tstring tostr(const _svector *v, int i) const = 0;
      virtual void fromstr(const _svector *v, int i,tstring t) const = 0;
     
      //!< prints the element (debugging purpose)
      /*!< \param out output stream
       *   \param v vector to print
       */
      virtual vProp * dup() const = 0; //!< duplicates the access
      virtual ~vProp() {}
    };

//! Set of vector property for scalar types (without constructor or destructor)
/*!
 *  A set of vector properties (\c PSet) handles like an array of vector properties.
 * All the vector properties in a same set have the same index bound.
 *
 *  A vector property in the set may have several status:
 *    - it may be \b declared (means a typing has been provided for the property)
 *    - it may be \b defined (means that the vector has been filled with data)
 *    - it may \b exist (there is a \c svector corresponding to the property)
 */

/*      bool exist(num) : la propriete existe
 *      int declared(num) : la propriete a ete declaree (typee)
 *      int defined(num) : la propriete est definie (vecteur non vide)
 *
 *      definition PSet X(-3,5) : les indices vont de -3 a 5
 *          Limites : start() et finish()
 *
 *                 PSet X(6) : entre 0 et 5
 *                 PSet X()  : vide
 *                 PSet X(Y) : copie de Y
 *
 *      Chargement d'une propriete -> exist, !declared, defined (normt)
 *
 *      Import d'un buffer :    void import (int num, const void *p)
 *      Copie d'un _svector :   void load   (int num, const _svector &v)
 *      Switch d'un _svector :  void swload (int num, _svector &v)
 *
 *      Acces "directs"
 *
 *          X[i] -> _svector *  Donnees non typees
 *          X(i) -> vProp *     Acces au type par methodes virtuelles
 *
 *      Modifications
 *
 *      Redimensionnement :     void resize(int start, int finish)
 *      Effacement d'une prop.: void erase (int num)
 *      Effacement ttes prop. : void clear ()
 *
 *      Utilitaire
 *
 *      Affichage des prop. :   void affiche()
 */
class PSet
{
  protected :
    
    int _start;   //!< index of first element in vector properties
  int  _finish; //!< index of last element in vector properties
  svector<vProp *> vtab; //!< array of virtual access 
  svector<_svector *> tab; //!< array of raw \c _svector
  svector<int> previndx; //!< back links for double linked list of \em existing properties
  svector<int> nextindx; //!< forward links for double linked list of \em existing properties
  svector<int> keep; //!< array of flags for reset strategy
  //! copy function
  void copy(const PSet &P);
  //! add in the double linked list of \em existing properties
  void link(int num) 
        {if (tab[num]==(_svector *)0) return;
        int prev = previndx[num] = previndx[-1];
        nextindx[prev] = num;
        previndx[-1] = num;
        nextindx[num] = -1;
        }
  //! remove from the double linked list of \em existing properties
    void unlink(int num) 
        {if (tab[num]==(_svector *)0) return;
        int prev = previndx[num];
        int next = nextindx[prev] = nextindx[num];
        previndx[next] = prev;
        }

    public :

      //! Property set with vectors indexed within two bounds
      /*! \param a start index for properties
       *  \param b last index for properties
       */
    PSet(int a, int b) : _start(a), _finish(b),vtab(PSET_SIZE),tab(PSET_SIZE),
        previndx(-1,PSET_SIZE-1), nextindx(-1,PSET_SIZE-1),keep(PSET_SIZE)
    	{vtab.clear(); tab.clear(); nextindx[-1]=previndx[-1]=-1;keep.clear();}
    //! Property set with vectors of a specified size (indexed from 0)
    /*! \param n size of the properties (indexed from 0 to n-1)
     */
    PSet(int n) : _start(0), _finish(n-1), vtab(PSET_SIZE), tab(PSET_SIZE),
        previndx(-1,PSET_SIZE-1), nextindx(-1,PSET_SIZE-1),keep(PSET_SIZE)
    	{vtab.clear(); tab.clear();nextindx[-1]=previndx[-1]=-1;keep.clear();}
    //! default constructor
    /*! The Property set will have to be resized before any use.
     */
    PSet() : _start(0), _finish(-1), vtab(PSET_SIZE), tab(PSET_SIZE),
        previndx(-1,PSET_SIZE-1), nextindx(-1,PSET_SIZE-1),keep(PSET_SIZE)
    	{vtab.clear(); tab.clear();nextindx[-1]=previndx[-1]=-1;keep.clear();}
    //! copy constructor
    PSet(const PSet &P) : _start(P._start), _finish(P._finish)
        ,vtab(P.vtab.getsize()), tab(P.tab.getsize()),
        previndx(-1,PSET_SIZE-1), nextindx(-1,PSET_SIZE-1),keep(PSET_SIZE)
    	{vtab.clear(); tab.clear();keep.clear();
        nextindx[-1]=previndx[-1]=-1;copy(P);}
    //! destructor
    ~PSet() {clear();}

    //! register a property
    /*! \param num property number (see propdef.h)
     *  \param pAccess virtual access point
     *  \param pv property vector
     */
    void reg(int num,vProp *pAccess,_svector *pv)
    	{vtab[num]=pAccess; tab[num]=pv; link(num);}
    //! register a property (\b declaration only)
    /*! \param num property number (see propdef.h)
     *  \param pAccess virtual access point
     */
    void reg(int num,vProp *pAccess)
    	{vtab[num]=pAccess;}
    //! deregister a property
    /*! \param num property number (see propdef.h)
     */
    void dereg(int num)
        {if (tab[num]==(_svector *)0) return;
        unlink(num); delete vtab[num]; delete tab[num];
        vtab[num]=(vProp *)0; tab[num]=(_svector *)0; keep[num]=0;
        }
    //! exchange the content of two property sets.
    /*! \param P property set to exchange content with.
     */
    void Tswap(PSet &P)
        {tab.Tswap(P.tab); vtab.Tswap(P.vtab);
        previndx.Tswap(P.previndx); nextindx.Tswap(P.nextindx);
        int tmp=_start; _start=P._start; P._start=tmp;
        tmp = _finish; _finish=P._finish; P._finish=tmp;
        }
    //! load a property as a copy of a \c _svector
    /*! \param num property number (see propdef.h)
     *  \param v raw \c _svector to copy
     */
    void load(int num, const _svector &v)
        {unlink(num); tab[num] = new _svector;*tab[num] =v; link(num);}
    //! load a property by exchange with an \c _svector
    /*! If no vector have been yet created for the requested property number,
     * an emty vector is created before exchange.
     * This function is usefull for temporay property change.
     * \param num property number (see propdef.h)
     *  \param v raw \c _svector to exchange with
     */
    void swload(int num, _svector &v)
        {if (tab[num]!=(_svector *)0)
            tab[num]->Tswap(v);
        else
            {
            tab[num] = new _svector;
            tab[num]->Tswap(v);
            link(num);
            }
        }
    //! starting index for properties
    int start() const {return _start;}
    //! ending index for properties
    int finish() const {return _finish;}
    //! erase a property
    /*! \param num property number (see propdef.h)
     */
    void erase(int num)	{dereg(num);}
    //! clear all the properties
    void clear();
    //! clear the \b keep flag for all the properties.
    void KeepClear() {keep.clear();}
    //! clear all the properties with no \b keep flag set.
    void reset();
    //! checks if a property \b exists
    /*! \param num property number (see propdef.h)
     */
    bool exist(int num) const {return tab[num]!=(_svector *)0;}
    //! checks if a property is \b declared
    /*! \param num property number (see propdef.h)
     */
    int declared(int num) const {return vtab[num]!=(vProp *)0;}
    //! checks if a property is \b defined
    /*! \param num property number (see propdef.h)
     */
    int defined(int num) const {return declared(num)&&(tab[num]->origin());}
    //! resize the property set
    /*! \param a start index for properties
      *  \param b last index for properties
      */
    void resize(int a, int b);
    //! Tswap two elements in all the existing properties
    /*! \param a index of first element
      *  \param b index of second element
      */
    void SwapIndex(int a, int b);
    //! copy an element into another one in all the existing properties
    /*! \param a index of source element
      *  \param b index of target element
      */
    void CopyIndex(int a,int b);
    //! first index for property numbers
    /*! should actualy be 0
     */
    int PStart() const {return tab.starti();}
    //! last index for property numbers
    /*! should be \c PSET_SIZE-1
     */
    int PEnd() const {return tab.stopi();}
    //! get virtual access point (constant version)
    /*! \param num property number (see propdef.h)
     */
    vProp * operator() (int num) const {return vtab[num];}
    //! get virtual access point (non constant version)
    /*! \param num property number (see propdef.h)
     */
    vProp * & operator() (int num) {return vtab[num];}
    //! get the raw \c _svector of a property (constant version)
    /*! \param num property number (see propdef.h)
     */
    _svector * operator[] (int num) const {return tab[num];}
    //! get the raw \c _svector of a property (constant version)
    /*! \param num property number (see propdef.h)
     */
    _svector * & operator[] (int num) {return tab[num];}
    //! get the \b keep status of a property (non constant version)
    /*! \param num property number (see propdef.h)
     *  \retval status \b keep flag
     *    - 0: don't keep
     *    - 1: keep
     *  the returned value may be used as a left value to set the \b keep flag.
     */
    int status(int num) const {return keep[num];}
    //! get the \b keep status of a property (constant version)
    /*! \param num property number (see propdef.h)
     *  \retval status \b keep flag
     *  - 0: don't keep
     *  - 1: keep
     */
    int & status(int num) {return keep[num];}
    //! set the \b keep status of a property
    /*! \param num property number (see propdef.h)
     */
    void Keep(int num) {keep[num]=1;}
    //! copy operator
    PSet &operator = (const PSet &X)
        {if (this==&X) return *this;
        clear();
        resize(X.start(),X.finish());
        copy(X);
        return *this;
        }
    //! get the value of an element as a string

    tstring Value(int PNum, int index, int *rc=0)
    { if (!exist(PNum)) {if (rc!=(int *)0) *rc=-1; return "";}
      if (!defined(PNum)) {if (rc!=(int *)0) *rc=-2; return "";}
      if (rc!=(int *)0) *rc=0;
      return vtab[PNum]->tostr(tab[PNum],index);
    }

    //! printing function (debugging purpose)
  
    void affiche(T_STD  ostream & out=T_STD  cout) const
        {out <<"PSet ("<<_start<<","<<_finish<<") "<<T_STD  endl;
        int i = -1;
        while ((i=nextindx[i])>=0)
            {out <<i<< " : ";
            if (vtab[i] != (vProp *)0) vtab[i]->affiche(out,tab[i]);
            else out << "unregistred"<<T_STD  endl;
            }
	}
      /*!< \param out output stream
       */
};
/*! \relates PSet
 * stream printing of a property set
 */

inline T_STD  ostream & operator<<(T_STD  ostream & out, const PSet &X)
    {X.affiche(out); return out;}

//! Template class for virtual access point to vector properties
template <class T>
class vP : public vProp
    {
    public :
      int size_elmt() const { return sizeof(T);}
      vProp * dup() const {return new vP<T>;}
      tstring tostr(const _svector *v, int i) const
      { ostringstream out;
	out << (const T &)(*(svector<T> *)v)[i];
	return tstring(out.str().c_str());
      }
      void fromstr(const _svector *v, int i, tstring t) const
      { istringstream in;
        in.str((const char *)t);
	in >> (T &)(*(svector<T> *)v)[i];
      }      
    void affiche(T_STD  ostream &out, const _svector *p) const
     	{if (p->origin()==(void *)0)
            {out << " undefined"<<T_STD  endl;
            return;
	    }
       else
           {out<<"{ ";
           for (int i = p->starti(); i<p->stopi(); i++)
               out << (const T &)(*(svector<T> *)p)[i] << " ";
           out <<"}"<<T_STD  endl;
           }
        }
      //!< prints the element (debugging purpose)
    /*!
      \param out output stream
      \param p vector to print
    */
    };
//! Vector property
template<class T>
class Prop : public rsvector<T>
{
    public :

      //! get a property
      /*! Note: 
       * - if the property does not exist, it is created.
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet the property belongs to
       * \param num property number (see propdef.h)
       * \retval get raw \c _svector containing propertie's data
       */
    static _svector & get (PSet &X, int num)
    	{if (X(num)==(vProp *)0)
		    {if (X[num]==(_svector *)0) // do not exist
            	X.reg(num,new vP<T>,new svector<T>(X.start(),X.finish()));
			else
				{X.reg(num,new vP<T>); // unregistered
#ifdef TDEBUG
				if (sizeof(T)!=X[num]->SizeElmt())
		        	{DPRINTF(("Prop #%d size_elmt (v): %d instead of %d",num,X[num]->SizeElmt(),sizeof(T)))//;
        			myabort();}
#endif
				}
		    }
#ifdef TDEBUG
		else if (X(num)->size_elmt() != sizeof(T))
		{DPRINTF(("Prop #%d size_elmt : %d instead of %d",num,X(num)->size_elmt(),sizeof(T)))//;
        	myabort();}
#endif
		return *X[num];
		}
      //! get a property and assign a default value
      /*! Note: 
       * - if the property does not exist, it is created and filled with default value
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet the property belongs to
       * \param num property number (see propdef.h)
       * \param value default value for the property
       * \retval get raw \c _svector containing propertie's data
       */
    static _svector & get (PSet &X, int num,const T& value)
    	{if (X(num)==(vProp *)0)
		    {if (X[num]==(_svector *)0) // do not exist
   	         	X.reg(num,new vP<T>,new svector<T>(X.start(),X.finish(),value));
		    else
		       	{X.reg(num,new vP<T>); // unregistered
   	         	X[num]->definit(&value);
#ifdef TDEBUG
				if (sizeof(T)!=X[num]->SizeElmt())
		        	{DPRINTF(("Prop #%d size_elmt (v): %d instead of %d",num,X[num]->SizeElmt(),sizeof(T)))//;
        			myabort();}
#endif
				}
		}
#ifdef TDEBUG
		else if (X(num)->size_elmt() != sizeof(T))
		{DPRINTF(("Prop #%d size_elmt : %d instead of %d",num,X(num)->size_elmt(),sizeof(T)))//;
        	myabort();}
#endif
        //else  X[num]->definit(&value);
	    return *X[num];
	    }
      //! constructor
      /*! Note: 
       * - if the property does not exist, it is created.
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet the property belongs to
       * \param num property number (see propdef.h)
       */
    Prop(PSet & X, int num) : rsvector<T>(get(X, num)) {}
      //! constructor with default value assignment
      /*! Note: 
       * - if the property does not exist, it is created and filled with default value
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet the property belongs to
       * \param num property number (see propdef.h)
       * \param value default value for the property
       */
    Prop(PSet & X, int num, const T&value) : rsvector<T>(get(X,num,value)) {}
    //! destructor
    ~Prop() {}
    //! access to base \c rsvector (non constant)
    rsvector<T> &me() {return *this;}
    //! access to base \c rsvector (constant)
    const rsvector<T> &me() const {return *this;}
    //! copy operator
    Prop<T> &operator=(const Prop<T> &s) {me()=s.me(); return *this;}
//    operator _svector &() {return v;}
//    operator const _svector &() const {return v;}
//    operator svector<T> &() { return svector<T>::cast(v);}
//    operator const svector<T> &() const { return svector<T>::cast(v);}
};
//! Pure virtual class performing typed access to single properties.
/*! To any declared property is associated an access point which belongs
 * to the \c vProp1 class. Through this access point, one can handle
 * type-related functionality, such as duplication, display, exportation,
 * construction, destruction or get
 * the size of the objects in the vector property.
 */
class vProp1
    {
    public:
    virtual int size_elmt() const = 0;//!< size of element
    virtual void destroy(void *) const = 0;//!< destroy an element
     
    virtual void affiche(T_STD  ostream& out, const void *elmt) const = 0; 
     
    //!< prints the element (debugging purpose)
    /*!< \param out output stream
     *   \param elmt element to print
     */    
    virtual _svector * Export(const void *elmt) const = 0;
    //!< Exports the element (for TGF file)
    /*!< \param elmt element to export
     *   \return \c _svector encoding of the element
     */    
    virtual vProp1* dup() const = 0; //!< duplicates the access
    virtual void * edup(void *elmt) const = 0; //!< duplicates an element
    /*!< \param elmt element to export
     * \retval edup copy of the element
     */
    virtual void copy(void *src, void *dest) const = 0; //!< copies an element
    /*!< \param src element to copy
     * \param dest destination element
     */
    virtual ~vProp1() {}
    };
//! Template class for virtual access point to single properties
template <class T>
class vP1 : public vProp1
    {
     public :
      int size_elmt() const { return sizeof(T);}
      vProp1 * dup() const {return new vP1<T>;}
      void destroy(void *p) const {delete (T*)p;}
      
      void affiche(T_STD  ostream&out, const void *p) const {out << *(T*)p << T_STD  endl;}
    //!< prints the property (debugging purpose)
    /*!< \param out output stream
     *   \param p vector to print
     */       
     _svector * Export(const void *p) const
        { return TypeHandler<T>::Export(p);}
    //!< exports an element
    /*!<
     *   \param p pointer to the element to export
     */  
     void * edup(void *p) const
         { return (void *) new T(*(T *)p);
         }
    /*!<
     *   \param p pointer to the element to duplicate
     */  
    //!< duplicates an element
     void copy(void *src, void *dest) const
         { *(T *)dest=*(T *)src;
         }
    //!< copies an element
    /*!< \param src pointer to the element to copy
     * \param dest pointer to the destination element
     */
    };
//! Set of single properties
/*!
 *  A set of single properties (\c PSet1) handles like an array of properties.
 * 
 *  A property in the set may have several status:
 *    - it may be \b declared (means a typing has been provided for the property)
 *    - it may \b exist (there is a \c svector corresponding to the property)
 */
class PSet1
{
    protected :

    svector<vProp1 *> vtab;//!< array of virtual access 
    svector<void *> tab;//!< array of raw pointers to data
    svector<int> keep; //!< array of flags for reset strategy


    //void copy(const PSet &P);
    //! copy function
    void copy(const PSet1 &P); 

    public :
    //! default constructor
    PSet1() : vtab(PSET_SIZE), tab(PSET_SIZE), keep(PSET_SIZE)
    	{vtab.clear(); tab.clear();keep.clear();}
    //! copy constructor
    PSet1(const PSet1 &P) :  vtab(PSET_SIZE), tab(PSET_SIZE), keep()
    	{vtab.clear(); tab.clear();
        copy(P);}
    //! destructor
    ~PSet1() {clear();}
    //! copy operator
    PSet1 & operator=(const PSet1 &P)
        { if (&P == this) return *this;
	  reset(); // clear all the properties without keep flag
	  copy(P);
        return *this;
        }
    //! exchange the content of two property sets.
    /*! \param P property set to exchange content with.
     */    
    void Tswap(PSet1 &P)
        { tab.Tswap(P.tab); vtab.Tswap(P.vtab); }
    //! register a property
    /*! \param num property number (see propdef.h)
     *  \param pAccess virtual access point
     *  \param pv pointer to the data
     */
    void reg(int num,vProp1 *pAccess,void *pv)
    	{ vtab[num]=pAccess; tab[num]=pv;}
    //! register a property (\b declaration only)
    /*! \param num property number (see propdef.h)
     *  \param pAccess virtual access point
     */
    void reg(int num,vProp1 *pAccess)
    	{ vtab[num]=pAccess; }
    //! deregister a property
    /*! \param num property number (see propdef.h)
     */
    void dereg(int num)
        { vtab[num]=(vProp1 *)0; tab[num]=(void *)0;}
    //! erase a property
    /*! \param num property number (see propdef.h)
     */
    void erase(int num)
    	{if(vtab[num] != (vProp1 *) 0)
            {vtab[num]->destroy(tab[num]);delete vtab[num];}
         else delete (char *)tab[num];
        dereg(num);
        }
    //! clear all the properties
    void clear();
    //! clear the \b keep flag for all the properties.
    void KeepClear() {keep.clear();}
    //! get the \b keep status of a property (constant version)
    /*! \param num property number (see propdef.h)
     *  \retval status \b keep flag
     *  - 0: don't keep
     *  - 1: keep
     */
    int status(int num) const {return keep[num];}
    //! get the \b keep status of a property (non constant version)
    /*! \param num property number (see propdef.h)
     *  \retval status \b keep flag
     *    - 0: don't keep
     *    - 1: keep
     *  the returned value may be used as a left value to set the \b keep flag.
     */
    int & status(int num) {return keep[num];}
    //! set the \b keep status of a property
    /*! \param num property number (see propdef.h)
     */
    void Keep(int num) {keep[num]=1;}
    //! clear all the properties with no \b keep flag set.
    void reset();
    //! import a property from a buffer
    /*! \param num property number (see propdef.h)
     *  \param p pointer to the raw buffer
     *  \param size size of the buffer
     */
    void import(int num, const void *p, int size)
        {tab[num] = ::operator new((unsigned int)size);
    	memcpy(tab[num],p,size);
        }
    //! loads a property by exchange with a raw \c _svector
    /*! \param num property number (see propdef.h)
     *  \param v \c _svector which data will be taken
     */
    void swload(int num, _svector &v)
        { _svector *z = new _svector;
        z->Tswap(v);
        tab[num] = z;
        }
    //! checks if a property \b exists
    /*! \param num property number (see propdef.h)
     */    
    bool exist(int num) const {return tab[num]!=(void *)0;}
    //! checks if a property is \b declared
    /*! \param num property number (see propdef.h)
     */
    int declared(int num) const {return vtab[num]!=(vProp1 *)0;}

    //! get virtual access point (constant version)
    /*! \param num property number (see propdef.h)
     */
    vProp1 * operator() (int num) const {return vtab[num];}
    //! get virtual access point (non constant version)
    /*! \param num property number (see propdef.h)
     */
    vProp1 * & operator() (int num) {return vtab[num];}
    //! get the raw data pointer of a property (constant version)
    /*! \param num property number (see propdef.h)
     */
    void * operator[] (int num) const {return tab[num];}
    //! get the raw data pointer of a property (non constant version)
    /*! \param num property number (see propdef.h)
     */
    void * & operator[] (int num) {return tab[num];}
    //! first index for property numbers
    /*! should actualy be 0
     */
    int PStart() const {return tab.starti();}
    //! last index for property numbers
    /*! should be \c PSET_SIZE-1
     */
    int PEnd() const {return tab.stopi();}
    //! printing function (debugging purpose)
  
    void affiche(T_STD  ostream &out=T_STD  cout) const
        { for (int i=tab.starti(); i<tab.stopi(); i++)
	    if (tab[i]!=(void *)0)
	        { out <<i<< " : ";
	        if (vtab[i] != (vProp1 *)0)
		    vtab[i]->affiche(out,tab[i]);
		else out << "unregistred"<<T_STD  endl;
		}
	}
};
//! Single property
template<class T>
class Prop1
{
    T & ref;
    public :


      //! get a property and assign a default value
      /*! Note: 
       * - if the property does not exist, it is created and filled with default value
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet1 the property belongs to
       * \param num property number (see propdef.h)
       * \param value default value for the property
       * \retval get propertie's data
       */

    static T & get (PSet1 &X, int num, const T& value)
	{
    if (X(num)==(vProp1 *)0)
	    {if (X[num]==(void *)0) // do not exist
            X.reg(num,new vP1<T>,new T(value));
	    else
	        { _svector *p = (_svector *)X[num];
		    X.reg(num,new vP1<T>,new T); // unregistered
		    TypeHandler<T>::Import(*(T *)X[num],p);
            delete p;
		    }
		}
	return *(T *)X[num];
	}
      //! get a property
      /*! Note: 
       * - if the property does not exist, it is created.
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet1 the property belongs to
       * \param num property number (see propdef.h)
       * \retval propertie's data
       */
    static T & get (PSet1 &X, int num)
	{
    if (X(num)==(vProp1 *)0)
	    {if (X[num]==(void *)0) // do not exist
            X.reg(num,new vP1<T>,new T);
	    else
	        { _svector *p = (_svector *)X[num];
		    X.reg(num,new vP1<T>,new T); // unregistered
		    TypeHandler<T>::Import(*(T *)X[num],p);
            delete p;
		    }
		}
	return *(T *)X[num];
	}
      //! constructor
      /*! Note: 
       * - if the property does not exist, it is created.
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet1 the property belongs to
       * \param num property number (see propdef.h)
       */    
    Prop1(PSet1 & X, int num) : ref(get(X, num)) {}
      //! constructor with default value assignment
      /*! Note: 
       * - if the property does not exist, it is created and filled with default value
       * - if the property is not \b declared, it is declared with the template type.
       * \param X \c PSet1 the property belongs to
       * \param num property number (see propdef.h)
       * \param value default value for the property
       */
    Prop1(PSet1 & X, int num, const T& value) : ref(get(X, num,value)) {}
    //! cast to value (non constant version)
    /*! the return value may be used as a lvalue
     */
    operator T & () { return ref;}
    //! cast to value (constant version)
    operator const T & () const { return ref;}
    //! access to value (non constant version)
    /*! the return value may be used as a lvalue
     */
    T & operator()() {return ref;}
    //! access to value (constant version)
    const T & operator()() const {return ref;}
    //! destructor
    ~Prop1() {}
};
/*! \relates PSet1
 * stream printing of a property set
 */

inline T_STD  ostream & operator<<(T_STD  ostream & out, const PSet1 &X) {X.affiche(out); return out;}

#endif
