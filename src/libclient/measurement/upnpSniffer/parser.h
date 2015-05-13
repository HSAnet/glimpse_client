#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QHash>

class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);
    ~Parser();
    int parseXML(QByteArray ba);
    QList<QMap<QString, QString> > parseXMLtoMaps(QByteArray ba, QString elementToSearchFor);
    QList<QMap<QString, QString> > parseUpnpReply(int expectedLength);

    QHash<QString, QString> results() const;
    void setResults(const QHash<QString, QString> &results);

    QByteArray rawData() const;
    void setRawData(const QByteArray &rawData);

    QList<QMap<QString, QString> > getFoundContent() const;

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);

signals:
    void xmlParsed();
    void contentFound(QString t);

private:
    QHash<QString, QString> m_results;
    QByteArray m_rawData;
    QList<QMap<QString, QString> > m_foundContent;
    QString m_searchTerm;
};

#endif // PARSER_H
