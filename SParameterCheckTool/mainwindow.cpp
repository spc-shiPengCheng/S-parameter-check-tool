#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initDialog();

    initSignal();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initDialog()
{
    setWindowTitle(tr("S Parameter Check Tool","散射参数检测工具"));
}

void MainWindow::initSignal()
{
    connect(ui->pushButton_browse, &QPushButton::clicked, this, &MainWindow::on_loadSParameterFile);
}

void MainWindow::initStyleSheet()
{


}


void MainWindow::on_loadSParameterFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,                                   // 父窗口
        tr("导入文件"),                          // 对话框标题
        QString(),                              // 默认目录，可写 QDir::homePath()
        tr("SNP File (*.s*p)")
        );

    if (fileName.isEmpty()) {
        return; // 用户取消
    }

    qDebug() << "选择的文件：" << fileName;
    QFileInfo fileInfo(fileName);

    ST_SPARAMETER_INFO loadSparameterInfo;
    loadSparameterInfo.fileName = fileInfo.fileName();
    QString fileSuffix = fileInfo.suffix();
    QString portNumStr = fileSuffix.sliced(1, fileSuffix.size() - 2);
    loadSparameterInfo.portNum = portNumStr.toInt();

    ui->label_portNum->setText(QString::number(loadSparameterInfo.portNum));
    // TODO: 在这里读取/导入 fileName
}
