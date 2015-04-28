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


#include <TAXI/Tbase.h>
#define PRIVATE
#include <TAXI/lralgo.h>
#undef PRIVATE

int _FastTwit::DeleteLeft(tvertex vi)
  {while(vi <= vin[Twin().ltop()])
      {if(Twin().MoreThanOne(TW_LEFT)) // droite a > 1 arete
          {Twin().ltop() = Hist.Link[Twin().ltop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_RIGHT)) // gauche non vide
          {Hist.Link[Twin().ltop()] = Twin().rbot();
          Twin().ltop() = Twin().lbot() = 0;
          break;
          }
      else
          {if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }
int _FastTwit::DeleteRight(tvertex vi)
  {while(vi <= vin[Twin().rtop()])
      {if(Twin().MoreThanOne(TW_RIGHT)) // droite a > 1 arete
          {Twin().rtop() = Hist.Link[Twin().rtop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_LEFT)) // gauche non vide
          { Hist.Link[Twin().rtop()] = Twin().lbot();
          Twin().rtop() = Twin().rbot() = 0;
          break;
          }
      else
          {if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }

void _FastTwit::Deletion(tvertex vi)
  {int c;
  if (_current==_fork) return;

  //printf("Deletion on %d\n",vi);//POM
  while(1)
      {c=DeleteRight(vi);
      if (c==AGAIN) continue;
      else if (c==FIN) return;
      if (DeleteLeft(vi)!=AGAIN) return;
      }
  }

void _FastTwit::Fuse(tedge ej)
  {if (PrevTwin().Empty(TW_LEFT))
      PrevTwin().lbot() = ej;

  Hist.Link[ej] = PrevTwin().ltop();
  PrevTwin().ltop() = Twin().ltop();

  if (Twin().NotEmpty(TW_RIGHT))
      {Hist.Link[Twin().rbot()] = PrevTwin().rtop();
      PrevTwin().rtop() = Twin().rtop();
      }
  _current--;
  }

void _FastTwit::Fusion(tedge ej)
  {//printf("Fusion with %d\n",ej); //POM
  do
      {if (vin[ej] >= vin[PrevTwin().ltop()])
          {if (vin[ej] >= vin[PrevTwin().rtop()])
              break; // Stop !
          }
      else if (vin[ej] >= vin[PrevTwin().rtop()])
          PrevTwin().Permute();
      else
          Flipin(ej);
      Fuse(ej);
      } while(_current>_fork+1);
  }

void _FastTwit::Flipin(tedge ej)
  {tedge cv = Couve();
  int l1 = (vin[PrevTwin().ltop()] <= vin[PrevTwin().rbot()]);
  int l2 = (vin[PrevTwin().rtop()] <= vin[PrevTwin().lbot()]);
  if (l2)
      {if(l1 && !Cover(cv))
          Flipe(ej);
      else
          {PrevTwin().Permute();
          if(Cover(cv))   //NotPlanar
              {Hist.Cove = cv;
              Hist.LeftTop  = PrevTwin().ltop();   
              Hist.LeftBot  = PrevTwin().lbot();
              Hist.RightTop = PrevTwin().rtop();   
              Hist.RightBot = PrevTwin().rbot();
              Hist.NP = ej;
              isnotplanar = 1;
              Hist.Thick = 5;
              longjmp(env,1);
              }
          else
              Flipe(ej);
          }
      }
  else
      {if (l1)
          {if(Cover(cv))
              {Hist.Cove = cv;
              Hist.LeftTop  = PrevTwin().ltop();     
              Hist.LeftBot = PrevTwin().lbot();
              Hist.RightTop = PrevTwin().rtop();    
              Hist.RightBot = PrevTwin().rbot();
              Hist.NP = ej;
              isnotplanar = 1;
              Hist.Thick = 3;
              longjmp(env,1);
              }
          else
              Flipe(ej);
          }
      else
          {Hist.Cove = cv; //cv ????
          Hist.LeftTop  = PrevTwin().ltop();    
          Hist.LeftBot  = PrevTwin().lbot();
          Hist.RightTop = PrevTwin().rtop();    
          Hist.RightBot = PrevTwin().rbot();
          Hist.NP = ej;
          isnotplanar = 1;
          Hist.Thick = 4;
          longjmp(env,1);
          }
      }
  }

void _FastTwit::Flipe(tedge ej) // toujours k=pile   bj=brin correspondant … ej
// Flip left under right
  {do
      {Hist.Link[PrevTwin().rbot()] = PrevTwin().ltop();
      PrevTwin().rbot() = PrevTwin().ltop();
      PrevTwin().ltop()=Hist.Link[PrevTwin().ltop()];
      Hist.Link[PrevTwin().rbot()] = 0;
      }while(vin[ej] < vin[PrevTwin().ltop()]);

  if(PrevTwin().ltop() == tedge(0)) PrevTwin().lbot() = tedge(0);
  }
void _FastTwit::Thin(tedge ej)
  { _current = _fork;
  NewTwin(ej);
  }

void _FastTwit::Thick()
  {int _SauvCurrent = _current;

  for(;_current > _fork ;--_current)
      {if(vin[Twin().ltop()] < vin[Twin().rtop()])
          Twin().Permute();
      }
  _current = _SauvCurrent;
  Flick();
  FirstTwin().rbot() = 0;
  FirstTwin().ltop() = Twin().ltop();
  GotoFirstTwin();
  }
void _FastTwit::Mflip()
  {tedge lbt,rtp;
  lbt = Twin().lbot();
  rtp = Twin().rtop();
  if(vin[rtp] == vin[lbt])                                /* effacement en cas d'egalite */
      {
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[rtp] = lbt;
      }
  else                                                    /* flipping de rtp sous lbt  */
      {
      Twin().lbot() = rtp;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[lbt] = rtp;
      Hist.Link[rtp] = 0;
      }
  }

void _FastTwit::Flick()
  {int SauvCurrent=_current;
  tedge cv = Couve();

  for(;_current > _fork;--_current)
      {while(Twin().rtop()!=0)
          {if(side_flip(Twin().rtop(),Twin().lbot(),cv))
              // flip rtop under lbot
              Mflip();
          else
              {if(side_flip(Twin().ltop(),Twin().rbot(),cv))
                  {Twin().Permute();Mflip();}
              else //NotPlanar
                  {Hist.Cove = cv;
                  Hist.LeftTop  = Twin().ltop();    Hist.LeftBot  = Twin().lbot();
                  Hist.RightTop =  Twin().rtop();    Hist.RightBot = Twin().rbot();
                  if(Twin1().lbot() != 0 && Twin1().rbot() != 0)
                      Hist.NP = (Twin1().lbot() <  Twin1().rbot()) ?
                          Twin1().lbot() :Twin1().rbot();
                  else if(Twin1().lbot() != 0)
                      Hist.NP = Twin1().lbot(); 
                  else
                      Hist.NP = Twin1().rbot();
                  isnotplanar = 1;
                  Hist.Thick = 6;
                  longjmp(env,1);
                  }
              }
          }
      if(_current < SauvCurrent)
          Hist.Link[NextTwin().lbot()] = Twin().ltop();
      }
  _current = SauvCurrent;
  }
/**********************************************************************************************************/
// Convention: lbot corresponds to the non null edge with minimum incidence.
 
#ifdef TDEBUG
void _NewFastTwit::Print()
{
  int c=_current;
  int fn=_topfork;
  int f=_fork;
      while (c>0)
	{
	  tedge l=tw[c].ltop();
	  tedge r=tw[c].rtop();
	  do {
	    cout << c << ": ";
	    if (l!=0) cout << vin[l]() << "(" << l << ")"; else cout << "      ";
	    if (r!=0) cout << "  " << vin[r]()<< "("<<r<<")";
	    cout << endl;
	    if (l!=0 && l!=tw[c].lbot()) l=Link[l]; else l=0;
	    if (r!=0 && r!=tw[c].rbot()) r=Link[r]; else r=0;
	  } while (l!=0 || r!=0);
	  --c;
	  if (c==f)
	    {cout   << "   =============" << endl; f=ForkStack[--fn];}
	  else cout << "   -------------"<<endl;
	}
    }
#endif

void _NewFastTwit::Deletion(tvertex vi)
  {tedge ej;
  if (vin[Twin().ltop()]<vi && vin[Twin().rtop()]<vi) return;
  while (_current!=_fork && vi<=vin[Twin().lbot()]) _current--; // pop all the stacks >= vi
  if(! Twin().Empty(TW_RIGHT)) // right side
      { for(ej=Twin().rtop(); vi<=vin[ej]; ej=Link[ej]) // find the first < vi
          ;
      Twin().rtop()=ej;
      if (ej==0) {Twin().rbot()=0;}
      }
    // left side (never empty)
  for (ej=Twin().ltop(); vi<=vin[ej]; ej=Link[ej]) // find the first < vi
      ;
  Twin().ltop()=ej;
  }
// Fuse with the previous stack. The right side of the current stack should be empty.
// The left side of the current stack is put on the top of the side 'side' of the previous stack.
void _NewFastTwit::FusePrev(int side)
  {if (Twin().NotEmpty(TW_RIGHT)) SetNonPlanar();
  Link[Twin().lbot()]=PrevTwin().top(side); PrevTwin().top(side)=Twin().ltop();
  --_current;
  }
// Fuse the stacks till the fusion will contain an element <= vi or the fork is reached.
void _NewFastTwit::FuseTo(tvertex vi)
  {if (_current==_fork+1) // only one stack
      { if (vin[Twin().ltop()]>vi && vin[Twin().rtop()]>vi) SetNonPlanar();	  
      return; // nothing to do !
      }
  while (_current>_fork+2 && vin[PrevTwin().lbot()]>vi) FusePrev(TW_LEFT);
  if (vin[PrevTwin().ltop()]>vi) // one more fuse to left ?
      { if (vin[PrevTwin().rtop()]>vi) SetNonPlanar();
      FusePrev(TW_LEFT);
      }
  else if (vin[PrevTwin().rtop()]>vi)	FusePrev(TW_RIGHT); // one more fuse to right ?
  else if (vin[Twin().ltop()]>vi && vin[Twin().rtop()]>vi) SetNonPlanar();
  }
void _NewFastTwit::Thin()
  { tedge ej= Twin().lbot();
  PopFork();
  Fusion(ej);
  }
void _NewFastTwit::Fusion(tedge ej) // for thin and cotree edges
  {if (vin[ej]>=vin[Twin().ltop()] && vin[ej]>=vin[Twin().rtop()])
      NewTwin(ej); // in this case, create a new twin.
  else 
      {FuseTo(vin[ej]);
      // add the edge ej 'by hand'
      if (vin[ej]>=vin[Twin().ltop()]) 
          { Link[ej]=Twin().ltop(); Twin().ltop()=ej;} 
      else // vin[ej]>=vin[Twin().rtop()]
          { Link[ej]=Twin().rtop(); Twin().rtop()=ej;
          if (Twin().rbot()==0)
              { Twin().rbot()=ej;
              if (vin[Twin().lbot()]>vin[ej]) Twin().Permute();
              }
          }
      }
  }
void _NewFastTwit::Thick()
  {tvertex vlow = vin[Twin1().lbot()];
  tvertex vlow2 = vin[FirstTwin().lbot()];
  FuseTo(vlow);
  if (Twin().rbot()!=0)
      { isnotplanar = 1;
      longjmp(env,1);
      }
  int _SauvCurrent=_current; // keep this stack in touch.
  PopFork();
  FuseTo(vlow2);
  if (vlow2>=vin[Twin().ltop()])
    if (vlow2 >= vin[Twin().rtop()]) // no fuse bewteen the stacks
      { _current++;
      Twin()=tw[_SauvCurrent]; // copy the backuped stack.
      return;
      }
    else
        {Link[tw[_SauvCurrent].lbot()]=Twin().ltop(); Twin().ltop()=tw[_SauvCurrent].ltop();} // link to left
  else // vlow2>=vin[Twin().rtop()]
      {Link[tw[_SauvCurrent].lbot()]=Twin().rtop(); Twin().rtop()=tw[_SauvCurrent].ltop(); // link to right
      if (Twin().rbot()==0)
          {Twin().rbot()=tw[_SauvCurrent].lbot();
          if (vin[Twin().lbot()]>vin[Twin().rbot()]) Twin().Permute();
          }
      }
  }    


