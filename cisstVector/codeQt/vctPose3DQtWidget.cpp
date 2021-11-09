/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-09-04

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstConfig.h>

#include <cisstVector/vctPose3DQtWidget.h>
#include <QTableWidget>
#include <QHeaderView>
#include <QKeyEvent>
#include <QPainter>

#define VCT_POSE_3D_MAX_ROWS 10

vctPose3DQtWidget::vctPose3DQtWidget(QWidget * parent):
    QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(QString("tbd"));
    setContentsMargins(0, 0, 0, 0);

    // set grid layout to display multiple views
    mLayout = new QGridLayout();
    setLayout(mLayout);

    // table of values
    mTable = new QTableWidget();
    mTable->setContentsMargins(0, 0, 0, 0);
    mTable->verticalHeader()->hide();
    mTable->horizontalHeader()->hide();
    mTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Minimum);
    mTable->setSizePolicy(policy);
    mTable->setRowCount(VCT_POSE_3D_MAX_ROWS);
    mTable->setColumnCount(3);
    for (int row = 0; row < VCT_POSE_3D_MAX_ROWS; ++row) {
        for (int column = 0; column < 3; ++column) {
            QTableWidgetItem * tableItem = mTable->item(row, column);
            if (tableItem == 0) {
                tableItem = new QTableWidgetItem();
                tableItem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
                tableItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                tableItem->setText("");
                mTable->setItem(row, column, tableItem);
            }
        }
    }
    mTable->resize(mTable->sizeHint());
    mTableRow = 0;
    mLayout->addWidget(mTable, 0, 0);

    // create 3 views
    vctPose3DQtWidgetView * view;
    view = new vctPose3DQtWidgetView(this, &mPoses, &mBB);
    view->SetDimensions(0, 1); // plot XY
    mLayout->addWidget(view, 0, 1);
    mViews.push_back(view);

    view = new vctPose3DQtWidgetView(this, &mPoses, &mBB);
    view->SetDimensions(0, 2); // plot XZ
    mLayout->addWidget(view, 1, 0);
    mViews.push_back(view);

    view = new vctPose3DQtWidgetView(this, &mPoses, &mBB);
    view->SetDimensions(1, 2); // plot YZ
    mLayout->addWidget(view, 1, 1);
    mViews.push_back(view);
}

void vctPose3DQtWidget::SetPrismaticRevoluteFactors(const double & prismatic,
                                                    const double & revolute)
{
    mPrismaticFactor = prismatic;
    const ViewsType::iterator end = mViews.end();
    for (ViewsType::iterator view = mViews.begin();
         view != end;
         ++view) {
        (*view)->SetPrismaticRevoluteFactors(prismatic, revolute);
    }

}

void vctPose3DQtWidget::SetValue(const vct3 & value)
{
    // update table if there's room
    if (mTableRow < VCT_POSE_3D_MAX_ROWS) {
        QString itemValue;
        for (int column = 0; column < 3; ++column) {
            mTable->item(mTableRow, column)->setText(QString::number(value.Element(column) * mPrismaticFactor, 'f', 2));
        }
        mTableRow++;
    }

    // update bounding box
    mBB.Expand(value);

    // save all values for display
    mPoses.push_back(value);

    const ViewsType::iterator end = mViews.end();
    for (ViewsType::iterator view = mViews.begin();
         view != end;
         ++view) {
        (*view)->update();
    }
}

void vctPose3DQtWidget::Clear(void)
{
    mTableRow = 0;
    for (int row = 0; row < VCT_POSE_3D_MAX_ROWS; ++row) {
        for (int column = 0; column < 3; ++column) {
            mTable->item(row, column)->setText("");
        }
    }
    mPoses.clear();
}


void vctPose3DQtWidget::keyPressEvent(QKeyEvent * event)
{
    std::cerr << "vctPose3DQtWidgetView::keyPressEvent: " << event->key() << std::endl;
    switch(event->key()) {
    case Qt::Key_1:
        break;
    default:
        break;
    }
}


vctPose3DQtWidgetView::vctPose3DQtWidgetView(QWidget * parent, PosesType * poses, vctBoundingBox3 * bb):
    vctQtOpenGLBaseWidget(parent),
    mPoses(poses),
    mBB(bb)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(QString("'a' to turn on/off autoresize\n'r' to reset autoresize if already in autoresize mode\n'1' to plot XY\n'2' to plot XZ\n'3' to plot YZ"));
    setContentsMargins(0, 0, 0, 0);
    mX = 0;
    mY = 1;
    SetAutoResize(true);
}

void vctPose3DQtWidgetView::SetPrismaticRevoluteFactors(const double & prismatic,
                                                        const double & CMN_UNUSED(revolute))
{
    mPrismaticFactor = prismatic;
}

void vctPose3DQtWidgetView::SetAutoResize(const bool autoResize)
{
    // reset BB used to auto-center
    mBB->Reset();
    mAutoResize = autoResize;
}

void vctPose3DQtWidgetView::ResetSize(void)
{
    mBB->Reset();
}

void vctPose3DQtWidgetView::SetDimensions(const size_t x, const size_t y)
{
    if ((x != y)
        && (x < 3)
        && (y < 3)) {
        mX = x;
        mY = y;
    } else {
        mX = 0;
        mY = 1;
    }
}

