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

#ifndef STATICDATA_H
#define STATICDATA_H
// staticData definitions are in settings.cpp
class staticData 
{private:
  static bool showArrow;
  static bool showOrientation;
  static bool showExtTbrin;
  static int  showVertex;
  static int useDistance;
  static bool schnyderRect;
  static bool schnyderColor;
  static bool schnyderLongestFace;
  static bool randomSeed;
  static bool randomEraseMultipleEdges;
  static bool randomUseGeneratedCir;

public:
  static bool IsUndoEnable;
  static int  macroRepeat;
  static int  macroDelay;
  static int  Gen_N1;
  static int  Gen_N2;
  static int  Gen_M;
  static int  MaxNS;
  static int  MaxND;
  static int  nCut;
  static int  sizeImage;
  static QString  fileImage;
  static QString  dirImage;

public:
  static bool & ShowArrow()
      {return showArrow;
      }
  static bool & ShowOrientation()
      {return showOrientation;
      }
  static bool & ShowExtTbrin()
      {return showExtTbrin;
      }
  static int & ShowVertex()
  // -3:nothing -2:index else:prop
      {return showVertex;
      }
  static bool & SchnyderRect()
      {static bool _SchnyderRect = false;
      return _SchnyderRect;
      }
  static bool & SchnyderColor()
      {static bool _SchnyderColor = false;
      return _SchnyderColor;
      }
  static bool & SchnyderLongestFace()
      {static bool _SchnyderLongestFace = true;
      return _SchnyderLongestFace;
      }
  static int & UseDistance()
      {return useDistance;
      }
  static bool & RandomSeed()
      {return randomSeed;
      }
  static bool & RandomEraseMultipleEdges()
      {return randomEraseMultipleEdges;
      }
  static bool & RandomUseGeneratedCir()
      {return randomUseGeneratedCir;
      }
};

#endif  

