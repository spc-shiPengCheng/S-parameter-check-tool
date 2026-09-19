#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./st_struct_file.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initDialog();

    void initSignal();

    void initStyleSheet();

private slots:
    void on_loadSParameterFile();

    void on_showSparameterCurve();

    void on_switchCurrentSnpFile();

private:
    Ui::MainWindow *ui;

    std::map<QString, snp>m_loadSParameterFile;
    // std::vector<snp> m_loadSParameterFile;

    QString m_currentSnpFileVariant;
};
#endif // MAINWINDOW_H
