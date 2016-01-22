#include "FenResponsable.h"
#include "ui_FenResponsable.h"
#include "FenConnexion.h"
#include "WindowCenter.h"
#include "connectionBdd.h"


FenResponsable::FenResponsable(QString nom, QString prenom, int compteur, int idPersonne, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenResponsable)
{
    m_idPersonne = idPersonne;
    ui->setupUi(this);
    ui->centralwidget->hide();

    ui->labelNom->setText("Bonjour "+ prenom + " "+ nom);
    ui->labelCompteur->setText("Vous disposez d'un compteur de "+ QString::number(compteur) +" 1/2 journée");
}
FenResponsable::~FenResponsable()
{
    delete ui;
}

void FenResponsable::on_actionQuitter_triggered()
{
    this->close();
}

void FenResponsable::on_actionFaire_une_demande_triggered()
{
    ui->centralwidget->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void FenResponsable::on_calendrierDebut_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeDButDateTimeEdit;
    creationQdialogCalendrier();
}

void FenResponsable::on_calendrierFin_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeFinDateTimeEdit;
    creationQdialogCalendrier();
}


void FenResponsable::creationQdialogCalendrier()
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

void FenResponsable::selectionDate()
{
    dateEtHeureEdit->setDate(calendrier->selectedDate());
    fenetreCalendrier->close();
}


void FenResponsable::on_actionConsulter_demande_s_triggered()
{
    if(!connectionExiste)
        createConnection();

    QSqlQueryModel *modelSelect = new QSqlQueryModel;

    proxyModel = new QSortFilterProxyModel();

    modelSelect->setQuery("SELECT idDemande, nom, prenom, DATE_FORMAT(dateDebut, '%d/%m/%Y %H:%i'), DATE_FORMAT(dateFin, '%d/%m/%Y %H:%i'), statut FROM Demande d, Personne p where d.idPersonne = p.idPersonne AND (role = 'Employe' OR role ='Responsable')");

    if(modelSelect->query().isActive()) // test la validité de la requête sql
    {
        modelSelect->setHeaderData(0, Qt::Horizontal, QObject::tr("N° Demande"));
        modelSelect->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
        modelSelect->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
        modelSelect->setHeaderData(3, Qt::Horizontal, QObject::tr("Date de début"));
        modelSelect->setHeaderData(4, Qt::Horizontal, QObject::tr("Date de fin"));
        modelSelect->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));


        ui->tableView->setModel(proxyModel);

        proxyModel->setSourceModel(modelSelect);

        ui->centralwidget->show();

        ui->stackedWidget->setCurrentIndex(1);
    }
    else
        QMessageBox::critical(this, "Erreur", "Erreur de base de donnée : " + modelSelect->query().lastError().text());
}

