#include "fontpushbutton.h"

#include <QFontDialog>

namespace Widgets
{

FontPushButton::FontPushButton(QWidget *parent) :
    QPushButton(parent)
{
    init();
}

FontPushButton::FontPushButton(const QString &text, QWidget *parent): QPushButton(text, parent)
{
    init();
}

void FontPushButton::onClicked()
{
    bool ok = false;
    QFont f = QFontDialog::getFont(&ok, font());

    if (!ok)
        return;
    emit fontSelected(f);
    int fontSize = this->font().pointSize();
    f.setPointSize(fontSize);
    setFont(f);
    setText(f.family());
    setToolTip(f.family());

}

void FontPushButton::init()
{
    setText(tr("Font"));
    connect(this, SIGNAL(clicked()), SLOT(onClicked()));
}
}
