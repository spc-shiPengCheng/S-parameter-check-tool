#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "./st_struct_file.h"
#include "./st_file_io.h"
#include "./check_sparameter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initDialog();

    initStyleSheet();

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
    QString style = R"(/* 全局基础设置 */
QWidget {
    background-color: #1E1E1E;
    color: #E0E0E0;
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
    font-size: 13px;
}

/* 1. GroupBox 样式优化：圆角、内边距、现代化标题 */
QGroupBox {
    background-color: #252526;
    border: 1px solid #3E3E42;
    border-radius: 6px;
    margin-top: 12px; /* 给标题留出空间 */
    padding-top: 15px;
}

/* 3. 输入框 (ComboBox, LineEdit) */
QComboBox, QLineEdit {
    background-color: #2D2D30;
    border: 1px solid #3E3E42;
    border-radius: 4px;
    padding: 4px 8px;
    color: #E0E0E0;
    min-height: 20px;
}
QComboBox:hover, QLineEdit:hover {
    border: 1px solid #007ACC; /* 悬停时边框变蓝 */
}
QComboBox::drop-down {
    border: none;
    width: 20px;
}

/* 4. 按钮 (Browse) */
QPushButton {
    background-color: #007ACC;
    color: #FFFFFF;
    border: none;
    border-radius: 4px;
    padding: 6px 16px;
    font-weight: bold;
}
QPushButton:hover {
    background-color: #1C8ADB; /* 悬停变亮 */
}
QPushButton:pressed {
    background-color: #005A9E; /* 按下变暗 */
}
QPushButton:disabled {
    background-color: #3E3E42;
    color: #808080;
}

/* 5. TabWidget 样式 (扁平的现代标签页) */
QTabWidget::pane {
    border: 1px solid #3E3E42;
    background-color: #252526;
    border-radius: 4px;
    top: -1px; /* 与Tab无缝连接 */
}
QTabBar::tab {
    background: transparent;
    color: #A0A0A0;
    padding: 8px 16px;
    border: none;
    margin-right: 4px;
}
QTabBar::tab:selected {
    color: #FFFFFF;
    border-bottom: 2px solid #007ACC; /* 选中时底部蓝色线条 */
    font-weight: bold;
}
QTabBar::tab:hover:!selected {
    color: #E0E0E0;
}

/* 6. 滚动条美化 (适用于右侧和下方的GroupBox) */
QScrollBar:vertical {
    border: none;
    background: #1E1E1E;
    width: 10px;
    margin: 0px;
}
QScrollBar::handle:vertical {
    background: #424242;
    min-height: 20px;
    border-radius: 5px;
}
QScrollBar::handle:vertical:hover {
    background: #555555;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0px;
})";
    setStyleSheet(style);


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

    struct snp SparameterData;
    readSparameter(fileName, SparameterData);

    qDebug() << "选择的文件：" << fileName;
    // QFileInfo fileInfo(fileName);



    ui->comboBox_SParameetrfile->addItem(tr(SparameterData.fileName.toStdString().c_str(),SparameterData.fileName.toStdString().c_str()));
    ui->label_portNum->setText(QString::number(SparameterData.portNum));
    ui->label_Frequencies->setText(QString("%1 to %2, %3 points").arg(SparameterData.frequency[0]).arg(SparameterData.frequency[SparameterData.frequency.size()-1]).arg(SparameterData.frequency.size()));
    ui->label_portImpedance->setText(QString::number(SparameterData.referenceZ));
    ui->label_reciprocity->setText(isReciprocal(SparameterData.sparametrRI) ? "Yes" : "No");
    ui->label_reciprocity->setStyleSheet(R"(QLabel{
    color: black;
    background-color: #4CAF50; /* 绿色 */
    border-radius: 4px;        /* 可选：加个圆角更好看 */
})");
    ui->label_passivity->setText(isPassive(SparameterData.sparametrRI) ? "Yes" : "No");
    ui->label_causality->setText(isCausal(SparameterData.sparametrRI) ? "Yes" : "No");

    ui->tableWidget_portSelect->setRowCount(SparameterData.portNum);
    ui->tableWidget_portSelect->setColumnCount(SparameterData.portNum);
    // TODO: 在这里读取/导入 fileName
}