void FenResponsable::on_toolButton_clicked()    // bouton filtre
{
    fenetreFiltre = new QDialog(this);
    QVBoxLayout *layoutFiltre = new QVBoxLayout;


    QLabel *filtreTexte = new QLabel("Filtrer par ...");


    filtreNom = new QLineEdit;
    filtrePrenom = new QLineEdit;
    filtreDDebut = new QDateTimeEdit;
    filtreDFin = new QDateTimeEdit;
    filtreStatut = new QComboBox;

    choixFiltre = new QComboBox;

    filtreNom->setVisible(true);
    filtrePrenom->setVisible(false);
    filtreDDebut->setVisible(false);
    filtreDFin->setVisible(false);
    filtreStatut->setVisible(false);

    filtreDDebut->setDate(QDate::currentDate());
    filtreDFin->setDate(QDate::currentDate());
    filtreDDebut->setCalendarPopup(true);
    filtreDFin->setCalendarPopup(true);

    filtreStatut->addItem("Validée");
    filtreStatut->addItem("Refus RH");
    filtreStatut->addItem("Refus Responsable");
    filtreStatut->addItem("Responsable");
    filtreStatut->addItem("RH");

    choixFiltre->addItem("Nom");
    choixFiltre->addItem("Prénom");
    choixFiltre->addItem("Date de début");
    choixFiltre->addItem("Date de fin");
    choixFiltre->addItem("Statut de la demande");



    layoutFiltre->addWidget(filtreTexte);
    layoutFiltre->addWidget(choixFiltre);

    QHBoxLayout *hboxNom = new QHBoxLayout;
    hboxNom->addWidget(filtreNom);
    QHBoxLayout *hboxPrenom = new QHBoxLayout;
    hboxPrenom->addWidget(filtrePrenom);
    QHBoxLayout *hboxDDebut = new QHBoxLayout;
    hboxDDebut->addWidget(filtreDDebut);
    QHBoxLayout *hboxDFin = new QHBoxLayout;
    hboxDFin->addWidget(filtreDFin);
    QHBoxLayout *hboxStatut = new QHBoxLayout;
    hboxStatut->addWidget(filtreStatut);

    layoutFiltre->addLayout(hboxNom);
    layoutFiltre->addLayout(hboxPrenom);
    layoutFiltre->addLayout(hboxDDebut);
    layoutFiltre->addLayout(hboxDFin);
    layoutFiltre->addLayout(hboxStatut);

    QHBoxLayout * hboxValide = new QHBoxLayout;
    QPushButton *valideFiltre = new QPushButton("Valider");

    valideFiltre->setMaximumWidth(100);

    hboxValide->addWidget(valideFiltre);
    layoutFiltre->addLayout(hboxValide);

    fenetreFiltre->setLayout(layoutFiltre);

    fenetreFiltre->show();

    connect(valideFiltre, SIGNAL(clicked()), this, SLOT(valideFiltre_clicked()));
    connect(choixFiltre, SIGNAL(currentTextChanged(QString)), this, SLOT(choixBoxFiltre(QString)));

}

void FenResponsable::choixBoxFiltre(QString choix)
{
    if(choix == "Nom")
    {
        filtreNom->setVisible(true);
    }
    else
        filtreNom->setVisible(false);
    if(choix == "Prénom")
    {
        filtrePrenom->setVisible(true);
    }
    else
        filtrePrenom->setVisible(false);
    if(choix == "Date de début")
    {
        filtreDDebut->setVisible(true);
    }
    else
        filtreDDebut->setVisible(false);
    if(choix == "Date de fin")
        filtreDFin->setVisible(true);
    else
        filtreDFin->setVisible(false);
    if(choix == "Statut de la demande")
        filtreStatut->setVisible(true);
    else
        filtreStatut->setVisible(false);
}

