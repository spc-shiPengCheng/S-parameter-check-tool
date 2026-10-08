/*
 * Author: spc
 * Description: Main ui file.
 */

#pragma once

#include <QMainWindow>
#include "qcustomplot.h"
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

    void on_switchCharacterTab();

    void on_plotSingularCurve();

    void on_passivityEnforce();

    void on_customPlotContextMenu(const QPoint &pos);

    void on_mouseMove(QMouseEvent *event);

private:
    Ui::MainWindow *ui;

    std::map<QString, snp>m_loadSParameterFile;

    QString m_currentSnpFileVariant;

    QCPItemLine *m_vLine;
    QCPItemLine *m_hLine;
    QCPItemText *m_cursorText;
};

