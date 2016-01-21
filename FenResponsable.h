#ifndef FENRESPONSABLE_H
#define FENRESPONSABLE_H

#include <QSqlQueryModel>
#include <QMainWindow>
#include <QDialog>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTableView>
#include <QComboBox>
#include <QLineEdit>
#include <QSortFilterProxyModel>

namespace Ui {
class FenResponsable;
}

class FenResponsable : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenResponsable(QString nom, QString prenom, int compteur, int idPersonne, QWidget *parent = 0);
    ~FenResponsable();

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

    void on_toolButton_clicked();

    void valideFiltre_clicked();

    void choixBoxFiltre(QString choix);

    void on_pushButton_clicked();

private:

    bool validationDemande();

    void creationQdialogCalendrier();

    QSortFilterProxyModel *proxyModel;

    QDateTimeEdit *dateEtHeureEdit;

    Ui::FenResponsable *ui;

    QDialog *fenetreCalendrier;
    QDialog *fenetreFiltre;

    QCalendarWidget *calendrier;
    int m_idPersonne;


    QLineEdit *filtreNom;
    QLineEdit *filtrePrenom;
    QDateTimeEdit *filtreDDebut;
    QDateTimeEdit *filtreDFin;
    QComboBox *filtreStatut;
    QComboBox *choixFiltre;
};

#endif // FENRESPONSABLE_H