void FenResponsable::valideFiltre_clicked()
{
    fenetreFiltre->close();
    if(choixFiltre->currentText() == "Nom" && !filtreNom->text().isEmpty())
    {
        proxyModel->setFilterRegExp(QRegExp(filtreNom->text(), Qt::CaseInsensitive,
                                                     QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(1);


    }
    if(choixFiltre->currentText() == "Prénom" && !filtrePrenom->text().isEmpty())
    {
        proxyModel->setFilterRegExp(QRegExp(filtrePrenom->text(), Qt::CaseInsensitive,
                                                     QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(2);
    }
    if(choixFiltre->currentText() == "Statut de la demande")
    {
        proxyModel->setFilterRegExp(QRegExp(filtreStatut->currentText(), Qt::CaseInsensitive,
                                                     QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(5);
    }
    if(choixFiltre->currentText() == "Date de début")
    {
        proxyModel->setFilterRegExp(QRegExp(filtreDDebut->dateTime().toString("dd/MM/yyyy hh:mm"), Qt::CaseInsensitive,
                                                     QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(3);

    }
    if(choixFiltre->currentText() == "Date de fin")
    {
        proxyModel->setFilterRegExp(QRegExp(filtreDFin->dateTime().toString("dd/MM/yyyy hh:mm"), Qt::CaseInsensitive,
                                                     QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(4);
    }

}

void FenResponsable::on_actionDeconnection_triggered()
{
    FenConnexion *fenetre = new FenConnexion;
    fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->close();
    setCenterOfApplication(fenetre);
    fenetre->show();
}

void FenResponsable::on_buttonValider_clicked()
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
        query.bindValue(":statut", "RH");
        query.bindValue(":idPersonne", m_idPersonne);
        query.bindValue(":nbJour", nbJour);
        query.exec();

        if(query.isActive())
            QMessageBox::information(this, "Confirmation de validation", "Votre demande a bien été enregistrée");
        else
            QMessageBox::critical(this, "Erreur", "Erreur de base de donnée : " + query.lastError().text());
    }
    else
        QMessageBox::critical(this, "Erreur de saisie", "Demande incorrect, les dates choisies ne sont pas valides");
}

bool FenResponsable::validationDemande()
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

void FenResponsable::on_buttonSupprimer_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel(); // selection de la data dans le tableau

    if(select->hasSelection())
    {
        QMessageBox msgBox(
                    QMessageBox::Question,
                    trUtf8("Supprimer la demande "),
                    "Etes vous sûr(e) ?",
                    QMessageBox::Yes | QMessageBox::No, this);

        msgBox.setButtonText(QMessageBox::Yes, trUtf8("Oui"));
        msgBox.setButtonText(QMessageBox::No, trUtf8("Non"));


       if(msgBox.exec() == QMessageBox::Yes)
       {

            QSqlQuery query;
            query.exec("DELETE FROM Demande WHERE idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );

            if(query.isActive())
                QMessageBox::information(this, "Confirmation de suppression", "Votre demande a bien été supprimée");
            else
                QMessageBox::critical(this, "Erreur", "Erreur de base de donnée : " + query.lastError().text());

            on_actionConsulter_demande_s_triggered();
       }

    }
    else
        QMessageBox::critical(this, "Erreur", "Aucune sélection à supprimer");
}

void FenResponsable::on_buttonValideDemande_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 5)).toString() == "Responsable")
    {
        QMessageBox msgBox(
                    QMessageBox::Question,
                    trUtf8("Valider la demande"),
                    "Etes vous sûr(e) ?",
                    QMessageBox::Yes | QMessageBox::No, this);

        msgBox.setButtonText(QMessageBox::Yes, trUtf8("Oui"));
        msgBox.setButtonText(QMessageBox::No, trUtf8("Non"));


       if(msgBox.exec() == QMessageBox::Yes)
       {
            QSqlQuery query;
            query.exec("UPDATE Demande SET statut = 'RH' WHERE idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );
            on_actionConsulter_demande_s_triggered();

            if(query.isActive())
                QMessageBox::information(this, "Confirmation", "Demande validée et Transmise au service RH");
            else
                QMessageBox::critical(this, "Erreur", "Erreur de base de donnée : " + query.lastError().text());
       }
    }
    else
        QMessageBox::critical(this, "Erreur", "Vous ne pouvez pas valider cette demande");
}

void FenResponsable::on_buttonRefuserDemande_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 5)).toString() == "Responsable")
    {
        QMessageBox msgBox(
                    QMessageBox::Question,
                    trUtf8("Refuser la demande"),
                    "Etes vous sûr(e) ?",
                    QMessageBox::Yes | QMessageBox::No, this);

        msgBox.setButtonText(QMessageBox::Yes, trUtf8("Oui"));
        msgBox.setButtonText(QMessageBox::No, trUtf8("Non"));


       if(msgBox.exec() == QMessageBox::Yes)
       {
            QSqlQuery query;
            query.exec("UPDATE Demande SET statut = 'Refus Responsable' WHERE idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );
            on_actionConsulter_demande_s_triggered();

            if(query.isActive())
                QMessageBox::information(this, "Confimation", "Demande refusée");
            else
                QMessageBox::critical(this, "Erreur", "Erreur de base de donnée : " + query.lastError().text());

       }

    }
    else
        QMessageBox::critical(this, "Erreur", "Vous ne pouvez pas refuser cette demande");
}



void FenResponsable::on_pushButton_clicked()
{
    on_actionConsulter_demande_s_triggered();
}
