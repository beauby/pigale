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


#include <TAXI/Tdebug.h> 

#define PRIVATE
#include <TAXI/lralgo.h>
#undef PRIVATE 
#include <TAXI/Tmessage.h> 
int _Twit::DeleteLeft(tvertex vi)
  {while(vi <= vin[Twin().ltop()])
      {if(Twin().MoreThanOne(TW_LEFT)) // droite a > 1 arete
          {Hist.Dus[Twin().ltop()] = Hist.Flip[Twin().ltop()] ^ Hist.Flip[Hist.Link[Twin().ltop()]];
          Twin().ltop() = Hist.Link[Twin().ltop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_RIGHT)) // gauche non vide
          {Hist.Link[Twin().ltop()] = Twin().rbot();
          Hist.Dus[Twin().ltop()] = (int)( 1 ^ Hist.Flip[Twin().ltop()] ^ Hist.Flip[Twin().rbot()]);
          Twin().ltop() = Twin().lbot() = 0;
          break;
          }
      else
          {Hist.Dus[Twin().ltop()] = 0;
          if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }
int _Twit::DeleteRight(tvertex vi)
  {while(vi <= vin[Twin().rtop()])
      {if(Twin().MoreThanOne(TW_RIGHT)) // droite a > 1 arete
          {Hist.Dus[Twin().rtop()] = Hist.Flip[Twin().rtop()] ^ Hist.Flip[Hist.Link[Twin().rtop()]];
          Twin().rtop() = Hist.Link[Twin().rtop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_LEFT)) // gauche non vide
          {Hist.Link[Twin().rtop()] = Twin().lbot();
          Hist.Dus[Twin().rtop()] = (int)(1 ^ Hist.Flip[Twin().rtop()] ^ Hist.Flip[Twin().lbot()]);
          Twin().rtop() = Twin().rbot() = 0;
          break;
          }
      else
          {Hist.Dus[Twin().rtop()] = 1;
          if(--_current)return AGAIN;
          return FIN;
          }
      //if(Hist.Dus[Twin().ltop()] < 0)DebugPrintf("\n Twit.cpp B");
      }
  return STOP;
  }

void _Twit::Deletion(tvertex vi)
// erase all cotree edges above the current fork whose lower vertex is >= vi
  {int c;
  if (_current==_fork) return;

  while(1)
      {c=DeleteRight(vi);
      if (c==AGAIN) continue;
      else if (c==FIN) return;
      if (DeleteLeft(vi)!=AGAIN) return;
      }
  }

void _Twit::Fuse(tedge ej)
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

void _Twit::Fusion(tedge ej)
  {do
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

void _Twit::Flipin(tedge ej)
  {tedge cv = Couve();
  int l1 = (vin[PrevTwin().ltop()] <= vin[PrevTwin().rbot()]);
  int l2 = (vin[PrevTwin().rtop()] <= vin[PrevTwin().lbot()]);

  if(l2)
      {if(l1 && !Cover(cv))
          Flipe(ej);
      else
          {PrevTwin().Permute();
          if(Cover(cv))
              Efnp(ej);
          else
              Flipe(ej);
          }
      }
  else
      {if(l1)
          {if(Cover(cv))
              Efnp(ej);
          else
              Flipe(ej);
          }
      else
          Efnp(ej);
      }
  }

void _Twit::Flipe(tedge ej) // toujours k=pile   bj=brin correspondant … ej
  {
  //cout << "flipe"<<endl;
  do
      {Hist.Link[PrevTwin().rbot()] = PrevTwin().ltop();
      PrevTwin().rbot() = PrevTwin().ltop();
      Hist.Flip[PrevTwin().ltop()]=(int)(1 ^ Hist.Flip[PrevTwin().ltop()]);
      PrevTwin().ltop()=Hist.Link[PrevTwin().ltop()];
      Hist.Link[PrevTwin().rbot()] = 0;
      }while(vin[ej] < vin[PrevTwin().ltop()]);

  if(PrevTwin().ltop() == 0) PrevTwin().lbot() = 0;
  }

void _Twit::Efnp(tedge ej)  // toujours k=pile;
// if Flipin did not succeed
  {isnotplanar=1;
  tedge je;
  do
      {je = PrevTwin().ltop();
      Hist.Dus[je] = Hist.Flip[je] ^ Hist.Flip[ej];
      Hist.Flip[je]= -1;
      PrevTwin().ltop() = Hist.Link[je];
      Hist.Link[je] = ej;
      }while(vin[ej] < vin[PrevTwin().ltop()]);

  if (PrevTwin().ltop() == 0) PrevTwin().lbot() = 0;
  }

void _Twit::Thin(tedge ej)
// align all lower cotree edges on the same side by fliping edges
// in such a case we cannot find an obstruction to planarity and use the fast align method
  {int flipin = Hist.Flip[ej];
  for (; _current > _fork; _current--)
      {if (Twin().lbot() == 0)
          Align(Twin().rtop(),flipin,ej);
      else if (Twin().rbot() == 0)
          Align(Twin().ltop(),flipin,ej);
      else
          {Align(Twin().rtop(),(int)(1 ^ flipin),ej);
          Align(Twin().ltop(),flipin,ej);
          }
      }
  NewTwin(ej);
  }

void _Twit::Align(tedge je,int flipin,tedge ej)
//erase and link all edge je up to ej excluded
  {tedge jje;

  do
      {if(je == ej)break;
      jje = Hist.Link[je];
      Hist.Link[je] = ej;
      Hist.Dus[je] = Hist.Flip[je] ^ flipin;
      Hist.Flip[je] = -1;
      }while((je = jje) != 0);
  }

void _Twit::Thick()
// align all lower cotree edges on the same side by fliping edges calling flick
// an obstruction to planarity may occur
  {int SaveCurrent = _current;

  for(;_current > _fork ;--_current)
      {if(vin[Twin().ltop()] < vin[Twin().rtop()])
          Twin().Permute();
      }
  _current = SaveCurrent;
  Flick(); 
  FirstTwin().rbot() = 0;  FirstTwin().ltop() = Twin().ltop();
  GotoFirstTwin();
  }

void _Twit::Drop()
// if Flick (called by Thick) did not succeed
  {isnotplanar=1;
  tedge rtp = Twin().rtop();
  tedge lbt = Twin().lbot();
  Twin().rtop() = Hist.Link[rtp];
  Hist.Link[rtp] = lbt;
  Hist.Dus[rtp] =  (int)(1 ^ Hist.Flip[rtp] ^ Hist.Flip[lbt]);
  Hist.Flip[rtp] = -1;
  }

void _Twit::Mflip()
  {tedge lbt,rtp;
  lbt = Twin().lbot();
  rtp = Twin().rtop();
  //cout << "mflip"<<endl;
  if(vin[rtp] == vin[lbt])                                /* effacement en cas d'egalite */
      {Hist.Dus[rtp] = (int)(1 ^ Hist.Flip[rtp] ^ Hist.Flip[lbt]);
      Hist.Flip[rtp] = -1;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[rtp] = lbt;
      }
  else                                                    /* flipping de rtp sous lbt  */
      {Hist.Flip[rtp] = (int)(1 ^ Hist.Flip[rtp]);
      Twin().lbot() = rtp;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[lbt] = rtp;
      Hist.Link[rtp] = 0;
      }
  }

void _Twit::Flick()
  {tedge cv;
  int SaveCurrent=_current;
  cv = Couve();
  for(;_current > _fork;--_current)
      {while(Twin().rtop()!=0)
          {if(side_flip(Twin().rtop(),Twin().lbot(),cv))
              Mflip();
          else
              {if(side_flip(Twin().ltop(),Twin().rbot(),cv))
                  {Twin().Permute();Mflip();}
              else
                  Drop();
              }
          }
      if(_current < SaveCurrent)
          Hist.Link[NextTwin().lbot()] = Twin().ltop();
      }
  _current = SaveCurrent;
  }

/***************************************************************************************************************/
int _NewTwit::DeleteLeft(tvertex vi)
  {while(vi <= vin[Twin().ltop()])
      {if(Twin().MoreThanOne(TW_LEFT)) // droite a > 1 arete
          {Hist.Dus[Twin().ltop()] = Hist.Flip[Twin().ltop()] ^ Hist.Flip[Hist.Link[Twin().ltop()]];
          Twin().ltop() = Hist.Link[Twin().ltop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_RIGHT)) // gauche non vide
          {Hist.Link[Twin().ltop()] = Twin().rbot();
          Hist.Dus[Twin().ltop()] = (int)( 1 ^ Hist.Flip[Twin().ltop()] ^ Hist.Flip[Twin().rbot()]);
          Twin().ltop() = Twin().lbot() = 0;
          break;
          }
      else
          {Hist.Dus[Twin().ltop()] = 0;
          if(--_current)return AGAIN;
          return FIN;
          }
      }
  return STOP;
  }
int _NewTwit::DeleteRight(tvertex vi)
  {while(vi <= vin[Twin().rtop()])
      {if(Twin().MoreThanOne(TW_RIGHT)) // droite a > 1 arete
          {Hist.Dus[Twin().rtop()] = Hist.Flip[Twin().rtop()] ^ Hist.Flip[Hist.Link[Twin().rtop()]];
          Twin().rtop() = Hist.Link[Twin().rtop()]; // pop Hist.Link ?
          continue;
          }
      else if (Twin().NotEmpty(TW_LEFT)) // gauche non vide
          {Hist.Link[Twin().rtop()] = Twin().lbot();
          Hist.Dus[Twin().rtop()] = (int)(1 ^ Hist.Flip[Twin().rtop()] ^ Hist.Flip[Twin().lbot()]);
          Twin().rtop() = Twin().rbot() = 0;
          break;
          }
      else
          {Hist.Dus[Twin().rtop()] = 1;
          if(--_current)return AGAIN;
          return FIN;
          }
      //if(Hist.Dus[Twin().ltop()] < 0)DebugPrintf("\n Twit.cpp B");
      }
  return STOP;
  }

void _NewTwit::Deletion(tvertex vi)
// erase all cotree edges above the current fork whose lower vertex is >= vi
  {int c;
  if (_current==_fork) return;

  while(1)
      {c=DeleteRight(vi);
      if (c==AGAIN) continue;
      else if (c==FIN) return;
      if (DeleteLeft(vi)!=AGAIN) return;
      }
  }

void _NewTwit::Fuse(tedge ej)
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

void _NewTwit::Fusion(tedge ej)
  {do
      {if (vin[ej] >= vin[PrevTwin().ltop()])
          {if (vin[ej] >= vin[PrevTwin().rtop()])
              break; // Stop !
          }
      else if (vin[ej] >= vin[PrevTwin().rtop()])
          PrevTwin().Permute();
      else
          Efnp(ej);
      Fuse(ej);
      } while(_current>_fork+1);
  }


void _NewTwit::Efnp(tedge ej)  // toujours k=pile;
// if Flipin did not succeed
  {isnotplanar=1;
  tedge je;
  do
      {je = PrevTwin().ltop();
      Hist.Dus[je] = Hist.Flip[je] ^ Hist.Flip[ej];
      Hist.Flip[je]= -1;
      PrevTwin().ltop() = Hist.Link[je];
      Hist.Link[je] = ej;
      }while(vin[ej] < vin[PrevTwin().ltop()]);

  if (PrevTwin().ltop() == 0) PrevTwin().lbot() = 0;
  }

void _NewTwit::Thin(tedge ej)
// align all lower cotree edges on the same side by fliping edges
// in such a case we cannot find an obstruction to planarity and use the fast align method
  {int flipin = Hist.Flip[ej];
  for (; _current > _fork; _current--)
      {if (Twin().lbot() == 0)
          Align(Twin().rtop(),flipin,ej);
      else if (Twin().rbot() == 0)
          Align(Twin().ltop(),flipin,ej);
      else
          {Align(Twin().rtop(),(int)(1 ^ flipin),ej);
          Align(Twin().ltop(),flipin,ej);
          }
      }
  NewTwin(ej);
  }

void _NewTwit::Align(tedge je,int flipin,tedge ej)
//erase and link all edge je up to ej excluded
  {tedge jje;

  do
      {if(je == ej)break;
      jje = Hist.Link[je];
      Hist.Link[je] = ej;
      Hist.Dus[je] = Hist.Flip[je] ^ flipin;
      Hist.Flip[je] = -1;
      }while((je = jje) != 0);
  }

void _NewTwit::Thick()
// align all lower cotree edges on the same side  calling flick
// an obstruction to planarity may occur
  {int SaveCurrent = _current;

  for(;_current > _fork ;--_current)
      {if(vin[Twin().ltop()] < vin[Twin().rtop()])
          Twin().Permute();
      }
  _current = SaveCurrent;
  Flick(); 
  FirstTwin().rbot() = 0;  FirstTwin().ltop() = Twin().ltop();
  GotoFirstTwin();
  }

void _NewTwit::Drop()
// if Flick (called by Thick) did not succeed
  {isnotplanar=1;
  tedge rtp = Twin().rtop();
  tedge lbt = Twin().lbot();
  //DebugPrintf("NOT PLANAR: rtp:%d lbt%d",rtp(),lbt());
  Twin().rtop() = Hist.Link[rtp];
  Hist.Link[rtp] = lbt;
  Hist.Dus[rtp] =  (int)(1 ^ Hist.Flip[rtp] ^ Hist.Flip[lbt]);
  Hist.Flip[rtp] = -1;
  }

void _NewTwit::Mflip()
  {tedge lbt,rtp;

  lbt = Twin().lbot();
  rtp = Twin().rtop();
  if(vin[rtp] == vin[lbt])                                /* effacement en cas d'egalite */
      {Hist.Dus[rtp] = (int)(1 ^ Hist.Flip[rtp] ^ Hist.Flip[lbt]);
      Hist.Flip[rtp] = -1;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[rtp] = lbt;
      }
  else                                                    /* flipping de rtp sous lbt  */
      {Hist.Flip[rtp] = (int)(1 ^ Hist.Flip[rtp]);
      Twin().lbot() = rtp;
      Twin().rtop() = Hist.Link[rtp];
      Hist.Link[lbt] = rtp;
      Hist.Link[rtp] = 0;
      }
  }

void _NewTwit::Flick()
  {int SaveCurrent=_current;
  tedge cv = Couve();

  for(;_current > _fork;--_current)
      {while(Twin().rtop()!=0)
           {if(side_flip(Twin().rtop(),Twin().lbot(),cv))
               Mflip();
           else
               {if(side_flip(Twin().ltop(),Twin().rbot(),cv))
                   {Twin().Permute();Mflip();}
               else
                   Drop();
               }
           }

      if(_current < SaveCurrent)
          Hist.Link[NextTwin().lbot()] = Twin().ltop();
      }
  _current = SaveCurrent;
  }
