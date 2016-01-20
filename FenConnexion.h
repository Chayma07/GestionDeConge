#ifndef FENCONNEXION_H
#define FENCONNEXION_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class FenConnexion;
}

class FenConnexion : public QWidget
{
    Q_OBJECT

public:
    explicit FenConnexion(QWidget *parent = 0);
    ~FenConnexion();

private slots:
    void on_buttonQuitter_clicked();

    void on_buttonConnexion_clicked();

private:
    Ui::FenConnexion *ui;
};

#endif // FENCONNEXION_H
