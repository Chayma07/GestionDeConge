#ifndef FENEMPLOYE_H
#define FENEMPLOYE_H

#include <QSqlQueryModel>
#include <QMainWindow>
#include <QDialog>
#include <QCalendarWidget>
#include <QDateTimeEdit>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableView>
#include <QSortFilterProxyModel>

namespace Ui {
class FenEmploye;
}

class FenEmploye : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenEmploye(QString nom, QString prenom, int compteur, int idPersonne, QWidget *parent = 0);
    ~FenEmploye();

private slots:
    void on_actionQuitter_triggered();

    void on_actionFaire_une_demande_triggered();

    void on_calendrierDebut_clicked();

    void selectionDate();

    void on_actionConsulter_demande_s_triggered();

    void on_calendrierFin_clicked();

    void on_actionDeconnection_triggered();

    void on_buttonValider_clicked();



private:

    void creationQdialogCalendrier();
    bool validationDemande();

    QDateTimeEdit *dateEtHeureEdit;

    Ui::FenEmploye *ui;

    QDialog *fenetreCalendrier;

    QCalendarWidget *calendrier;

    int m_idPersonne;
};

#endif // FENEMPLOYE_H
