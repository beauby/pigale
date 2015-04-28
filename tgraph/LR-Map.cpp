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


#include <TAXI/graph.h>
#include <TAXI/graphs.h>
#include <TAXI/bicon.h>
#include <TAXI/lralgo.h>
#include <TAXI/embed.h>
#include <TAXI/Tmessage.h>

int Embed::LowerSideUpperSide()
  {tedge ej,je;
  tedge i,j;
  if(m >= n)cir.fill(n,m,0);
  cir[0] = acir[0] = 0;

  // Side.blk_num[0] = 0;
  // special programm for trees

  if(m == n - 1)
      {if(debug())DebugPrintf("LowerSideUpperSide: graph is a tree");
//       Side.dwn.fill(1,m,DROITE);
//       Side.up.fill(1,m,AUTRE|DROITE);
      for(i = 1;i <= m; i++)
          {Side.dwn[i] =  DROITE;
          Side.up[i] =  AUTRE|DROITE;
          }
      return 0;
      }

  svector<tedge> tab(m - n + 2); tab.clear();

  // determine the Side of the upper and lower end of each edge
  // 0 and tree edges have been initialized to GAUCHE

  //Side.up.fill(n,m,AUTRE|DROITE);
  for(i = n;i <= m;++i)Side.up[i] = AUTRE|DROITE;
  Side.dwn[0] = Side.up[0] = GAUCHE;
  for(i = n;i <= m;++i)
      {if(!(Side.up[i] & AUTRE))continue;
      j = 0;
      ej=i;
      do
          {tab[j++] = ej;
          ej = Hist.Link[ej];
          }while(Side.up[ej] & AUTRE);
      tab[j--] = ej;
      // if(ej == 0) ++Side.blk_num[0]; /* Un nouveau bloc */
      do
          {Side.up[tab[j]] =  Side.up[tab[j + 1]] ^ Hist.Dus[tab[j]] ;

          /* restoration of flipping index of each erased edge */
          if(Hist.Flip[tab[j]] == -1)
              Hist.Flip[tab[j]] = Hist.Flip[tab[j+1]] ^ Hist.Dus[tab[j]];
          }while(j--!=0);
      }

  // compute low Side (ls) for cotree edges
  for(je = n;je <= m;++je)
      Side.dwn[je] =  Side.up[je] ^ Hist.Flip[je];

  // computation of ls and us of non-reference thin and thick edges
  for(je = 1;je < n;++je)
      {if(LrSort.tref[vin[je]] == je)
          continue;
      // Side of thick edges
      if(status[je] == PROP_TSTATUS_THICK)
          {Side.dwn[je] = Side.dwn[Hist.Link[je]];
          Side.up[je] = Side.dwn[je];
          }
      // restoration of flipping index of thin edges
      else if(Hist.Link[je] > 0)
          {Hist.Flip[je] = Hist.Flip[Hist.Link[je]] ^  Hist.Dus[je];
          Side.dwn[je] = Side.dwn[Hist.Link[je]];
          Side.up[je] = Side.dwn[je] ^ Hist.Flip[je];
          }
      else
          {Side.dwn[je] = DROITE;
          Side.up[je] = AUTRE | DROITE;
          }
      }
  return 0;
  }
void Embed::PrintLowerSideUpperSide()
  {if(m == n - 1)return;
  printf("planar original representation\n ej       Left       Right\n");
  tbrin ej;
  for(ej = 1;ej < n;++ej)
      printf("T:%4d %4d\n",vin[ej](),vin[-ej]());
  for(ej = n;ej <= m;++ej)
    printf("CT:%4d %4d\n",vin[ej](),vin[-ej]());

  for(ej = n;ej <= m;++ej)
      {switch(Side.dwn[ej]*2 + Side.up[ej])
          {case 0 :
              printf("%4d%7d\n%11d\n",ej(),vin[-ej](),vin[ej]());break;
          case 1 :/*up =1 dwn=0*/
              printf("%4d%17d\n%11d\n",ej(),vin[-ej](),vin[ej]());break;
          case 2 :/*flip up= 0 dwn=1*/
              printf("%4d%7d\n%21d\n",ej(),vin[-ej](),vin[ej]());break;
          case 3 :
              printf("%4d%17d\n%21d\n",ej(),vin[-ej](),vin[ej]());break;
          default:
              printf("ej=%d err=%d %d\n",ej(),Side.dwn[ej],Side.up[ej]);
              break;
          }
      printf("--------------------------\n");
      }
  }

#define CAS(_dwn,_up) (((_dwn)<<2)|(_up))

