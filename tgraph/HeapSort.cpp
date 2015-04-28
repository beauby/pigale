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

void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap);

static void HeapAdjust(int first,int nelements,int *heap,int i);

static int (*compare)(int a,int b);

void HeapSort(int (*f)(int a,int b),int first,int nelements,int *heap)
  {int i,tmp;

  compare = f;
  for(i = 0;i < nelements;i++)
      heap[i] = i;

  for(i = (nelements - 2) >> 1;i > -1;--i)
      HeapAdjust(first,nelements,heap,i);

  for(i = nelements - 1;i ;--i)
      {tmp = heap[i];heap[i] = heap[0];heap[0] = tmp;
      HeapAdjust(first,i,heap,0);
      }
  }

void HeapAdjust(int first,int nelements,int *heap,int i)
  {int fils,item;

  item = heap[i];
  fils = (i << 1) + 1;
  while(fils < nelements)
      {if((fils < nelements - 1) &&
          compare(heap[fils]+first,heap[fils + 1]+first))
          ++fils;
      if(!compare(item+first,heap[fils]+first))
          break;
      else
          {heap[(fils - 1) >> 1] = heap[fils];
          fils = (fils << 1) + 1;
          }
      }
  heap[(fils - 1) >> 1] = item;
  }

