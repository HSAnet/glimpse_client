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

void Parser::parseUpnpReply()
{
//    Old code - This was used to write the parser separatly

//    QFile *answer = new QFile("/home/simon/code/QtHttp/massiveAnswer");
//    if (!answer->open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        qDebug() << "Loading File Problem";
//        exit(-1);
//    }
//    QByteArray ba = answer->readAll();
//    QString s(ba);
    QString s(m_rawData);
    int end = s.indexOf("s:Envelope"); //TODO needed?
    s.remove(0, end-1);
    QTextDocumentFragment html;
    QTextDocumentFragment frag = html.fromHtml(s);
    QByteArray realXML;
    realXML.append(frag.toPlainText());
    QList<QMap<QString, QString> > tableOfContents = parseXMLtoMaps(realXML);
    if(tableOfContents.length() != 0)
    {
        m_foundContent = tableOfContents;
        emit contentFound();
    }
    for(int i = 0; i < tableOfContents.length(); i++)
    {
        qDebug() << "*** The Following contents are available in this element: ***";
        QString all = "";
        foreach(QString s, tableOfContents[i].keys())
        {
               all.append(" " + s);
        }
        qDebug() << all;
        qDebug() << tableOfContents[i].value("title");
    }

}

QHash<QString, QString> Parser::results() const
{
    return m_results;
}

void Parser::setResults(const QHash<QString, QString> &results)
{
    m_results = results;
}


bool Parser::parseXML(QByteArray ba)
{
    QXmlStreamReader * xmlReader = new QXmlStreamReader(ba);

    QHash<QString, QString> values;
    QString name;
    QString text;
    //Parse the XML until we reach end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            //If token is just StartDocument - go to next
            if(token == QXmlStreamReader::StartDocument) {
                continue;
            }
            //If token is StartElement - read it
            if(token == QXmlStreamReader::StartElement) {
                if(xmlReader->name() == "root") {
                    continue;
                }
                name = xmlReader->name().toString();
            }
            if(token == QXmlStreamReader::Characters && !name.isEmpty()) {
                text = xmlReader->text().toString();
                values.insert(name, text); //TODO excerpt
                name.clear();
                text.clear();
            }
    }
    if(xmlReader->hasError()){
            qDebug() << "xmlFile.xml Parse Error";
            values.clear();
            return false;
    }
    emit xmlParsed();
    m_results = values;
    return true;
}

/**
 * @brief Parser::parseXMLtoMaps
 * @param ba is the formatted ByteArray (no "&amp" etc. but normal xml code "<\>") that came as answer from the UPnP Server
 * @return It returns a List of QMaps. Each Map is a one element with its title, artists etc. as keys
 */

QList<QMap<QString, QString> > Parser::parseXMLtoMaps(QByteArray ba)
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
        if(token == QXmlStreamReader::StartDocument)
        {
           continue;
        }
        if(token == QXmlStreamReader::StartElement)
        {
            /* The start of a new element which is then marked with the foundElementFlag */
            if(name == "item")
            {
                foundElementFlag = 1;
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
            if(name == "item")
            {
                contents.append(element);
                element.clear();
                foundElementFlag = 0;
            }
        }
    }
    if(xmlReader->hasError()){
            qDebug() << "XML Parse Error";
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

void Parser::parseAnswer()
{
    parseUpnpReply();
}
QList<QMap<QString, QString> > Parser::getFoundContent() const
{
    return m_foundContent;
}

QByteArray Parser::rawData() const
{
    return m_rawData;
}

void Parser::setRawData(const QByteArray &rawData)
{
    m_rawData = rawData;
}