void Embed::FillUpperHalfEdges()
  {tvertex vi;
  tbrin topri,toprif,topli,toplif,botli,botlif,botri,botrif;
  tbrin ee;
  tedge ej;

  /* generate the clockwise edge ends order at each vertex   */
  /* the upper end of the edge ej is called -ej           */
  /* the lower end of the edge ej is called ej           */
  /* in clockwise order one fins : i-vi,topli,botli,toplif,  */
  /*                 botlif,reff,toprif,botrif,*/
  /*                 topri,botri,i-vi          */

  // special case for the root
  tedge top;
  ej = top = LrSort.tref[1];
  while((ej = LrSort.linkt[ej]) != 0)
      {cir[top] = ej.firsttbrin();
      top = ej;
      }
  cir[top] = LrSort.tref[1].firsttbrin();

  // other vertices
  for(vi = 2;vi <= n;++vi)
      {ej = LrSort.tel[vi];
      if(ej == 0)/* seulement une arete de reference en ce sommet */
          {if(LrSort.tref[vi]== 0)
              {cir[treein(vi).secondtbrin()] = treein(vi).secondtbrin();
              continue;
              }
          else if(LrSort.tref[vi]< n)
              {cir[treein(vi).secondtbrin()] = LrSort.tref[vi].firsttbrin();
              cir[LrSort.tref[vi].firsttbrin()] = treein(vi).secondtbrin();
              continue;
              }
          else
              {cir[treein(vi).secondtbrin()] = LrSort.tref[vi].secondtbrin();
              cir[LrSort.tref[vi].secondtbrin()] = treein(vi).secondtbrin();
              continue;
              }

          }
      else
          {toprif = toplif = botlif = topli = 0;
          topri = botli = botri = botrif =0;
          do
              {if(ej >= n) ee = ej.secondtbrin();
              else ee = ej.firsttbrin();

              switch(CAS(Side.dwn[ej],Side.up[ej]))
                  {case CAS(GAUCHE,GAUCHE) :/* feeding topli */
                      ins_top(ee,botli,topli);
                  break;

                  case CAS(GAUCHE,DROITE) :/* feeding toprif with flipped edges */
                      if(LrSort.tref[vi] == 0 || (LrSort.tref[vi] < n  &&  status[LrSort.tref[vi]] < PROP_TSTATUS_THIN))
                          {ins_top(ee,botli,topli);break;}
                      ins_top(ee,botrif,toprif);
                      break;

                  case CAS(DROITE,GAUCHE) :/* feeding botlif with flipped edges */
                      if(LrSort.tref[vi] == 0 || (LrSort.tref[vi] < n  &&  status[LrSort.tref[vi]] < PROP_TSTATUS_THIN))
                          {ins_bot(ee,botri,topri);break;}
                      ins_bot(ee,botlif,toplif);
                      break;

                  case CAS(DROITE,DROITE) :/* feeding botri */
                      ins_bot(ee,botri,topri);
                      break;

                  case CAS(DROITE,AUTRE|DROITE) :/* feeding botrif with articulated edges */
                      ins_bot(ee,botrif,toprif);
                      break;

                  case CAS(AUTRE|DROITE,AUTRE|DROITE) :/* feeding botli with articulated edges */
                      if(LrSort.tref[vi] < n  && status[LrSort.tref[vi]] < PROP_TSTATUS_THIN)
                          {ins_bot(ee,botli,topli);break;}
                      ins_bot(ee,botrif,toprif);
                      break;

                  default:DPRINTF(("default ej:%d %d %d",ej(),Side.dwn[ej],Side.up[ej]));
                      break;
                  }
              }while((ej = LrSort.linkt[ej]) != 0);
          }
      fuse(vi,topri,toprif,topli,toplif,botli,botlif,botri,botrif);
      }
  }
void Embed::fuse(tvertex vi,tbrin topri,tbrin toprif,tbrin topli,tbrin toplif,
                 tbrin botli,tbrin botlif,tbrin botri,tbrin botrif) // fusing all the piles
  {/* fusing botrif and topri */
  if(toprif!=0)
      {cir[botrif] = topri;
      topri = toprif;
      if(botri == 0)botri = botrif;
      }

  /* fusing 1-vi and topli */
  cir[treein(vi).secondtbrin()] = topli;
  if(topli == 0)botli = treein(vi).secondtbrin();

  /* fusing botli and toplif */
  cir[botli] = toplif;
  if(toplif != 0){botli = botlif;}

  /* fusing botli and LrSort.tref(vi) */
  if(LrSort.tref[vi]!=0)
      {if(LrSort.tref[vi] < n)
          {cir[botli] = LrSort.tref[vi].firsttbrin();
          botli = LrSort.tref[vi].firsttbrin();
          }
      else
          {cir[botli] = LrSort.tref[vi].secondtbrin();
          botli = LrSort.tref[vi].secondtbrin();
          }
      }

  /* fusing botli and topri */
  cir[botli] = topri;
  if(topri == 0)botri = botli;

  /* fusing botri and 1-vi */
  cir[botri] = treein(vi).secondtbrin();
  }
int Embed::FillLowerHalfEdges()
  {svector<tbrin> left(0,n);        left.SetName("Embed::left");
  svector<tbrin> ref(0,n);          ref.SetName("Embed::ref");
  for(int je = -m;je < n; je++) acir[cir[je]] = je;
  tbrin ee = 1;
  left[1]=1;
  ref[vin[1]]=1;
  ee = cir[-1];

  while(1)
      {if(ee <= -n)                        // brin haut de coarbre
          {bun_in(-ee,ref[vin[-ee]],left); // insertion de ee
          ee = cir[ee];
          continue;
          }
      if (ee >= n)                        // brin bas de coarbre (droite)
          {ee = cir[ee];
          continue;
          }
      if (ee > 0)                         // brin bas d'arbre
          {if (ee==1) break;
          left[ee] = ee;
          ref[vin[ee]] = ee;
          }
      ee = cir[-ee];                      // tous les brins d'arbre
      }
  extbrin() = left[1]; 
  return 0;
  }
