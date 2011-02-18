#include <QtGui>
#include <QtOpenGL>
#include <QTime>
#include <QImage>

#include <math.h>
#include <QImage>
#include "svlFilterImageQWidget.h"

/**********************************/
/*** svlFilterImageQWidget class ***/
/**********************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageQWidget)

        svlFilterImageQWidget::svlFilterImageQWidget(QWidget *parent)
            : QGLWidget(parent),
            svlFilterBase(),
            Image(0)
{

    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
    QObject::connect(this, SIGNAL(QSignalUpdateGL()), this,  SLOT(updateGL()));

}

svlFilterImageQWidget::~svlFilterImageQWidget()
{
    makeCurrent();
    Release();
}

void svlFilterImageQWidget::initializeGL()
{

    glEnable(GL_NORMALIZE);
    glViewport(0,0,width(),height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0.0, width(), 0.0, height(), 0.0, 1000 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    CheckGLError();

}

void svlFilterImageQWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.6f, 0.0f, 0.0f);

    int w = width();
    int h = height();
    if (!Image)
        return;
    int iw = Image->GetWidth();
    int ih = Image->GetHeight();

    glEnable (GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture (GL_TEXTURE_2D, 8);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, iw, ih, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->GetUCharPointer());

    glPushMatrix();
    glRotatef(-90,0,0,1);
    glTranslatef(-h,0,0.0);

    glBegin (GL_QUADS);

    glTexCoord2f (0.0, 0.0);
    glVertex3f (0,0,0.0);

    glTexCoord2f (0.0, 1.0);
    glVertex3f (h,0,0.0);

    glTexCoord2f (1.0, 1.0);
    glVertex3f (h,w, 0.0);

    glTexCoord2f (1.0, 0.0);
    glVertex3f (0,w, 0.0);

    glEnd ();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    CheckGLError();

}

void svlFilterImageQWidget::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0.0, width, 0.0, height, 0.0, 1000 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void svlFilterImageQWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void svlFilterImageQWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {

    }

    else if (event->buttons() & Qt::RightButton) {

    }
    lastPos = event->pos();
}

int svlFilterImageQWidget::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();
    //Image = dynamic_cast<svlSampleImage*>  (new svlSampleImageRGB());
    // Image->SetSize(0,syncInput->Get,ImageHeight);

    Image = dynamic_cast<svlSampleImage*> (svlSampleImage::GetNewFromType(syncInput->GetType()));
    svlSampleImage *img= dynamic_cast<svlSampleImage*> (syncInput);

    //    Image = dynamic_cast<svlSampleImage*>  (new svlSampleImageRGB());
    for (unsigned int i = 0; i < Image->GetVideoChannels(); i++) {
        Image->SetSize(i,img->GetWidth(),img->GetHeight());
    }

    syncOutput = syncInput;

//    if (GetInput()->GetType() == svlTypeImageRGB) {
//        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(syncInput);


      return SVL_OK;
}

int svlFilterImageQWidget::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage* img = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = img->GetVideoChannels();
    unsigned int idx;

    _OnSingleThread(procInfo)
    {

        //    Image = dynamic_cast<svlSampleImage*>  (new svlSampleImageRGB());
        for (unsigned int i = 0; i < videochannels; i++) {
            memcpy(Image->GetUCharPointer(i), img->GetUCharPointer(i), Image->GetDataSize(i));
        }

        emit QSignalUpdateGL();

    }

    return SVL_OK;
}

int svlFilterImageQWidget::Release()
{
    return SVL_OK;
}

void svlFilterImageQWidget::CheckGLError(){
    for (GLenum gl_error = glGetError(); (gl_error); gl_error = glGetError())
    {
        switch (gl_error){
        case (GL_INVALID_ENUM):
            printf("%s: %u caught: Invalid ENUM\n", __FUNCTION__,gl_error);
            break;
        case (GL_INVALID_VALUE):
            printf("%s: %u caught: Invalid VALUE\n", __FUNCTION__,gl_error);
            break;
        case (GL_INVALID_OPERATION):
            printf("%s: %u caught: Invalid OPERATION\n", __FUNCTION__,gl_error);
            break;
        case (GL_STACK_OVERFLOW):
            printf("%s: %u caught:  GL_STACK_OVERFLOW\n", __FUNCTION__,gl_error);
            break;

        case (GL_STACK_UNDERFLOW):
            printf("%s: %u caught:  GL_STACK_UNDERFLOW\n", __FUNCTION__,gl_error);
            break;
        case (GL_OUT_OF_MEMORY):
            printf("%s: %u caught:  GL_OUT_OF_MEMORY\n", __FUNCTION__,gl_error);
            break;
        default:
            printf("%s: %u caught: NO IDEA\n", __FUNCTION__,gl_error);
        }
        //printf("%s: %s caught at %u\n", __FUNCTION__,(const GLbyte*)gluErrorString(gl_error), __LINE__);

    }
}

