#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct ST_SPARAMETER_INFO
    {
        QString fileName;
        int portNum;
        double referenceImpenance;
        int frequencyNum;
        double startFrequecny;
        double endFrequency;
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initDialog();

    void initSignal();

    void initStyleSheet();

private slots:
    void on_loadSParameterFile();

private:
    Ui::MainWindow *ui;

    std::vector<ST_SPARAMETER_INFO> m_loadSParameterFile;
};
#endif // MAINWINDOW_H
