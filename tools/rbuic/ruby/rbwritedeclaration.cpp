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

#include "rbwritedeclaration.h"
#include "rbwriteicondeclaration.h"
#include "rbwriteinitialization.h"
#include "rbwriteiconinitialization.h"
#include "rbextractimages.h"
#include "driver.h"
#include "ui5.h"
#include "uic.h"
#include "databaseinfo.h"
#include "customwidgetsinfo.h"

#include <qtextstream.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

namespace {
    void openNameSpaces(const QStringList &namespaceList, QTextStream &output) {
        if (namespaceList.empty())
            return;
        const QStringList::const_iterator cend = namespaceList.constEnd();
        for (QStringList::const_iterator it = namespaceList.constBegin(); it != cend; ++it) {
            if (!it->isEmpty()) {
                output << "module " << *it << "\n";
            }
        }
    }

    void closeNameSpaces(const QStringList &namespaceList, QTextStream &output) {
        if (namespaceList.empty())
            return;

        QListIterator<QString> it(namespaceList);
        it.toBack();
        while (it.hasPrevious()) {
            const QString ns = it.previous();
            if (!ns.isEmpty()) {
                output << "end #namespace " << ns << "\n";
            }
        }
    }
}

namespace Ruby {

WriteDeclaration::WriteDeclaration(Uic *uic)  :
    m_uic(uic),
    m_driver(uic->driver()),
    m_output(uic->output()),
    m_option(uic->option())
{
}

void WriteDeclaration::acceptUI(DomUI *node)
{
    QString qualifiedClassName = node->elementClass() + m_option.postfix;
    //Ensure the class name starts with a capital letter
    QString className = qualifiedClassName.mid(0, 1).toUpper() + qualifiedClassName.mid(1);

    QString varName = m_driver->findOrInsertWidget(node->elementWidget());
    QString widgetClassName = node->elementWidget()->attributeClass();

    QStringList namespaceList = qualifiedClassName.split(QLatin1String("::"));
    if (namespaceList.count() > 0) {
        className = namespaceList.last().mid(0, 1).toUpper() +namespaceList.last().mid(1);
        namespaceList.removeLast();
    }

    openNameSpaces(namespaceList, m_output);

    if (namespaceList.count())
        m_output << "\n";

    m_output << "class " << m_option.prefix << className << "\n";

    const QStringList connections = m_uic->databaseInfo()->connections();
    for (int i=0; i<connections.size(); ++i) {
        const QString connection = connections.at(i);

        if (connection == QLatin1String("(default)"))
            continue;

        m_output << m_option.indent << "@" << connection << "Connection = Qt::SqlDatabase.new\n";
    }

    TreeWalker::acceptWidget(node->elementWidget());
    if (const DomButtonGroups *domButtonGroups = node->elementButtonGroups())
        acceptButtonGroups(domButtonGroups);

    m_output << "\n";

    WriteInitialization(m_uic).acceptUI(node);
//TODO: fix this
    if (node->elementImages()) {
        if (m_option.extractImages) {
            ExtractImages(m_uic->option()).acceptUI(node);
        } else {
            m_output << "\n"
                << "protected:\n"
                << m_option.indent << "enum IconID\n"
                << m_option.indent << "{\n";
            WriteIconDeclaration(m_uic).acceptUI(node);

            m_output << m_option.indent << m_option.indent << "unknown_ID\n"
                << m_option.indent << "};\n";

            WriteIconInitialization(m_uic).acceptUI(node);
        }
    }

    m_output << "end\n\n";

    closeNameSpaces(namespaceList, m_output);

    if (namespaceList.count() > 0)
        m_output << "\n";

    if (m_option.generateNamespace && !m_option.prefix.isEmpty()) {
        namespaceList.append(QLatin1String("Ui"));

        openNameSpaces(namespaceList, m_output);
        QString indent;
//        for(int i = 0; i < namespaceList.length(); ++i) indent << m_option.indent;
        m_output << m_option.indent << "class " << className << '<' << m_option.prefix << className << "\n" << m_option.indent << "end\n";

        closeNameSpaces(namespaceList, m_output);

        if (namespaceList.count())
            m_output << "\n";
    }
}

void WriteDeclaration::acceptWidget(DomWidget *node)
{
    QString className = QLatin1String("Qt::Widget");
    if (node->hasAttributeClass())
        className = node->attributeClass();

    QString item = m_driver->findOrInsertWidget(node);
    item = item.mid(0, 1).toLower() + item.mid(1);
    m_output << m_option.indent << "attr_reader :" << item << "\n";

    TreeWalker::acceptWidget(node);
}

void WriteDeclaration::acceptSpacer(DomSpacer *node)
{
     m_output << m_option.indent << "attr_reader :" << m_driver->findOrInsertSpacer(node) << "\n";
     TreeWalker::acceptSpacer(node);
}

void WriteDeclaration::acceptLayout(DomLayout *node)
{
    QString className = QLatin1String("Qt::Layout");
    if (node->hasAttributeClass())
        className = node->attributeClass();

    QString item = m_driver->findOrInsertLayout(node);
    item = item.mid(0, 1).toLower() + item.mid(1);
    m_output << m_option.indent << "attr_reader :" << item << "\n";

    TreeWalker::acceptLayout(node);
}

void WriteDeclaration::acceptActionGroup(DomActionGroup *node)
{
    QString item = m_driver->findOrInsertActionGroup(node);
    item = item.mid(0, 1).toLower() + item.mid(1);
    m_output << m_option.indent << "attr_reader :" << item << "\n";

    TreeWalker::acceptActionGroup(node);
}

void WriteDeclaration::acceptAction(DomAction *node)
{
    QString item = m_driver->findOrInsertAction(node);
    item = item.mid(0, 1).toLower() + item.mid(1);
    m_output << m_option.indent << "attr_reader :" << item << "\n";

    TreeWalker::acceptAction(node);
}

void WriteDeclaration::acceptButtonGroup(const DomButtonGroup *buttonGroup)
{
    QString item = m_driver->findOrInsertButtonGroup(buttonGroup);
    item = item.mid(0, 1).toLower() + item.mid(1);
    m_output << m_option.indent << "attr_reader :" << item << "\n";

    TreeWalker::acceptButtonGroup(buttonGroup);
}

} // namespace Ruby

QT_END_NAMESPACE
