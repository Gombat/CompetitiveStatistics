#include "sessioneditpageplayers.h"
#include "sessioneditpagesession.h"
#include "sessioneditwizard.h"

#include <QAbstractButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringList>
#include <QVariant>
#include <QWizard>

#include "database.h"

SessionEditPagePlayers::SessionEditPagePlayers(QWidget *parent, QString session)
 : QWizardPage(parent), m_session_name_init(session)
{ }

void SessionEditPagePlayers::initializePage()
{
    setTitle( "Edit Session - Players" );

    m_list = new QListWidget(this);
    m_list->setSortingEnabled(true);
    if ( !m_session_name_init.isEmpty() )
    { m_list->addItems(Database::players(m_session_name_init)); }
    for ( int i = 0; i < m_list->count(); i++ )
    { m_list->item(i)->setFlags(m_list->item(i)->flags() | Qt::ItemIsEditable); }
    m_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    m_add = new QPushButton("add player", this);
    QObject::connect(m_add, SIGNAL(clicked(bool)), this, SLOT(onAdd(bool)));
    m_del = new QPushButton("delete selected", this);
    QObject::connect(m_del, SIGNAL(clicked(bool)), this, SLOT(onDel(bool)));

    SessionEditPageSession* pageSession = qobject_cast<SessionEditPageSession*>(
        wizard()->page(SessionEditWizard::PageID::Session));
    if ( pageSession )
    {
        m_add->setEnabled(false);
        m_del->setEnabled(false);
    }

    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_list,0,0,1,2);
    m_layout->addWidget(m_add,1,0);
    m_layout->addWidget(m_del,1,1);
}

void SessionEditPagePlayers::onAdd(bool)
{
    QString name = QInputDialog::getText(this, "Add Player", "Player Name: ");
    if ( players().contains(name,Qt::CaseInsensitive) )
    {
        QMessageBox::warning( this, "Add Player",
            QString( "Player \"%1\" already is in list!" ).arg(name) );
        return;
    }
    m_list->addItem(name);
}

void SessionEditPagePlayers::onDel(bool)
{
    QList<QListWidgetItem*> selected = m_list->selectedItems();
    if ( selected.count() )
    { m_list->takeItem(m_list->row(selected.at(0))); }
}

QStringList SessionEditPagePlayers::players() const
{
    QStringList players;
    for ( int i = 0; i < m_list->count(); i++ )
    { players << m_list->item(i)->text(); }
    return players;
}

bool SessionEditPagePlayers::validatePage()
{
    return true;
}
