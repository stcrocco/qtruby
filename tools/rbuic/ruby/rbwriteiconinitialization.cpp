/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "rbwriteiconinitialization.h"
#include "rbwriteicondata.h"
#include "driver.h"
#include "ui5.h"
#include "utils.h"
#include "uic.h"

#include <qtextstream.h>
#include <qstring.h>

QT_BEGIN_NAMESPACE

namespace Ruby {

WriteIconInitialization::WriteIconInitialization(Uic *uic)
    : driver(uic->driver()), output(uic->output()), option(uic->option())
{
    this->uic = uic;
}

void WriteIconInitialization::acceptUI(DomUI *node)
{
    if (node->elementImages() == 0)
        return;

    QString className = node->elementClass() + option.postfix;

    output << option.indent << "def self.icon(id)\n";

    WriteIconData(uic).acceptUI(node);

    output << option.indent << "case id\n";

    TreeWalker::acceptUI(node);

    output << option.indent << option.indent << "else\n";
    output << option.indent << option.indent << "default: return Qt::Pixmap.new\n";

    output << option.indent << option.indent << "end\n"
           << option.indent << "end\n\n";
}

QString WriteIconInitialization::iconFromDataFunction()
{
    return QLatin1String("qt_get_icon");
}

void WriteIconInitialization::acceptImages(DomImages *images)
{
    TreeWalker::acceptImages(images);
}

void WriteIconInitialization::acceptImage(DomImage *image)
{
    QString img = image->attributeName() + QLatin1String("_data");
    QString data = image->elementData()->text();
    QString fmt = image->elementData()->attributeFormat();

    QString imageId = image->attributeName() + QLatin1String("_ID");
    QString imageData = image->attributeName() + QLatin1String("_data");
    QString ind = option.indent + option.indent;

    output << ind << "when " << imageId << "\n";

    if (fmt == QLatin1String("XPM.GZ")) {
      output << option.indent << option.indent << option.indent << "return Qt::Pixmap.new(" << imageData << ")\n";
    } else {
        output << option.indent << option.indent << option.indent << 
          " img = Qt::Image.new\n";
        output << option.indent << option.indent << option.indent << "img.loadFromData(" << imageData << ", " <<
          "imageData.length, " << fixString(fmt, ind) << ")\n";
        output << option.indent << option.indent << option.indent <<
          "return Qt::Pixmap.fromImage(img)\n";
    }
}

} // namespace Ruby

QT_END_NAMESPACE
