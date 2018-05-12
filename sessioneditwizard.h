#ifndef SESSIONEDITWIZARD_H
#define SESSIONEDITWIZARD_H

#include <QWizard>

#include "sessioneditpagesession.h"
#include "sessioneditpageplayers.h"
#include "sessioneditpagematches.h"
#include "sessioneditpageoverview.h"

class SessionEditWizard : public QWizard
{
    Q_OBJECT
public:
    enum PageID {
        Session,
        Players,
        Matches,
        Overview
    };

    explicit SessionEditWizard(QWidget *parent = 0, QString session = "");

    void initialize();

signals:

public slots:

private:
    QString m_session;
};

#endif // SESSIONEDITWIZARD_H
