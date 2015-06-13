#include "parser.h"

#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QTextDocumentFragment>
#include <QXmlStreamReader>
#include <QMap>

Parser::Parser(QObject *parent) : QObject(parent)
{

}

Parser::~Parser()
{

}

QList<QMap<QString, QString> > Parser::parseUpnpReply(int expectedLength)
{
    QList<QMap<QString, QString> > tableOfContents;
    QString s(m_rawData);
    QTextDocumentFragment html;
    QByteArray realXML;
    int end = s.indexOf("s:Envelope");
    if(end == -1)
    {
        qDebug() << "No xml data was found";
        return tableOfContents;
    }
    s.remove(0, end-1);
    if(s.length() != expectedLength)
    {
        throw 400;
    }
    QTextDocumentFragment frag = html.fromHtml(s);
    QString s2 = frag.toPlainText();
    realXML.append(frag.toPlainText());

    tableOfContents = parseXMLtoMaps(realXML, m_searchTerm);
    realXML.clear();
    m_rawData.clear();
    if(m_foundContent != tableOfContents)
    {
        m_foundContent = tableOfContents;
    }else{
        qDebug() << "Nothing new was found on lowest (item-)level";
    }
    return tableOfContents;
}

QHash<QString, QString> Parser::results() const
{
    return m_results;
}

void Parser::setResults(const QHash<QString, QString> &results)
{
    m_results = results;
}


int Parser::parseRootXML(QByteArray ba, QList<QMap<QString, QString> > *contents)
{
    QXmlStreamReader * xmlReader = new QXmlStreamReader(ba);
    QString serviceType = "urn:schemas-upnp-org:service:ContentDirectory:1";
    QString controlURL = "controlURL";
    QString eventSubURL = "eventSubURL";
    int foundElementFlag = 0;
    QMap<QString, QString> element;
    QString key;
    //Parse the XML until the end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            QString name = xmlReader->name().toString();
            QString content = xmlReader->text().toString();
            if(content == serviceType)
            {
                foundElementFlag = 2;
            }
            if(foundElementFlag != 0 &&
                    (name == eventSubURL ||
                     name == controlURL))
            {
                key = (name == controlURL) ? controlURL:eventSubURL;
            }
            if(token == QXmlStreamReader::Characters)
            {
                if(foundElementFlag != 0 && !key.isEmpty())
                {
                    element.insert(key, content);
                    foundElementFlag --;
                }
            }
            if(token == QXmlStreamReader::StartDocument) {
                continue;
            }
            if(token == QXmlStreamReader::StartElement) {
                if(xmlReader->name() == "root") {
                    continue;
                }
            }
            if(token == QXmlStreamReader::EndElement)
            {
                /* At the end of the element it has to appended to the contents list and be cleared */
                if(name == "serviceType" && !element.isEmpty())
                {
                    contents->append(element);
                    element.clear();
                }
            }
    }
    if(xmlReader->hasError()){
            qDebug() << "xmlFile.xml Parse Error";
            return -1;
    }
    return 0;
}

/**
 * @brief Parser::parseXMLtoMaps
 * @param ba is the formatted ByteArray (no "&amp" etc. but normal xml code "<\>") that came as answer from the UPnP Server
 * @return It returns a List of QMaps. Each Map is a one element with its title, artists etc. as keys
 */

QList<QMap<QString, QString> > Parser::parseXMLtoMaps(QByteArray ba, QString elementToSearchFor)
{
    QXmlStreamReader * xmlReader = new QXmlStreamReader(ba);
    QList<QMap<QString, QString> > contents;
    QMap<QString, QString> element;
    QString name;
    QString text;
    QString key;
    int foundElementFlag = 0;

    while(!xmlReader->atEnd() && !xmlReader->hasError())
    {
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        name = xmlReader->name().toString();
        text = xmlReader->text().toString();
        QXmlStreamAttributes atts = xmlReader->attributes();
        if(token == QXmlStreamReader::StartDocument)
        {
           continue;
        }
        if(token == QXmlStreamReader::StartElement)
        {
            /* The start of a new element which is then marked with the foundElementFlag */
            if(name == elementToSearchFor)
            {
                foundElementFlag = 1;
                QStringRef ref = atts.value("id");
                if(ref != "")
                {
                    element.insert("id", ref.toString());
                }
            }
            if(foundElementFlag == 1)
            {
                /* The key has to be saved to insert it into the map when the corresponding value is found */
                key = name;
            }
        }
        if(token == QXmlStreamReader::Characters)
        {
            if(foundElementFlag == 1 && !key.isEmpty())
            {
                element.insert(key, text);
            }
        }
        if(token == QXmlStreamReader::EndElement)
        {
            /* At the end of the element it has to appended to the contents list and be cleared */
            if(name == elementToSearchFor)
            {
                contents.append(element);
                element.clear();
                foundElementFlag = 0;
            }
        }
    }
    if(xmlReader->hasError()){
            /* Since obtaining as much information as possible is the task, the
             * list wont be cleared.
             * An xml error can be arbitrary characters at the
             * end of the byte array.
             * This can happen */
            /* contents.clear(); */
            return contents;
    }
    return contents;
}

QList<QMap<QString, QString> > Parser::getFoundContent() const
{
    return m_foundContent;
}

QString Parser::searchTerm() const
{
    return m_searchTerm;
}

void Parser::setSearchTerm(const QString &searchTerm)
{
    m_searchTerm = searchTerm;
}


QByteArray Parser::rawData() const
{
    return m_rawData;
}

void Parser::setRawData(const QByteArray &rawData)
{
    m_rawData = rawData;
}

