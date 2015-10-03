/*
Copyright (c) 2007, Trenton Schulz

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BONJOURSERVICEBROWSER_H
#define BONJOURSERVICEBROWSER_H

#include <QtCore/QObject>
#include <dns_sd.h>
#include <QTimer>
#include "bonjourrecord.h"

class QSocketNotifier;
class BonjourServiceBrowser : public QObject
{
    Q_OBJECT
public:
    BonjourServiceBrowser(QObject *parent = 0);
    ~BonjourServiceBrowser();
    inline QList<BonjourRecord> currentRecords() const { return _bonjourRecords; }
    inline QString serviceType() const { return browsingType; }
    QList<BonjourRecord> _bonjourRecords;
    void setInterval(int interval);

signals:
    void currentBonjourRecordsChanged(const QList<BonjourRecord> &list);
    void error(DNSServiceErrorType err);
    void browse(QString dev);
    void finished();

public slots:
    void browseForServiceType(const QString &serviceType);
    void cleanUp();
    void browseForFoundServiceTypes();
    void finishBrowseAttempt();

private slots:
    void bonjourSocketReadyRead();
    void handleError(DNSServiceErrorType err);

private:
    static void DNSSD_API bonjourBrowseReply(DNSServiceRef , DNSServiceFlags flags, quint32,
                                   DNSServiceErrorType errorCode, const char *serviceName,
                                   const char *regType, const char *replyDomain, void *context);
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    QString browsingType;
    int _interval;
};

#endif // BONJOURSERVICEBROWSER_H
