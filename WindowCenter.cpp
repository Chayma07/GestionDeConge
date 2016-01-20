#include "WindowCenter.h"

void setCenterOfApplication(QWidget* widget)
{
/*
QSize size = widget->sizeHint();
QDesktopWidget* desktop = QApplication::desktop();
int width = desktop->width();
int height = desktop->height();
int mw = size.width();
int mh = size.height();
int centerW = (width/2) - (mw/2);
int centerH = (height/2) - (mh);
*/
widget->move(0, 0);
}
