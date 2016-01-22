#include "FenConnexion.h"
#include "ui_fenConnexion.h"
#include "FenEmploye.h"
#include "FenResponsable.h"
#include "Fenrh.h"
#include "WindowCenter.h"
#include "connectionBdd.h"

FenConnexion::FenConnexion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FenConnexion)
{
    ui->setupUi(this);

    connect(ui->motDePasseLineEdit, SIGNAL(returnPressed()), this, SLOT(on_buttonConnexion_clicked()));
}

FenConnexion::~FenConnexion()
{
    delete ui;
}

void FenConnexion::on_buttonQuitter_clicked()
{
    this->close();
}

void FenConnexion::on_buttonConnexion_clicked()
{
    QString id = ui->identifiantLineEdit->text();
    QString password = ui->motDePasseLineEdit->text();

    QString identifiant = "";
    QString role;
    QString nom;
    QString prenom;
    int compteur;
    int idPersonne;

    if(!connectionExiste)
        createConnection();

    QSqlQuery query("SELECT identifiant, mdp, role, nom, prenom, compteur, idPersonne FROM Personne");
    if (query.isActive())
    {
        while (query.next()) {

            if(id == query.value(0).toString() && password == query.value(1).toString())
            {
                identifiant = query.value(0).toString();
                role = query.value(2).toString();
                nom = query.value(3).toString();
                prenom = query.value(4).toString();
                compteur = query.value(5).toInt();
                idPersonne = query.value(6).toInt();
                break;
            }
        }
        if(identifiant.isEmpty())
             QMessageBox::critical(this, "Erreur de connexion", "Identifiant et/ou Mot de passe incorrect !");

        else
        {
            if(role == "Employe")
            {
                this->close();
                FenEmploye *fenetre = new FenEmploye(nom, prenom, compteur, idPersonne);
                setCenterOfApplication(fenetre);
                fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
                fenetre->show();
            }
            if(role == "Responsable")
            {
                this->close();
                FenResponsable *fenetre = new FenResponsable(nom, prenom, compteur, idPersonne);
                setCenterOfApplication(fenetre);
                fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
                fenetre->show();
            }
            if(role == "RH" || role == "Responsable RH")
            {
                this->close();
                FenRh *fenetre = new FenRh(nom, prenom, compteur, idPersonne, role);
                setCenterOfApplication(fenetre);
                fenetre->setWindowFlags(Qt::WindowStaysOnTopHint);
                fenetre->show();
            }
        }
    }
    else
        QMessageBox::critical(this," Erreur", "Erreur de connexion à la base de donnée");
}
