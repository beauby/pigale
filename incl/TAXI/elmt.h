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

#ifndef ELMT_H
#define ELMT_H

#include <TAXI/Tbase.h>
class tedge;
class tbrin
    {
    int value;

    public:

    tbrin() {}
    tbrin(int i) : value(i) {}
    tbrin(const tbrin &i) : value(i.value) {}
    ~tbrin() {}

    tbrin & operator = (const tbrin  &i) {value = i.value; return *this;}
    tbrin & operator = (int  i) {value = i; return *this;}
    // operator int  () const {return value;}
    tbrin & cross() { value=-value; return *this;}
    tbrin opposite() const { return tbrin(-value);}
    bool out() const {return value > 0;}
    bool in() const {return !out();}
    int operator()()const {return value;}
    int & operator()() {return value;}
    tedge GetEdge() const;
    tbrin & operator++() {++value; return *this;}
    tbrin operator++(int) { return tbrin(value++);}
    tbrin & operator--() {--value; return *this;}
    tbrin operator--(int) { return tbrin(value--);}

    int operator<(const tbrin &e) const {return value < e();}
    int operator==(const tbrin &e) const {return value == e();}
    int operator>(const tbrin &e) const {return value > e();}
    int operator<=(const tbrin &e) const {return value <= e();}
    int operator>=(const tbrin &e) const {return value >= e();}
    int operator!=(const tbrin &e) const {return value != e();}

    int operator<(int e) const {return value < e;}
    int operator==(int e) const {return value == e;}
    int operator>(int e) const {return value > e;}
    int operator<=(int e) const {return value <= e;}
    int operator>=(int e) const {return value >= e;}
    int operator!=(int e) const {return value != e;}

    int operator!() const {return !value;}
    int operator-() const {return -value;}

    tbrin operator +(int i) {return tbrin(value + i);}
    tbrin operator -(int i) {return tbrin(value - i);}
    tbrin& operator +=(int i) {value += i;return *this;}
    tbrin& operator -=(int i) {value -= i;return *this;}

    };
class tvertex
    {
    int value;

    public:

    tvertex() {}
    tvertex(int i) : value(i) {}
    tvertex(const tvertex &i) : value(i.value) {}
    ~tvertex() {}

    tvertex & operator = (const tvertex &i) {value = i.value; return *this;}
    tvertex & operator = (int  i) {value = i; return *this;}
    // operator int  () const {return value;}
    int operator()()const {return value;}
    int & operator()() {return value;}
    tvertex & operator++() {++value; return *this;}
    tvertex operator++(int) { return tvertex(value++);}
    tvertex & operator--() {--value; return *this;}
    tvertex operator--(int) { return tvertex(value--);}

    int operator<(const tvertex &e) const {return value < e();}
    int operator==(const tvertex &e) const {return value == e();}
    int operator>(const tvertex &e) const {return value > e();}
    int operator<=(const tvertex &e) const {return value <= e();}
    int operator>=(const tvertex &e) const {return value >= e();}
    int operator!=(const tvertex &e) const {return value != e();}

    int operator<(int e) const {return value < e;}
    int operator==(int e) const {return value == e;}
    int operator>(int e) const {return value > e;}
    int operator<=(int e) const {return value <= e;}
    int operator>=(int e) const {return value >= e;}
    int operator!=(int e) const {return value != e;}

    int operator!() const {return !value;}
    int operator-() const {return -value;}

    tvertex operator +(int i) {return tvertex(value + i);}
    tvertex operator -(int i) {return tvertex(value - i);}
    tvertex& operator +=(int i) {value += i;return *this;}
    tvertex& operator -=(int i) {value -= i;return *this;}

    };

class tedge
    {
    int value;

    public:

    tedge() {}
    tedge(int i) : value(i) {}
    tedge(const tedge &i) : value(i.value) {}
    // tedge(const tbrin &b) : value((b() > 0)? b():-b()) {}
    ~tedge() {}

    tedge & operator = (const tedge &i) {value = i.value; return *this;}
    tedge & operator = (int  i) {value = i; return *this;}
    // operator int  () const {return value;}

    tbrin firsttbrin() const { return tbrin(value);}
    tbrin secondtbrin() const { return tbrin(-value);}
    int operator()()const {return value;}
    int & operator()() {return value;}
    tedge & operator++() {++value; return *this;}
    tedge operator++(int) { return tedge(value++);}
    tedge & operator--() {--value; return *this;}
    tedge operator--(int) { return tedge(value--);}

    int operator<(const tedge &e) const {return value < e();}
    int operator==(const tedge &e) const {return value == e();}
    int operator>(const tedge &e) const {return value > e();}
    int operator<=(const tedge &e) const {return value <= e();}
    int operator>=(const tedge &e) const {return value >= e();}
    int operator!=(const tedge &e) const {return value != e();}

    int operator<(int e) const {return value < e;}
    int operator==(int e) const {return value == e;}
    int operator>(int e) const {return value > e;}
    int operator<=(int e) const {return value <= e;}
    int operator>=(int e) const {return value >= e;}
    int operator!=(int e) const {return value != e;}

    int operator!() const {return !value;}
    int operator-() const {return -value;}

    tedge operator +(int i) {return tedge(value + i);}
    tedge operator -(int i) {return tedge(value - i);}
    tedge& operator +=(int i) {value += i;return *this;}
    tedge& operator -=(int i) {value -= i;return *this;}
    };
inline tedge tbrin::GetEdge() const {return (tedge)((value <0) ? -value : value);}

inline T_STD  ostream & operator<<(T_STD  ostream & out, const tbrin &b) {return out<<b();}
inline T_STD  ostream & operator<<(T_STD  ostream & out, const tedge &e) {return out<<e();}
inline T_STD  ostream & operator<<(T_STD  ostream & out, const tvertex &v) {return out<<v();}
inline T_STD  istream & operator>>(T_STD  istream & in, tbrin &b) {return in>>b();}
inline T_STD  istream & operator>>(T_STD  istream & in, tedge &e) {return in>>e();}
inline T_STD  istream & operator>>(T_STD  istream & in, tvertex &v) {return in>>v();}
template <class T>
T_STD  istream& operator>>(T_STD  istream & in, T *&p) {long l; in >>l; p=(T *)l; return in;}

#endif
