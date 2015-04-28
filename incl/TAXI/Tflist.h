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
// class flist: list implemented by arrays.
//

#ifndef __TFLIST_H__
#define __TFLIST_H__

#include  <TAXI/Tstack.h>

template<class T>
class flist {
    private:

    tarray<T> table;
    int head, tail, len, current;
    tarray<int> next;
    tarray<int> prev;
    stack<int> unused;

    public:

    // constructors.
    flist() : head(-1), tail(-1), len(0), current(-1) {}
    // Next constructor must have two parameters.
    // Otherwise, sexp::sexp(const flist<sexp> &) and this function
    // make a vicious loop.
    // n >= 1.

    flist(T item, int n) : head(0), tail(0), len(1), current(0)
		  {table[0] = item; next[0] = -1; prev[0] = -1;
          while ((--n) > 0) append(item);
          }
    flist(const flist<T> &l) :table(l.table),head(l.head),tail(l.tail)
        ,len(l.len)
        ,current(l.current), next(l.next), prev(l.prev), unused(l.unused)
        { }
    ~flist() {}

    flist<T> &operator =(const flist<T> &l)
        {if (this != &l)
            {table = l.table; head = l.head; tail = l.tail; len = l.len;
            next = l.next; prev = l.prev; unused = l.unused;
            }
        return *this;
        }

    int operator ==(const flist<T> & l) const {
        if (len != l.len) return 0;
        int cur1 = current;
        int cur2 = l.current;

        flist * const local1 = (flist * const)this;
        flist * const local2 = (flist * const)&l;

        to_head();
        l.to_head();
        for (; cur_valid() && l.cur_valid(); l++, operator++()) {
            if (table[current] != l.table[l.current]) {
                local1->current = cur1;
                local2->current = cur2;
                return 0;
            }
        }
        local1->current = cur1;
        local2->current = cur2;
        return 1;
    }


    // return current object.
    T & operator~()
        {if(!cur_valid())
            {DPUTS("current position points to nothing!!\n"); exit(1);}
        return table[current];
        }
    const T & operator~() const
        {if(!cur_valid())
            {DPUTS("current position points to nothing!!\n"); exit(1);}
        return table[current];
        }

    // Diabled because this is too confusing.
    // inline operator T () { return cur();}

    inline int length() const {return len;}
    inline int empty() const {return (!len);}
    inline int cur_valid() const {return ((current == -1)? 0 : 1);}

    inline       T & first()       {return table[head];}
    inline const T & first() const {return table[head];}
    inline       T & last()        {return table[tail];}
    inline const T & last()  const {return table[tail];}

    // move current position.
    inline void to_head() const
        {flist * const local = (flist * const)this; local->current = head;}
    inline void to_tail() const
        {flist * const local = (flist * const)this; local->current = tail;}
    int operator++() const
        {if (!cur_valid()) return 0;
        flist * const local = (flist * const)this;
        local->current = next[current];
        return cur_valid();
        }
    inline int operator++(int i) const
        {for (operator++(); i && cur_valid(); (operator++(), i--)){;}
        return cur_valid();
        }
    int operator--() const
        {if(!cur_valid()) return 0;
        flist * const local = (flist * const)this;
        local->current = prev[current];
        return cur_valid();
        }
    inline int operator--(int i) const
        {for (operator--(); i && current; (operator--(), i--)){;}
        return cur_valid();
        }
    inline void cyclic_succ()
        {current = next[current]; 
        if (!cur_valid()) current = head;
        }
    inline void cyclic_pred()
        {current = prev[current];
        if (!cur_valid()) current = tail;
        }

    // modification functions.

    // modify item at the current position.
    // Diabled because this is too confusing.
    // T & operator = (const T &obj) {
    //    if (!cur_valid()) { DPUTS("current position points to nothing!!\n"); exit(1); }
    //    table[current] = obj;
    //    return table[current];
    //}

    T pop()
        {if (empty()) { DPUTS("list is empty!!\n"); exit(1); }
        T wanted = table[head];
        unused.push(head);
        len--;
        if (current == head) current = -1;
        if (len) { head = next[head]; prev[head] = -1; }
        else head = tail = -1;
        return wanted;
        }
    void push(const T &obj)
        {int p;
        if (unused.empty()) p = len; else p = unused.pop();
        table(p) = obj;
        if (len) { next(p) = head; head = prev(head) = p; }
        else { next(p) = -1; head = tail = p; }
        prev(head) = -1;
        len++;
        }
	 void append(const T &obj)
         {int p;
         if (unused.empty()) p = len; else p = unused.pop();
         table(p) = obj;
         if (len) { prev(p) = tail; tail = next(tail) = p; }
         else { prev(p) = -1; tail = head = p; }
         next(tail) = -1;
         len++;
         }
    void insert_before(const T &obj)
        {if(!cur_valid())
            {DPUTS("current position points to nothing!!\n"); exit(1);}
        int p;
        if(unused.empty()) p = len; else p = unused.pop();
        table(p) = obj;
        if(current == head)
            {prev(head) = p; next(p) = head; head = p; prev(p) = -1;}
        else 
            {next(p) = current; prev(p) = prev(current);
            prev(current) = p; next(prev(p)) = p;
            }
        current = p;
        len++;
        }
    void insert_after(const T &obj)
        {if(!cur_valid())
            {DPUTS("current position points to nothing!!\n"); exit(1); }
        int p;
        if (unused.empty()) p = len; else p = unused.pop();
        table(p) = obj;
        if (current == tail)
            {next(tail) = p; prev(p) = tail; tail = p; next(p) = -1; }
        else 
            {prev(p) = current; next(p) = next(current);
            next(current) = p; prev(next(p)) = p;
            }
        current = p;
        len++;
        }
    // remove the current item and point to the next item.
    T remove()
        {if(!cur_valid())
            { DPUTS("no item at the current position!!\n"); exit(1); }
        T wanted = table[current];
        unused.push(current);
        len--;
        if (current == head) { head = next[head]; }
        else { next[prev[current]] = next[current]; }
        if (current == tail) { tail = prev[tail]; }
        else { prev[next[current]] = prev[current]; }
        current = next[current];
        return wanted;
        }
    inline void clear()
        {table.clear(); head = tail = current = -1;
        len = 0; next.clear(); prev.clear(); unused.clear();
        }
    inline const char *  class_name() const
        {return "flist"; }
};

//#undef stack

#endif
