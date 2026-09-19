#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

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

    // 隐藏水平表头（列标题）
    ui->tableWidget_portSelect->horizontalHeader()->setVisible(false);
    // 隐藏垂直表头（行号）
    ui->tableWidget_portSelect->verticalHeader()->setVisible(false);

    ui->tableWidget_portSelect->setSelectionMode(QAbstractItemView::MultiSelection);
}

void MainWindow::initSignal()
{
    connect(ui->pushButton_browse, &QPushButton::clicked, this, &MainWindow::on_loadSParameterFile);

    connect(ui->tableWidget_portSelect, &QTableWidget::cellClicked, this,&MainWindow::on_showSparameterCurve);

    connect(ui->comboBox_SParameetrfile, &QComboBox::currentIndexChanged, this,&MainWindow::on_switchCurrentSnpFile);
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

    ui->tableWidget_portSelect->setStyleSheet(
        /*"QTableWidget::item { background-color: #F0F0F0; }"*/
        "QTableWidget::item:selected { background-color: #3399FF; color: white; }"
        );


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

    snp SparameterData;
    readSparameter(fileName, SparameterData);
    m_loadSParameterFile.insert(std::pair<QString, snp>(SparameterData.fileName, SparameterData));

    qDebug() << "选择的文件：" << fileName;
    // QFileInfo fileInfo(fileName);



    ui->comboBox_SParameetrfile->addItem(tr(SparameterData.fileName.toStdString().c_str(),SparameterData.fileName.toStdString().c_str()),QString(SparameterData.fileName));
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
    for(int row=0;row<SparameterData.portNum;row++)
    {
        for(int col=0;col<SparameterData.portNum;col++)
        {
            ui->tableWidget_portSelect->setItem(row,col, new QTableWidgetItem(QString("S(%1,%2)").arg(row+1).arg(col+1)));
        }
    }

    m_currentSnpFileVariant = SparameterData.fileName;
    ui->comboBox_SParameetrfile->setCurrentIndex(ui->comboBox_SParameetrfile->count()-1);
    ui->tableWidget_portSelect->clearSelection();
    ui->customplot->clearGraphs();

    // TODO: 在这里读取/导入 fileName
}

void MainWindow::on_showSparameterCurve()
{
    ui->customplot->clearGraphs();
    QList<QTableWidgetItem*> selectItems = ui->tableWidget_portSelect->selectedItems();


    // 假设你已经在界面中放置了一个 QCustomPlot 控件，并命名为 customPlot
    int graphCount = 0;
    for (const QTableWidgetItem* item : selectItems) {

        // 2. 添加一个曲线图 (Graph)
        ui->customplot->addGraph();

        QVector<double> sparameterFrequecny;
        QVector<double> sparameterMag;

        for(int i=0;i<(int)m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI.size();i++)
        {
            sparameterMag.push_back(std::abs(m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI[i][item->row()][item->column()]));
            sparameterFrequecny.push_back(m_loadSParameterFile[m_currentSnpFileVariant].frequency[i]);
        }
        // 3. 为曲线图设置数据
        ui->customplot->graph(graphCount)->setData(sparameterFrequecny, sparameterMag);

        // 4. 为曲线图命名（会显示在图例中）
        ui->customplot->graph(graphCount)->setName("二次函数");

        // 5. 设置坐标轴标签
        ui->customplot->xAxis->setLabel("Frequency");
        ui->customplot->yAxis->setLabel("Mag");

        graphCount++;
    }




    // 6. 设置坐标轴显示范围
    // ui->customplot->xAxis->setRange(-1, 1);
    // ui->customplot->yAxis->setRange(0, 1);
     ui->customplot->rescaleAxes();

    // 7. 显示图例
    // ui->customplot->legend->setVisible(true);

    // 8. 刷新图表，使所有设置生效
    ui->customplot->replot();
}

void MainWindow::on_switchCurrentSnpFile()
{
    m_currentSnpFileVariant = ui->comboBox_SParameetrfile->currentData().toString();

    snp SparameterData = m_loadSParameterFile[m_currentSnpFileVariant];
    int currentSnpIndex = ui->comboBox_SParameetrfile->findData(m_currentSnpFileVariant);
    if(currentSnpIndex == -1)
    {
        currentSnpIndex = 0;
    }
    ui->comboBox_SParameetrfile->setCurrentIndex(currentSnpIndex);
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
    for(int row=0;row<SparameterData.portNum;row++)
    {
        for(int col=0;col<SparameterData.portNum;col++)
        {
            ui->tableWidget_portSelect->setItem(row,col, new QTableWidgetItem(QString("S(%1,%2)").arg(row+1).arg(col+1)));
        }
    }

    ui->tableWidget_portSelect->clearSelection();
    ui->customplot->clearGraphs();
}
