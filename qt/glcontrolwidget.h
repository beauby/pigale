#ifndef GLCONTROLWIDGET_H
#define GLCONTROLWIDGET_H

#include <QGLWidget> 
#include <QEvent>

/*! This class handles the user inputs to perform rotations, translations and zoom.
  It derives from  QGLWidget, the Qt class allowing OpenGL operations
 */
class GLControlWidget : public QGLWidget
{
    Q_OBJECT

public:
  GLControlWidget(QWidget *parent,QGLWidget *share = 0);
  //! apply geometric transform as defined by the translation, rotation and scale values
  virtual void transform();
public slots:
  //! defines the rotation along the X axis
  void setXRotation( double degrees );
  //! defines the rotation along the Y axis
  void setYRotation( double degrees );
  //! defines the rotation along the Z axis
  void setZRotation( double degrees );
  //! defines the zoom factor
  void setScale( double s );
  //! defines the translation  along the X axis
  void setXTrans( double x );
  //! defines the translation  along the Y axis
  void setYTrans( double y );
  //! defines the translation  along the Z axis
  void setZTrans( double z );
  //! defines the rotation steps
  virtual void setRotationImpulse( double x, double y, double z );
  //! defines the translation steps
  virtual void setTranslationImpulse( double x, double y, double z );

protected:
  //! set the speed of automatic rotation
  void setAnimationDelay( int ms );
  //! record the mouse position
  void mousePressEvent( QMouseEvent *e );
  //! record the mouse position
  void mouseReleaseEvent( QMouseEvent *e );
  //! Rotates or translate the picture
  void mouseMoveEvent( QMouseEvent * );
  //! stops or starts automatic rotation
  void mouseDoubleClickEvent( QMouseEvent * );
  //! used to set the zoom factor
  void wheelEvent( QWheelEvent * );
  //! used to set the zoom factor
  void keyPressEvent( QKeyEvent *k );
  //! restore automatic rotation
  void showEvent( QShowEvent * );
  //! stops automatic rotation
  void hideEvent( QHideEvent * );

    GLfloat xRot, yRot, zRot;
    GLfloat xTrans, yTrans, zTrans;
    GLfloat scale;
    bool animation;

protected slots:
    virtual void animate();

private:
    bool wasAnimated;
    QPoint oldPos;
    QTimer* timer;
    int delay;
};

#endif
