#include "Fenrh.h"
#include "ui_Fenrh.h"
#include "FenConnexion.h"
#include "WindowCenter.h"
#include "connectionBdd.h"
#include <QDebug>

FenRh::FenRh(QString nom, QString prenom, int compteur, int idPersonne, QString statutUtilisateur, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenRh)
{
    m_statutUtilisateur = statutUtilisateur;
    m_idPersonne = idPersonne;
    ui->setupUi(this);
    ui->centralwidget->hide();

    ui->labelNom->setText("Bonjour "+ prenom + " "+ nom);
    ui->labelCompteur->setText("Vous disposez d'un compteur de "+ QString::number(compteur) +" 1/2 journée");
}

FenRh::~FenRh()
{
    delete ui;
}

void FenRh::on_actionQuitter_triggered()
{
    this->close();
}

void FenRh::on_actionFaire_une_demande_triggered()
{
    ui->centralwidget->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void FenRh::on_calendrierDebut_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeDButDateTimeEdit;
    creationQdialogCalendrier();
}

void FenRh::on_calendrierFin_clicked()
{
    dateEtHeureEdit = ui->dateEtHeureDeFinDateTimeEdit;
    creationQdialogCalendrier();
}


void FenRh::creationQdialogCalendrier()
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

void FenRh::selectionDate()
{
    dateEtHeureEdit->setDate(calendrier->selectedDate());
    fenetreCalendrier->close();
}


void FenRh::on_actionConsulter_demande_s_triggered()
{
    if(!connectionExiste)
        createConnection();

    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT idDemande, nom, prenom, DATE_FORMAT(dateDebut, '%d/%m/%Y %H:%i'), DATE_FORMAT(dateFin, '%d/%m/%Y %H:%i'), statut FROM Demande d, Personne p where d.idPersonne = p.idPersonne AND (role = 'Employe' OR role ='Responsable' OR role = 'RH') AND statut != 'Responsable' AND statut != 'Refus Responsable'");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("N° Demande"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Date de début"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Date de fin"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Statut"));

    ui->tableView->setModel(model);

    ui->centralwidget->show();

    ui->stackedWidget->setCurrentIndex(1);
}

void FenRh::on_actionDeconnection_triggered()
{
    FenConnexion *fenetre = new FenConnexion;
    fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->close();
    setCenterOfApplication(fenetre);
    fenetre->show();
}

void FenRh::on_buttonValider_clicked()
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
        query.bindValue(":statut", "Responsable RH");
        query.bindValue(":idPersonne", m_idPersonne);
        query.bindValue(":nbJour", nbJour);
        query.exec();

        QMessageBox::information(this, "Confirmation de validation", "Votre demande a bien été enregistrée");

    }
    else
        QMessageBox::critical(this, "Erreur de saisie", "Demande incorrect, les dates choisies ne sont pas valides");
}

bool FenRh::validationDemande()
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

void FenRh::on_buttonSupprimer_clicked()
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
            on_actionConsulter_demande_s_triggered();
       }

    }
    else
        QMessageBox::critical(this, "Erreur", "Aucune sélection à supprimer");
}

void FenRh::on_buttonValideDemande_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();

    if (ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 5)).toString() == "RH" || m_statutUtilisateur == "Responsable RH")
    {

        QSqlQuery query("SELECT compteur, nbJour FROM Personne p, Demande d WHERE p.idPersonne = d.idPersonne AND idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );

        int totalCompteur = 0, totalJour = 0;

        while (query.next()) {

                totalCompteur = query.value(0).toInt();
                totalJour = query.value(1).toInt();


        }

        if(totalCompteur - totalJour > 0)
        {
            totalCompteur -= totalJour;



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
                query.exec("UPDATE Demande d, Personne p SET statut = 'Validée', compteur = '" + QString::number(totalCompteur) + "' WHERE p.idPersonne = d.idPersonne AND idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );
                on_actionConsulter_demande_s_triggered();

                QMessageBox::information(this, "Confimation", "Demande validée");
           }
        }
        else
        {
             QMessageBox::critical(this, "Erreur", "Compteur insuffisant");
        }
    }
    else
        QMessageBox::critical(this, "Erreur", "Vous ne pouvez pas valider cette demande");
}

void FenRh::on_buttonRefuserDemande_clicked()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 5)).toString() == "RH" || m_statutUtilisateur == "Responsable RH")
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
            query.exec("UPDATE Demande SET statut = 'Refus RH' WHERE idDemande = '" + ui->tableView->model()->data(ui->tableView->model()->index(select->currentIndex().row(), 0)).toString() + "'" );
            on_actionConsulter_demande_s_triggered();

            QMessageBox::information(this, "Confimation", "Demande refusée");
       }

    }
    else
        QMessageBox::critical(this, "Erreur", "Vous ne pouvez pas refuser cette demande");
}

void FenRh::on_actionAjouter_une_personne_triggered()
{
    ui->centralwidget->show();
    ui->stackedWidget->setCurrentIndex(2);
}

