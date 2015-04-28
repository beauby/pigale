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

/*!
\file clientEvent.h
\brief CustomEvents created used by Pigale
*/

#ifndef CLIENTEVENT_H 
#define CLIENTEVENT_H

#include <QEvent> 
#define USER_EVENT  QEvent::User


enum UserEvents {
	CLIENT_EVENT = USER_EVENT,
	SERVER_INIT_EVENT,
	SERVER_READY_EVENT,
	TEXT_EVENT,
	WRITE_EVENT,
	WRITEB_EVENT,
	CLEARTEXT_EVENT,
	WAIT_EVENT,
	DRAWG_EVENT,
	HANDLER_EVENT,
	PROGRESS_EVENT,
	BANNER_EVENT,
    READY_EVENT
};

class clientEvent : public QEvent
    {public:
      clientEvent( int action,QString  param)
          : QEvent((QEvent::Type)CLIENT_EVENT),value(action),str(param)
          {}
      QString getParamString() const { return str; }
      int getAction() const { return value; }
    private:
      int value;
      QString str;
    };

class serverInitEvent : public QEvent
    {public:
      serverInitEvent()
          :  QEvent((QEvent::Type)(SERVER_INIT_EVENT))
          {}
    };

class serverReadyEvent : public QEvent
    {public:
      serverReadyEvent()
          : QEvent((QEvent::Type)SERVER_READY_EVENT)
          {}
    };

class textEvent : public QEvent
    {public:
      textEvent( QString  txt)
          : QEvent((QEvent::Type) TEXT_EVENT),str(txt)  
          {}
      QString getString() const { return str; }
    private:
      QString str;
    };

class clearTextEvent : public QEvent
    {public:
      clearTextEvent()
          : QEvent((QEvent::Type) CLEARTEXT_EVENT)  
          {}
    };

class writeEvent : public QEvent
    {public:
      writeEvent( QString  txt)
          : QEvent((QEvent::Type) WRITE_EVENT),str(txt)  
       {}
      QString getString() const { return str; }
    private:
      QString str;
    };

class writeBufEvent : public QEvent
    {public:
      writeBufEvent(char * _ptr,uint _size)
          : QEvent((QEvent::Type) WRITEB_EVENT),ptr(_ptr),size(_size) 
          {} 
      char * getPtr() const { return ptr; }
      uint   getSize() const { return size; }
    private:
      char *ptr;
      uint size;
    };

class bannerEvent : public QEvent
    {public:
      bannerEvent( QString  txt)
          : QEvent((QEvent::Type) BANNER_EVENT),str(txt)  
          {}
      QString getString() const { return str; }
    private:
      QString str;
    };

class waitEvent : public QEvent
    {public:
      waitEvent( QString  txt)
          : QEvent((QEvent::Type) WAIT_EVENT),str(txt)  
          {}
      QString getString() const { return str; }
    private:
      QString str;
    };

class drawgEvent : public QEvent
    {public:
      drawgEvent()
          : QEvent((QEvent::Type) DRAWG_EVENT)  
          {}
    };

class handlerEvent : public QEvent
    {public:
      handlerEvent( int action,int drawingType,int saveType)
          : QEvent((QEvent::Type) HANDLER_EVENT),_action( action ),_drawingType(drawingType)
          ,_saveType(saveType)  
          {}
      int type(){return HANDLER_EVENT;}
      int getAction() const { return _action; }
      int getDrawingType() const { return _drawingType; }
      int getSaveType() const { return _saveType; }
    private:
      int _action;
      int _drawingType;
      int _saveType;
    };

class progressEvent : public QEvent
/*
action == 0   init step
action == 1   show step
action == -1  close
*/
    {public:
      progressEvent( int action,int step = 0)
          : QEvent((QEvent::Type) PROGRESS_EVENT),_action( action ),_step(step)  
          {}
      int getAction() const { return _action; }
      int getStep() const { return _step; }
    private:
      int _action;
      int _step;
    };

class readyEvent : public QEvent
    {public:
      readyEvent()
          : QEvent((QEvent::Type) READY_EVENT)  
          {}
    };

#endif



