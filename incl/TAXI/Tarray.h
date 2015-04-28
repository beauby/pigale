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
//
// class array: array of infinite length initialized by "init".
//

#ifndef __TARRAY_H__
#define __TARRAY_H__

#include <TAXI/Tdebug.h>

template<class T>
class tarray {
    T *ary;
    int size;

    void extend(int idx) {
        int new_size = size, i;
        T *new_ary;

        if (new_size <= 0) new_size = 1;
        while (new_size <= idx) new_size *= 2;
        new_ary = new T[new_size];
        for (i = 0; i < size; i++) new_ary[i] = ary[i];
        for (i = size; i < new_size; i++) new_ary[i] = init;
        size = new_size;
        delete [] ary;
        ary = new_ary;
    }

    protected:
    T init;

    public:

    // constructors.
    inline tarray(int _size, const T &_init) : size(_size), init(_init) {
         int i;
         ary = new T[size];
         CHKNEW(ary);
         for (i = 0; i < size; i++) ary[i] = init;
    }
    // If there is no initializer, T() will be used.
    inline tarray(int _size = 1) : size(_size), init(T()) {
         int i;
         ary = new T[size];
         CHKNEW(ary);
         for (i = 0; i < size; i++) ary[i] = init;
    }
    inline tarray(const tarray<T> &ar) : size(ar.size), init(ar.init) {
         ary = new T[size];
         CHKNEW(ary);
         for (int i = 0; i < size; i++) ary[i] = ar.ary[i];
    }

    // deconstructor.
    inline virtual ~tarray() { delete [] ary; }

    // member functions.
    virtual tarray<T> & operator =(const tarray<T> &ar)
    {
        if (this != &ar) {
            init = ar.init;
            int i;
            if (size < ar.size) {
                delete [] ary;
                size = ar.size;
                ary = new T[size];
                for (i = 0; i < size; i++) ary[i] = ar.ary[i];
            }
            else {
                for (i = 0; i < ar.size; i++) ary[i] = ar.ary[i];
                for (i = ar.size; i < size; i++) ary[i] = init;
            }
        }
        return *this;
    }

    // indirection which does not resize.
    virtual T const & operator[] (int idx) const
    { if (size <= idx) { DPUTS("Index is out of range."); exit(1); } return ary[idx]; }
    virtual T & operator[] (int idx)
    { if (size <= idx) { DPUTS("Index is out of range."); exit(1); } return ary[idx]; }

    // indirection which may resize (if the given index is greator than the length of the tarray).
    virtual T const & operator() (int idx) const {
        if (size <= idx) {
            tarray<T> * const tmp = (tarray<T> * const)this;
            tmp->extend(idx);
        }
        return ary[idx];
    }
    virtual T & operator() (int idx) { if (size <= idx) extend(idx); return ary[idx]; }

    // sort elements whose index is in [head, tail].
    //    inline void sort(int head, int tail, int (*compare)(T *, T *))
    //        { qsort((void *)&(ary[head]), tail-head+1, sizeof(T), compare); }

    inline virtual void clear() { for (int i = 0; i < size; i++) ary[i] = init; }
    inline virtual const char * ClassName() const { return "tarray"; }
};

#endif

