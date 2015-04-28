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
// class stack: FILO stack.
//

#ifndef __TSTACK_H__
#define __TSTACK_H__

#include <TAXI/Tarray.h>

template<class T>
class stack {
    private:

    tarray<T> table;
    int head;

    public:

    stack() : head(0) {}
    ~stack() {}


    stack<T> &operator =(const stack<T> &s)
        {if (this != &s) {table = s.table; head = s.head;}
        return *this;
        }
	 inline int length() const { return head; }
	 inline int empty() const { return (head == 0)? 1 : 0; }

	 inline T pop()
         {if(!head) {DPUTS("stack is empty!!\n"); exit(1); }
         head--; return table[head+1];
         }
	 inline void push(const T &obj) { head++; table(head) = obj; }
	 inline T peep() const { return table[head]; }
	 inline void clear() { head = 0; }

    inline const char *  class_name() const { return "stack"; }
};

#endif
