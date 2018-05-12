#include "sessioneditwizard.h"

SessionEditWizard::SessionEditWizard(QWidget *parent, QString session)
 : QWizard(parent), m_session(session)
{
}

void SessionEditWizard::initialize()
{
    setPage(PageID::Session, new SessionEditPageSession(this, m_session));
    setPage(PageID::Players, new SessionEditPagePlayers(this, m_session));
    setPage(PageID::Matches, new SessionEditPageMatches(this, m_session));
    setPage(PageID::Overview, new SessionEditPageOverview(this, m_session));
}
