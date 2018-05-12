#ifndef CHALLONGEIMPORTER_H
#define CHALLONGEIMPORTER_H

#include <QObject>
#include <QFile>

#include "database.h"
#include "data.h"

class ChallongeImporter : public QObject
{
public:
    ChallongeImporter(QObject* parent = NULL);

    void errMsg( const QString& msg ) const;
    void importFile( const QString& path, const QString& session_tournament_name ) const;
    void import( const QString& data, const QString& session_tournament_name ) const;

    void importMatches(const QJsonObject& tournament,
        const QString& session_date, const QString& session_name,
        const int session_challonge_id, const int session_tournament_id,
        const QString& session_url, QMap<int,PlayerData>& player_data,
        std::vector<MatchData>& match_data) const;

private:
    void importParticipants_p(const QJsonObject& tournament,
        QMap<int,PlayerData>& player_data) const;
    void exportParticipants_p( const QMap<int,PlayerData>& player_data ) const;

    void importMatches_p( const QJsonArray matches,
        std::vector<MatchData>& match_data,
        const QMap<int,PlayerData>& player_data ) const;

    PlayerData player_p( const QMap<int,PlayerData>& player_data,
                         const int p_id ) const;

    QWidget* m_mainwindow;
};

#endif // CHALLONGEIMPORTER_H
