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
    bool parseXML(QByteArray ba);
    QList<QMap<QString, QString> > parseXMLtoMaps(QByteArray ba);
    void parseUpnpReply();

    QHash<QString, QString> results() const;
    void setResults(const QHash<QString, QString> &results);

    QByteArray rawData() const;
    void setRawData(const QByteArray &rawData);

    QList<QMap<QString, QString> > getFoundContent() const;

signals:
    void xmlParsed();
    void contentFound();

public slots:
    void parseAnswer();

private:
    QHash<QString, QString> m_results;
    QByteArray m_rawData;
    QList<QMap<QString, QString> > m_foundContent;
};

#endif // PARSER_H
