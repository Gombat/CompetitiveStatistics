#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QDomDocument>

class ConfigParser
{
public:
    ConfigParser();
    virtual ~ConfigParser();

    bool load(const QString& cfg_file);

    const QMap<QString, QVector<QString> >& weeklies() const { return m_weeklies; }
    const QMap<QString, QString>& gamerTagReplacements() const { return m_gamerTagReplacements; }

    QString lastError() const { return m_lastError; }

private:
    bool parseWeeklies(QDomElement e);

    QString m_lastError;

    QString m_elo_ranking_path;
    QString m_pvp_path;

    QMap<QString, QVector<QString> > m_weeklies;
    QMap<QString, QString> m_gamerTagReplacements;
};

#endif // CONFIGPARSER_H
