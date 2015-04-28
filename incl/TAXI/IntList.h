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

#if !defined __IntList_H__
#define      __IntList_H__


#include <TAXI/Tsvector.h>
#include <TAXI/Tdebug.h>

/* 
list of distinct non zero integers.

good point: 
   1. You can check if an item is in list in constant time. (InList())
   2. You can delete an item in list in constant time. (del())

bad point: 
   1. Items must be integers.
   2. Item cannot be 0.             
   3. All items in the list must be distinct numbers.
*/
class IntList
    {
    svector<int> next, prev;
    int head, tail, cur;
    tstring name;

    public:

    IntList() : next(0,0,0), prev(0,0,0), head(0), tail(0), cur(0), name("*") {}
    ~IntList() {}

    int InList(int i) { return (prev(i) || head==i); }
    int empty() const { return (head? 0:1); }
    int first() const { return head; }
    int last() const { return tail; }
    
    void push(int i)
        {
        if (InList(i)){DebugPrintf("Item %d  already in list.",i);myabort();}
        next(i)=head;
        if (head) prev(head)=i;
        else tail=i;
        prev(i)=0;
        head=i;
        }
    int pop()
        {if (!head) {DebugPrintf("Cannot pop: list is empty.");myabort();}
        if (cur==head) cur=next[cur];
        int ret=head;
        head=next[head];
        next[ret]=prev[ret]=0;
        if (head) prev[head]=0;
        else tail=0;
        return ret;
        }
    int pop_last()
        {if(!head) {DebugPrintf("Cannot pop last: list is empty."); myabort();}
        if (cur==tail) cur=0;
        int ret=tail;
        tail=prev[tail];
        next[ret]=prev[ret]=0;
        if (tail) next[tail]=0;
        else head=0;
        return ret;
        }
    void del(int i)
        {if (!InList(i)) {DebugPrintf("Item %d not in list.",i);myabort();}
        if (cur==i) cur=next[cur];
        if (prev[i]) next[prev[i]]=next[i];
        else head=next[i];
        if (next[i]) prev[next[i]]=prev[i];
        else tail=prev[i];
        next[i]=prev[i]=0;
        }
    void clear() { next.clear(); prev.clear(); head=tail=cur=0; }
    void SetName(tstring s) { name=s; }

    // iterator.
    void ToHead() { cur=head; }
    int operator ++() { if (cur) cur=next[cur]; return cur; }
    int operator ~() const { return cur; }
    };

#endif