void FenRh::on_pushButton_clicked()
{
    if(!connectionExiste)
        createConnection();

    if(validationFormulaire())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO Personne (idPersonne, nom, prenom, identifiant, mdp, role, compteur) "
                       "VALUES (NULL, :nom, :prenom, :identifiant, :mdp, :role, :compteur)");

        query.bindValue(":nom", ui->nomLineEdit->text());
        query.bindValue(":prenom", ui->prNomLineEdit->text());
        query.bindValue(":identifiant", ui->identifiantLineEdit->text());
        query.bindValue(":mdp", ui->motDePasseLineEdit->text());
        query.bindValue(":role", ui->statutComboBox->currentText());
        query.bindValue(":compteur", ui->compteurSpinBox->value());
        query.exec();

        QMessageBox::information(this, "Confirmation de validation", "La personne a bien été enregistrée");

    }
    else
        QMessageBox::critical(this, "Erreur de saisie", "Demande incorrect, tous les champs du formulaire sont à compléter");
}

bool FenRh::validationFormulaire()
{
    bool valide = true;

    if(ui->nomLineEdit->text().isEmpty())
        valide = false;
    if(ui->prNomLineEdit->text().isEmpty())
        valide = false;
    if(ui->identifiantLineEdit->text().isEmpty())
        valide = false;
    if(ui->motDePasseLineEdit->text().isEmpty())
        valide = false;
    if(ui->compteurSpinBox->value() == 0)
        valide = false;

    return valide;
}

void FenRh::on_actionModifier_une_personne_triggered()
{
    if(!connectionExiste)
        createConnection();

    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT nom, prenom, identifiant, mdp, role, compteur FROM Personne");


    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Identifiant"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Mot de passe"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Role"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Compteur"));

    ui->tableView_2->setModel(model);
    ui->centralwidget->show();
    ui->stackedWidget->setCurrentIndex(3);
}

void FenRh::on_pushButton_2_clicked()
{

    QItemSelectionModel *select = ui->tableView_2->selectionModel(); // selection de la data dans le tableau
    if(select->hasSelection())
    {


        QSqlQuery query;
        query.exec("SELECT nom, prenom, identifiant, mdp, role, compteur FROM Personne WHERE identifiant = '" + ui->tableView_2->model()->data(ui->tableView_2->model()->index(select->currentIndex().row(), 2)).toString() + "'" );

        fenetreModif = new QDialog(this);
        QVBoxLayout *layoutModif = new QVBoxLayout;
        nomPersonne = new QLineEdit;
        prenomPersonne = new QLineEdit;
        identifiantPersonne = new QLineEdit;
        mdpPersonne = new QLineEdit;
        rolePersonne = new QLineEdit;
        compteurPersonne = new QSpinBox;

        while (query.next()) {
            nomPersonne->setText(query.value(0).toString());
            prenomPersonne->setText(query.value(1).toString());
            identifiantPersonne->setText(query.value(2).toString());
            mdpPersonne->setText(query.value(3).toString());
            rolePersonne->setText(query.value(4).toString());
            compteurPersonne->setValue(query.value(5).toInt());
        }

        identifiantPer = identifiantPersonne->text(); // stocke l'identifiant actuel

        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow(tr("Nom:"), nomPersonne);
        formLayout->addRow(tr("Prenom:"), prenomPersonne);
        formLayout->addRow(tr("Identifiant:"), identifiantPersonne);
        formLayout->addRow(tr("Mot de passe:"), mdpPersonne);
        formLayout->addRow(tr("Role:"), rolePersonne);
        formLayout->addRow(tr("Compteur:"), compteurPersonne);

        layoutModif->addLayout(formLayout);

        QPushButton *valideModif = new QPushButton("Valider modification");

        connect(valideModif, SIGNAL(clicked()), this, SLOT(modificationPersonne()));

        layoutModif->addWidget(valideModif);

        fenetreModif->setLayout(layoutModif);

        fenetreModif->resize(300,300);
        fenetreModif->show();


    }
    else
        QMessageBox::critical(this, "Erreur", "Merci de faire votre sélection avant de cliquer sur modifier");
}

void FenRh::modificationPersonne()
{
    QSqlQuery query;
    query.prepare("UPDATE Personne SET nom = :nom, prenom = :prenom, identifiant = :identifiant, mdp = :mdp, role = :role, compteur = :compteur WHERE identifiant = '" + identifiantPer + "'");



    query.bindValue(":nom", nomPersonne->text());
    query.bindValue(":prenom", prenomPersonne->text());
    query.bindValue(":identifiant", identifiantPersonne->text());
    query.bindValue(":mdp", mdpPersonne->text());
    query.bindValue(":role", rolePersonne->text());
    query.bindValue(":compteur", compteurPersonne->value());
    query.exec();

    fenetreModif->close();
    on_actionModifier_une_personne_triggered();
}
