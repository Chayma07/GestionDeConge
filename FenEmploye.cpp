#include "FenEmploye.h"
#include "ui_fenEmploye.h"
#include "FenConnexion.h"
#include "WindowCenter.h"
#include "connectionBdd.h"

FenEmploye::FenEmploye(QString nom, QString prenom, int compteur, int idPersonne, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenEmploye)
{
    m_idPersonne = idPersonne;
    ui->setupUi(this);
    ui->centralwidget->hide();


    ui->labelNom->setText("Bonjour "+ prenom + " "+ nom);
    ui->labelCompteur->setText("Vous disposez d'un compteur de "+ QString::number(compteur) +" 1/2 journée");
}

FenEmploye::~FenEmploye()
{
    delete ui;
}

void FenEmploye::on_actionQuitter_triggered()
{
    this->close();
}

void FenEmploye::on_actionFaire_une_demande_triggered()
{
    ui->centralwidget->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void FenEmploye::on_calendrierDebut_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeDButDateTimeEdit;
    creationQdialogCalendrier();
}

void FenEmploye::on_calendrierFin_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeFinDateTimeEdit;
    creationQdialogCalendrier();
}


void FenEmploye::creationQdialogCalendrier()
{
    fenetreCalendrier = new QDialog(this);
    QHBoxLayout layout;

    calendrier = new QCalendarWidget;
    QPushButton *buttonValider = new QPushButton("Valider");

    layout.addWidget(calendrier);
    layout.addWidget(buttonValider);

    fenetreCalendrier->setLayout(&layout);

    connect(buttonValider, SIGNAL(clicked()), this, SLOT(selectionDate()));

    fenetreCalendrier->exec();
}

void FenEmploye::selectionDate()
{
    dateEtHeureEdit->setDate(calendrier->selectedDate());
    fenetreCalendrier->close();
}


void FenEmploye::on_actionConsulter_demande_s_triggered()
{
    if(!connectionExiste)
        createConnection();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();

    model->setQuery("SELECT idDemande, DATE_FORMAT(dateDebut, '%d/%m/%Y %H:%i'), DATE_FORMAT(dateFin, '%d/%m/%Y %H:%i'), statut FROM Demande WHERE idPersonne = 1");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("N° Demande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Date de début"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date de fin"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Statut"));

    ui->tableView->setModel(proxyModel);

    proxyModel->setSourceModel(model);

    ui->centralwidget->show();

    ui->stackedWidget->setCurrentIndex(1);
}

void FenEmploye::on_actionDeconnection_triggered()
{
    FenConnexion *fenetre = new FenConnexion;
    fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->close();
    setCenterOfApplication(fenetre);
    fenetre->show();
}

void FenEmploye::on_buttonValider_clicked()
{
    if(!connectionExiste)
        createConnection();


    int nbJour = 0;

    QDate dDebut = ui->dateEtHeureDeDButDateTimeEdit->date();
    QDate dFin = ui->dateEtHeureDeFinDateTimeEdit->date();

    if(dDebut < dFin)
    {
        for(int i = dDebut.dayOfYear(); i < ui->dateEtHeureDeFinDateTimeEdit->date().dayOfYear(); i++)
        {


            if(dDebut.dayOfWeek() != 6 && dDebut.dayOfWeek() != 7)
                nbJour++;

            dDebut = dDebut.addDays(1);
        }
        nbJour *= 2; // convertion en 1/2 journée
    }

    int hFin = ui->dateEtHeureDeFinDateTimeEdit->time().hour();
    int hDebut = ui->dateEtHeureDeDButDateTimeEdit->time().hour();

    if(hFin != hDebut)
    {

        if(hDebut < 8)
            hDebut = 8;



        if( hFin - hDebut < 5 )
            nbJour += 1; // une demi journée
        else
            nbJour += 2; // une journée complète
    }

    if(validationDemande())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO Demande (idDemande, dateDebut, dateFin, statut, idPersonne, nbJour) "
                       "VALUES (NULL, :dateDebut, :dateFin, :statut, :idPersonne, :nbJour)");
        query.bindValue(":dateDebut", ui->dateEtHeureDeDButDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm"));
        query.bindValue(":dateFin", ui->dateEtHeureDeFinDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm"));
        query.bindValue(":statut", "Responsable");
        query.bindValue(":idPersonne", m_idPersonne);
        query.bindValue(":nbJour", nbJour);
        query.exec();

        QMessageBox::information(this, "Confirmation de validation", "Votre demande a bien été enregistrée");

    }
    else
        QMessageBox::critical(this, "Erreur de saisie", "Demande incorrect, les dates choisies ne sont pas valides");
}

bool FenEmploye::validationDemande()
{
    bool valide = true;

    if(ui->dateEtHeureDeDButDateTimeEdit->dateTime() == QDateTime(QDate(2000,01,01)) || ui->dateEtHeureDeFinDateTimeEdit->dateTime() == QDateTime(QDate(2000,01,01)))
        valide = false;

    if(ui->dateEtHeureDeDButDateTimeEdit->dateTime() > ui->dateEtHeureDeFinDateTimeEdit->dateTime())
        valide = false;

    if(ui->dateEtHeureDeDButDateTimeEdit->dateTime() == ui->dateEtHeureDeFinDateTimeEdit->dateTime())
        valide = false;

    if (ui->dateEtHeureDeDButDateTimeEdit->dateTime() < QDateTime::currentDateTime())
        valide = false;

    return valide;
}