void vctPose3DQtWidgetView::keyPressEvent(QKeyEvent * event)
{
    switch(event->key()) {
    case Qt::Key_1:
        mX = 0;
        mY = 1;
        mBB->Reset();
        break;
    case Qt::Key_2:
        mX = 0;
        mY = 2;
        mBB->Reset();
        break;
    case Qt::Key_3:
        mX = 1;
        mY = 2;
        mBB->Reset();
        break;
    case Qt::Key_A:
        SetAutoResize(!mAutoResize);
        break;
    case Qt::Key_R:
        ResetSize();
        break;
    default:
        break;
    }
}

void vctPose3DQtWidgetView::initializeGL(void)
{
    glMatrixMode(GL_MODELVIEW); // set the model view matrix
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); // disable depth test
    glDisable(GL_LIGHTING); // disable lighting
    glShadeModel(GL_SMOOTH); // smooth render
    const QColor bgColor = palette().color(QPalette::Base);
    glClearColor(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 1.0);
}

void vctPose3DQtWidgetView::paintGL(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // update translation and scale based on viewport and bounding box
    if (mAutoResize && (mBB->Counter() != 0)) {
        // scale to fit viewport without changing ratio
        vct3 sizeBB = mBB->Diagonal();
        if ((sizeBB[mX] > 0.0) && (sizeBB[mY] > 0.0)) {
            const double ratioX = mViewport.X() / sizeBB[mX];
            const double ratioY = mViewport.Y() / sizeBB[mY];
            mViewportScale = (ratioY > ratioX ? ratioX : ratioY) * 0.98; // add some padding
        } else {
            mViewportScale = 1.0;
        }
        // take center of BB for display
        vct3 centerBB = mBB->MidPoint();
        centerBB.Multiply(mViewportScale); // scale to viewport
        vct2 centerVP(mViewport);
        centerVP.Divide(2.0);
        mViewportTranslation.DifferenceOf(centerVP, vctDouble2(centerBB[mX], centerBB[mY]));
    }
    glTranslated(mViewportTranslation.X(), mViewportTranslation.Y(), 0.0);
    glScaled(mViewportScale, mViewportScale, 1.0);
    const PosesType::const_iterator end = mPoses->end();
    glColor3d(0.0, 0.0, 0.0);
    glPointSize(3.0);
    glBegin(GL_POINTS); {
        for (PosesType::const_iterator pose = mPoses->begin();
             pose != end;
             ++pose) {
            glVertex2d(pose->Element(mX), pose->Element(mY));
        }
    } glEnd();

    // bounding box
    glColor3d(0.7, 0.7, 0.7);
    glBegin(GL_LINE_STRIP); {
        glVertex2d(mBB->MinCorner()[mX], mBB->MinCorner()[mY]);
        glVertex2d(mBB->MaxCorner()[mX], mBB->MinCorner()[mY]);
        glVertex2d(mBB->MaxCorner()[mX], mBB->MaxCorner()[mY]);
        glVertex2d(mBB->MinCorner()[mX], mBB->MaxCorner()[mY]);
        glVertex2d(mBB->MinCorner()[mX], mBB->MinCorner()[mY]);
    } glEnd();
    // zero values
    if (mBB->MinCorner()[mY] * mBB->MaxCorner()[mY] < 0.0) {
        glBegin(GL_LINE_STRIP); {
            glVertex2d(mBB->MinCorner()[mX], 0.0);
            glVertex2d(mBB->MaxCorner()[mX], 0.0);
        } glEnd();
    }
    if (mBB->MinCorner()[mX] * mBB->MaxCorner()[mX] < 0.0) {
        glBegin(GL_LINE_STRIP); {
            glVertex2d(0.0, mBB->MinCorner()[mY]);
            glVertex2d(0.0, mBB->MaxCorner()[mY]);
        } glEnd();
    }

    const char legend[3] = {'x', 'y', 'z'};
    const int font_size = 8;
    QPainter painter(this);
    const QColor txtColor = palette().color(QPalette::Text);
    painter.setPen(txtColor);
    painter.setFont(QFont("Helvetica", font_size));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.drawText(1, 1 + font_size,
                     QString().sprintf("%c: %0.2f",
                                       legend[mY],
                                       mBB->MaxCorner()[mY] * mPrismaticFactor));
    painter.drawText(1, mViewport.Y() - font_size,
                     QString().sprintf("%0.2f/%0.2f",
                                       mBB->MinCorner()[mY] * mPrismaticFactor,
                                       mBB->MinCorner()[mX] * mPrismaticFactor));
    painter.drawText(mViewport.X() - font_size * 10, mViewport.Y() - font_size,
                     QString().sprintf("%c: %0.2f",
                                       legend[mX],
                                       mBB->MaxCorner()[mX] * mPrismaticFactor));
    painter.end();
}


void vctPose3DQtWidgetView::resizeGL(int width, int height)
{
    mViewport.Assign(width, height);
    GLsizei w = static_cast<GLsizei>(width);
    GLsizei h = static_cast<GLsizei>(height);
    glViewport(0 , 0, w , h); // set up viewport
    glMatrixMode(GL_PROJECTION); // set the projection matrix
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}
