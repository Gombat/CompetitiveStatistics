#include "configparser.h"

#include <QFile>

#include <iostream>

ConfigParser::ConfigParser()
{

}

ConfigParser::~ConfigParser()
{
    //delete m_handler;
}

bool ConfigParser::load(const QString& cfg_file)
{
    m_weeklies.clear();
    m_gamerTagReplacements.clear();

    QFile inputFile(cfg_file);
    if (!inputFile.exists())
    {
        m_lastError = "Config file \"" + cfg_file + "\" does not exist!";
        return false;
    }
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        m_lastError = "Config file \"" + cfg_file + "\" could not be opened!";
        return false;
    }

    QDomDocument doc("doc");
    QString errMsg;
    int errLine, errCol;
    if (!doc.setContent(&inputFile, &errMsg, &errLine, &errCol))
    {

        m_lastError = QString("Config file \"%1\" could not be opened as xml content in Line %2, col %3: %4").arg(cfg_file).arg(errLine).arg(errCol).arg(errMsg);
        inputFile.close();
        return false;
    }
    inputFile.close();

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            //std::cout << qPrintable(e.tagName()) << std::endl; // the node really is an element.
            if (e.tagName().compare("results", Qt::CaseInsensitive) == 0)
            {
                if (!parseWeeklies(e))
                    return false;
            }
            else if (e.tagName().compare("exports", Qt::CaseInsensitive) == 0)
            {
                if (e.hasChildNodes())
                {
                    QDomNodeList children = e.childNodes();
                    for( int i = 0; i < children.size(); i++)
                    {
                        QDomElement childE = children.at(i).toElement();
                        if (!childE.isNull())
                        {
                            if (childE.tagName().compare("elo_ranking", Qt::CaseInsensitive) == 0)
                            {
                                m_elo_ranking_path = childE.attribute("path");
                            }
                            else if (childE.tagName().compare("pvp", Qt::CaseInsensitive) == 0)
                            {
                                m_pvp_path = childE.attribute("path");
                            }
                        }
                    }
                }
            }
            else if (e.tagName().compare("gamertag_replacements", Qt::CaseInsensitive) == 0)
            {
                if (e.hasChildNodes())
                {
                    QDomNodeList children = e.childNodes();
                    for( int i = 0; i < children.size(); i++)
                    {
                        QDomElement childE = children.at(i).toElement();
                        if (!childE.isNull())
                        {
                            if (childE.tagName().compare("tag", Qt::CaseInsensitive) == 0)
                            {
                                QString name = childE.attribute("name");
                                QString replaces = childE.attribute("replaces");
                                if (!name.isEmpty() || !replaces.isEmpty())
                                {
                                    m_gamerTagReplacements[replaces.toLower()] = name;
                                }
                            }
                        }
                    }
                }
            }
        }
        n = n.nextSibling();
    }

    return true;
}

bool ConfigParser::parseWeeklies(QDomElement e)
{
    m_weeklies.clear();

    if (e.hasChildNodes())
    {
        QDomNodeList weeklies = e.childNodes();
        for( int i = 0; i < weeklies.size(); i++)
        {
            QDomElement weekly = weeklies.at(i).toElement();
            if (!weekly.isNull())
            {
                if (weekly.tagName().compare("weekly", Qt::CaseInsensitive) == 0)
                {
                    QString weekly_name = weekly.attribute("name");

                    QDomNodeList files = weekly.childNodes();
                    for( int i = 0; i < files.size(); i++)
                    {
                        QDomElement file = files.at(i).toElement();
                        if (!file.isNull())
                        {
                            if (file.tagName().compare("file", Qt::CaseInsensitive) == 0)
                            {
                                QString filepath = file.attribute("path");
                                QFile fileCheck(filepath);
                                if (!fileCheck.exists())
                                {
                                    m_lastError = QString("Config file contains invalid weekly files! Weekly \"%1\", file \"%2\"").arg(weekly_name).arg(filepath);
                                    return false;
                                }

                                m_weeklies[weekly_name].push_back(filepath);
                            }
                        }
                    }
                }
            }
        }
    }


    return true;
}
