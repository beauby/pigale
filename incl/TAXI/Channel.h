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
#ifndef _CHANNEL_H_
#define _CHANNEL_H_
class ChannelRouter
{
    int nseg;
    int minv,maxv;
    int nchannel;
    int nfree;
    svector<int> top;
    svector<int> link;
    svector<int> end;
    svector<int> topendc;  // stack of channels (1 stack by end coordinate)
    svector<int> linkendc;
    svector<bool> free;
public:
    ChannelRouter(int n, int min_v, int max_v):
        nseg(n), minv(min_v), maxv(max_v), top(minv,maxv),
        link(0,nseg), end(0,nseg),topendc(minv,maxv),
        linkendc(0,nseg), free(0,nseg)
        {
        top.SetName("Channel Router top");
        link.SetName("Channel Router link");
        end.SetName("Channel Router end");
        topendc.SetName("Channel Router topendc");
        linkendc.SetName("Channel Router linkendc");
        free.SetName("Channel Router free");
        top.clear();
        link.clear();
        }
    void clear()
        {
        top.clear();
        link.clear();
        }
    void insert(int s,int from, int to)
        {link[s]=top[from];
        top[from]=s;
        end[s]=to;
        }
    int solve(svector<int> &channel)
        { nchannel=nfree=0;
        free.clear();
        topendc.clear();
        linkendc.clear();
        int s,c;
        for (int i=minv; i<=maxv; i++)
            {// free channels
                c=topendc[i];
                while (c!=0)
                    {free[c-1]=true;
                    nfree++;
                    c=linkendc[c];
                    }
                topendc[i]=0;

                s=top[i];
                while(s!=0)
                    {   // find a free channel
                        if(nfree==0)
                            c=nchannel++;
                        else
                            {for (c=0; c<nchannel; c++)
                                if (free[c])
                                    {// free[c]=false;
                                        nfree--;
                                        break;
                                    }
                            }
                        free[c]=false;
                        // affects it
                        channel[s]=c;
                        int to=end[s];
                        linkendc[c+1]=topendc[to];
                        topendc[to]=c+1;
                        s=link[s];
                    }
                // re-free channels for "dot" segs
                c=topendc[i];
                while (c!=0)
                    {free[c-1]=true;
                    nfree++;
                    c=linkendc[c];
                    }
                topendc[i]=0;
            }
        return nchannel;
        }
};
#endif
