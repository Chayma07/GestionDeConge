#ifndef FENRH_H
#define FENRH_H

#include <QSqlQueryModel>
#include <QMainWindow>
#include <QDialog>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableView>
#include <QLineEdit>
#include <QSpinBox>

namespace Ui {
class FenRh;
}

class FenRh : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenRh(QString nom, QString prenom, int compteur, int idPersonne, QString statutUtilisateur, QWidget *parent = 0);
   ~FenRh();

private slots:
    void on_actionQuitter_triggered();

    void on_actionFaire_une_demande_triggered();

    void on_calendrierDebut_clicked();

    void selectionDate();

    void on_actionConsulter_demande_s_triggered();

    void on_calendrierFin_clicked();

    void on_actionDeconnection_triggered();

    void on_buttonValider_clicked();

    void on_buttonSupprimer_clicked();

    void on_buttonValideDemande_clicked();

    void on_buttonRefuserDemande_clicked();

    void on_actionAjouter_une_personne_triggered();

    void on_pushButton_clicked();

    void on_actionModifier_une_personne_triggered();

    void modificationPersonne();

    void on_pushButton_2_clicked();



private:

    bool validationFormulaire();

    bool validationDemande();

    void creationQdialogCalendrier();

    QDateTimeEdit *dateEtHeureEdit;

    Ui::FenRh *ui;

    QDialog *fenetreCalendrier;
    QDialog *fenetreModif;

    QCalendarWidget *calendrier;
    int m_idPersonne;
    QString m_statutUtilisateur;

    QString identifiantPer;

    QLineEdit *nomPersonne;
    QLineEdit *prenomPersonne;
    QLineEdit *identifiantPersonne;
    QLineEdit *mdpPersonne;
    QLineEdit *rolePersonne;
    QSpinBox *compteurPersonne;
};

#endif // FENRH_H
