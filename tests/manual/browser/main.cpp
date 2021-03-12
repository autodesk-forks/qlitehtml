/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of QLiteHtml.
**
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
** Foundation with exceptions as appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include <qlitehtmlwidget.h>

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>
#include <QWidget>

Q_LOGGING_CATEGORY(log, "qlitehtml.browser")

class BrowserWindow : public QWidget
{
    Q_OBJECT

public:
    BrowserWindow();

private:
    QNetworkAccessManager m_nam;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BrowserWindow w;
    w.show();
    return app.exec();
}

BrowserWindow::BrowserWindow()
{
    QVBoxLayout *vlayout = new QVBoxLayout;
    setLayout(vlayout);
    QHBoxLayout *urlLayout = new QHBoxLayout;
    urlLayout->addWidget(new QLabel(tr("URL:")));
    QLineEdit *urlInput = new QLineEdit;
    urlLayout->addWidget(urlInput);

    vlayout->addLayout(urlLayout);
    QLiteHtmlWidget *htmlWidget = new QLiteHtmlWidget;
    vlayout->addWidget(htmlWidget);

    resize(1000, 650);

    htmlWidget->setResourceHandler([this](const QUrl &url) {
        // create blocking request
        // TODO implement asynchronous requests in container_qpainter
        QEventLoop loop;
        QByteArray data;
        qCDebug(log) << "Resource requested:" << url;
        QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [&data, &loop, reply] {
            qCDebug(log) << "Resource finished:" << reply->url() << reply->error();
            if (reply->error() == QNetworkReply::NoError)
                data = reply->readAll();
            reply->deleteLater();
            loop.exit();
        });
        loop.exec(QEventLoop::ExcludeUserInputEvents);
        return data;
    });
    connect(urlInput, &QLineEdit::returnPressed, htmlWidget, [this, htmlWidget, urlInput] {
        urlInput->setEnabled(false);
        const QUrl url = QUrl(urlInput->text().trimmed());
        qCDebug(log) << "Url requested:" << url;
        QNetworkReply *reply = m_nam.get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, this, [htmlWidget, urlInput, reply] {
            qCDebug(log) << "Url finished:" << reply->url() << reply->error();
            if (reply->error() == QNetworkReply::NoError) {
                const QByteArray data = reply->readAll();
                htmlWidget->setUrl(reply->url());
                htmlWidget->setHtml(QString::fromUtf8(data));
            }
            urlInput->setEnabled(true);
            reply->deleteLater();
        });
    });

    QAction *action;
    action = new QAction(tr("Enter location"));
    action->setShortcut({"Ctrl+L"});
    connect(action, &QAction::triggered, [urlInput] { urlInput->setFocus(); });
    addAction(action);
}

#include "main.moc"
