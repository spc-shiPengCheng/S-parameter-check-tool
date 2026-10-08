/*
 * Author: spc
 * Description: Main ui file.
 */
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

    ui->comboBox_parameterType->addItem("S","S");
    ui->comboBox_parameterType->addItem("Y","Y");
    ui->comboBox_parameterType->addItem("Z","Z");

    ui->comboBox_dataType->addItem("Amplitude/Phase","MAG");
    ui->comboBox_dataType->addItem("Real/Imag","RI");
    ui->comboBox_dataType->addItem("Db/Phase","DB");

    ui->customplot->setContextMenuPolicy(Qt::CustomContextMenu);
    // 允许拖拽和滚轮缩放
    ui->customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 允许水平和垂直方向拖拽
    ui->customplot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);

    // 允许水平和垂直方向滚轮缩放
    ui->customplot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    m_vLine = new QCPItemLine(ui->customplot);
    m_vLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    m_vLine->setVisible(false);

    m_hLine = new QCPItemLine(ui->customplot);
    m_hLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    m_hLine->setVisible(false);

    m_cursorText = new QCPItemText(ui->customplot);
    m_cursorText->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    m_cursorText->setPadding(QMargins(5, 5, 5, 5));
    m_cursorText->setBrush(QBrush(QColor(255, 255, 220, 220)));
    m_cursorText->setPen(QPen(Qt::gray));
    m_cursorText->setVisible(false);
}

void MainWindow::initSignal()
{
    connect(ui->pushButton_browse, &QPushButton::clicked, this, &MainWindow::on_loadSParameterFile);

    connect(ui->tableWidget_portSelect, &QTableWidget::cellClicked, this,&MainWindow::on_showSparameterCurve);

    connect(ui->comboBox_SParameetrfile, &QComboBox::currentIndexChanged, this,&MainWindow::on_switchCurrentSnpFile);

    connect(ui->comboBox_parameterType, &QComboBox::currentIndexChanged, this,&MainWindow::on_showSparameterCurve);

    connect(ui->comboBox_dataType, &QComboBox::currentIndexChanged, this,&MainWindow::on_showSparameterCurve);

    connect(ui->tabWidget_character, &QTabWidget::currentChanged, this, &MainWindow::on_switchCharacterTab);

    connect(ui->pushButton_plotSingularCurve, &QPushButton::clicked, this, &MainWindow::on_plotSingularCurve);

    connect(ui->pushButton_passivityEnforce, &QPushButton::clicked, this, &MainWindow::on_passivityEnforce);

    connect(ui->customplot, &QCustomPlot::customContextMenuRequested, this, &MainWindow::on_customPlotContextMenu);

    connect(ui->customplot, &QCustomPlot::mouseMove, this, &MainWindow::on_mouseMove);
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

    int graphColorCount = selectItems.size();
    if(ui->comboBox_dataType->currentData().toString() == "RI")
    {
        graphColorCount = graphColorCount*2;
    }




    std::vector<std::vector<std::vector<std::complex<double>>>> currentParameter;
    if(ui->comboBox_parameterType->currentData().toString() == "S")
    {
        currentParameter = m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI;
    }
    else if(ui->comboBox_parameterType->currentData().toString() == "Y")
    {
        currentParameter = m_loadSParameterFile[m_currentSnpFileVariant].yparametrRI;
    }
    else if(ui->comboBox_parameterType->currentData().toString() == "Z")
    {
        currentParameter = m_loadSParameterFile[m_currentSnpFileVariant].zparametrRI;
    }

    // 假设你已经在界面中放置了一个 QCustomPlot 控件，并命名为 customPlot
    int graphCount = 0;
    for (const QTableWidgetItem* item : selectItems) {

        // 2. 添加一个曲线图 (Graph)
        QCPGraph* graph = ui->customplot->addGraph();
        // 自动生成颜色：色相均匀分布，饱和度和亮度固定
        int hue = ((graphCount+1) * 360 / graphColorCount) % 360;
        QColor color = QColor::fromHsv(hue, 200, 255);
        graph->setPen(QPen(color, 2));   // 线宽 2

        if(ui->comboBox_dataType->currentData().toString() == "MAG")
        {
            QVector<double> sparameterFrequecny;
            QVector<double> sparameterMag;

            for(int i=0;i<(int)currentParameter.size();i++)
            {
                sparameterMag.push_back(std::abs(currentParameter[i][item->row()][item->column()]));
                sparameterFrequecny.push_back(m_loadSParameterFile[m_currentSnpFileVariant].frequency[i]);
            }
            // 3. 为曲线图设置数据
            ui->customplot->graph(graphCount)->setData(sparameterFrequecny, sparameterMag);

            // 4. 为曲线图命名（会显示在图例中）
            ui->customplot->graph(graphCount)->setName(QString("S(%1,%2)").arg(item->row()+1).arg(item->column()+1));

            // 5. 设置坐标轴标签
            ui->customplot->xAxis->setLabel("Frequency");
            ui->customplot->yAxis->setLabel("Mag");
        }
        else if(ui->comboBox_dataType->currentData().toString() == "RI")
        {
            QVector<double> sparameterFrequecny;
            QVector<double> sparameterReal;
            QVector<double> sparameterImag;

            for(int i=0;i<(int)currentParameter.size();i++)
            {
                sparameterReal.push_back(currentParameter[i][item->row()][item->column()].real());
                sparameterImag.push_back(currentParameter[i][item->row()][item->column()].imag());
                sparameterFrequecny.push_back(m_loadSParameterFile[m_currentSnpFileVariant].frequency[i]);
            }
            // 3. 为曲线图设置数据
            ui->customplot->graph(graphCount)->setData(sparameterFrequecny, sparameterReal);

            // 4. 为曲线图命名（会显示在图例中）
            ui->customplot->graph(graphCount)->setName(QString("S(%1,%2)-real").arg(item->row()+1).arg(item->column()+1));

            graphCount++;
            // 2. 添加一个曲线图 (Graph)
            QCPGraph* graph = ui->customplot->addGraph();
            // 自动生成颜色：色相均匀分布，饱和度和亮度固定
            int hue = ((graphCount+1) * 360 / graphColorCount) % 360;
            QColor color = QColor::fromHsv(hue, 200, 255);
            graph->setPen(QPen(color, 2));   // 线宽 2

            // 3. 为曲线图设置数据
            ui->customplot->graph(graphCount)->setData(sparameterFrequecny, sparameterImag);

            // 4. 为曲线图命名（会显示在图例中）
            ui->customplot->graph(graphCount)->setName(QString("S(%1,%2)-imag").arg(item->row()+1).arg(item->column()+1));

            // 5. 设置坐标轴标签
            ui->customplot->xAxis->setLabel("Frequency");
            ui->customplot->yAxis->setLabel("Real/Imag");
        }
        else if(ui->comboBox_dataType->currentData().toString() == "DB")
        {
            QVector<double> sparameterFrequecny;
            QVector<double> sparameterDB;

            for(int i=0;i<(int)currentParameter.size();i++)
            {
                sparameterDB.push_back(20*std::log(abs(currentParameter[i][item->row()][item->column()])));
                sparameterFrequecny.push_back(m_loadSParameterFile[m_currentSnpFileVariant].frequency[i]);
            }
            // 3. 为曲线图设置数据
            ui->customplot->graph(graphCount)->setData(sparameterFrequecny, sparameterDB);

            // 4. 为曲线图命名（会显示在图例中）
            ui->customplot->graph(graphCount)->setName(QString("S(%1,%2)-db").arg(item->row()+1).arg(item->column()+1));

            // 5. 设置坐标轴标签
            ui->customplot->xAxis->setLabel("Frequency");
            ui->customplot->yAxis->setLabel("DB");
        }

        graphCount++;
    }




    // 6. 设置坐标轴显示范围
    // ui->customplot->xAxis->setRange(-1, 1);
    // ui->customplot->yAxis->setRange(0, 1);
     ui->customplot->rescaleAxes();

    // 7. 显示图例
    ui->customplot->legend->setVisible(true);

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

void MainWindow::on_switchCharacterTab()
{
    QWidget* currentPage = ui->tabWidget_character->currentWidget();

    if(currentPage == ui->tab_passivity)
    {

    }

}

void MainWindow::on_plotSingularCurve()
{
    ui->customplot->clearGraphs();


    // 2. 添加一个曲线图 (Graph)
    QCPGraph* graph = ui->customplot->addGraph();
    // 自动生成颜色：色相均匀分布，饱和度和亮度固定
    int hue = (1 * 360 / 1) % 360;
    QColor color = QColor::fromHsv(hue, 200, 255);
    graph->setPen(QPen(color, 2));   // 线宽 2

    QVector<double> sparameterFrequecny;
    QVector<double> sparameterSingularValue;

    std::vector<double> singularValue = getSingular(m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI);
    for(int i=0;i<(int)m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI.size();i++)
    {
        sparameterSingularValue.push_back(singularValue[i]);
        sparameterFrequecny.push_back(m_loadSParameterFile[m_currentSnpFileVariant].frequency[i]);
    }
    // 3. 为曲线图设置数据
    ui->customplot->graph(0)->setData(sparameterFrequecny, sparameterSingularValue);

    // 4. 为曲线图命名（会显示在图例中）
    ui->customplot->graph(0)->setName(QString("Singular Value"));

    // 5. 设置坐标轴标签
    ui->customplot->xAxis->setLabel("Frequency");
    ui->customplot->yAxis->setLabel("Singular Value");

    // 6. 设置坐标轴显示范围
    // ui->customplot->xAxis->setRange(-1, 1);
    // ui->customplot->yAxis->setRange(0, 1);
    // ui->customplot->rescaleAxes();
    auto [min_it, max_it] = std::minmax_element(singularValue.begin(), singularValue.end());
    ui->customplot->yAxis->setRange(*min_it*0.8, *max_it*1.2); // 根据实际需求设定一个合理的范围

    // 7. 显示图例
    ui->customplot->legend->setVisible(true);

    // 2. 让 Y 轴刻度显示更多小数位，方便观察真实数值
    // ui->customplot->yAxis->setNumberFormat("f");
    // ui->customplot->yAxis->setNumberPrecision(8); // 显示 8 位小数
    // ui->customplot->rescaleAxes();
    ui->customplot->xAxis->rescale();

    // 8. 刷新图表，使所有设置生效
    ui->customplot->replot();
}

void MainWindow::on_passivityEnforce()
{
    std::vector<std::vector<std::vector<std::complex<double>>>> passiveSparameter;
    passicityEnforce(m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI, passiveSparameter);
    m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI = passiveSparameter;

    std::vector<std::vector<std::vector<std::complex<double>>>> yparameter;
    s2y(m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI,yparameter,m_loadSParameterFile[m_currentSnpFileVariant].referenceZ);
    m_loadSParameterFile[m_currentSnpFileVariant].yparametrRI = yparameter;

    std::vector<std::vector<std::vector<std::complex<double>>>> zparameter;
    s2z(m_loadSParameterFile[m_currentSnpFileVariant].sparametrRI,zparameter,m_loadSParameterFile[m_currentSnpFileVariant].referenceZ);
    m_loadSParameterFile[m_currentSnpFileVariant].zparametrRI = zparameter;
}

void MainWindow::on_customPlotContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    QMenu *exportMenu = menu.addMenu(tr("导出图片"));

    QAction *pngAction     = exportMenu->addAction(tr("PNG (标准)"));
    QAction *pngHdAction   = exportMenu->addAction(tr("PNG (高清 2x)"));
    QAction *pngTransAction= exportMenu->addAction(tr("PNG (透明背景)"));
    exportMenu->addSeparator();
    QAction *jpgAction     = exportMenu->addAction(tr("JPG"));
    QAction *bmpAction     = exportMenu->addAction(tr("BMP"));
    exportMenu->addSeparator();
    QAction *pdfAction     = exportMenu->addAction(tr("PDF (矢量)"));

    QAction *resetAction     = menu.addAction(tr("重置"));

    QAction *selected = menu.exec(ui->customplot->mapToGlobal(pos));
    if (!selected) return;

    if (selected == resetAction) {
        ui->customplot->rescaleAxes();
        ui->customplot->replot();
        return;
    }

    QString filter;
    QString defaultPath = QDir::homePath() + "/plot";

    if (selected == pngAction || selected == pngHdAction || selected == pngTransAction) {
        filter = "PNG 图片 (*.png)";
        defaultPath += ".png";
    } else if (selected == jpgAction) {
        filter = "JPG 图片 (*.jpg)";
        defaultPath += ".jpg";
    } else if (selected == bmpAction) {
        filter = "BMP 图片 (*.bmp)";
        defaultPath += ".bmp";
    } else if (selected == pdfAction) {
        filter = "PDF 文件 (*.pdf)";
        defaultPath += ".pdf";
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("导出图片"), defaultPath, filter);
    if (fileName.isEmpty()) return;

    bool ok = false;

    if (selected == pngAction) {
        ok = ui->customplot->savePng(fileName);
    } else if (selected == pngHdAction) {
        ok = ui->customplot->savePng(fileName, 0, 0, 2.0);  // 2 倍分辨率
    } else if (selected == pngTransAction) {
        // 临时把背景设为透明
        QBrush oldBrush = ui->customplot->background();
        ui->customplot->setBackground(QBrush(Qt::NoBrush));
        ok = ui->customplot->savePng(fileName);
        ui->customplot->setBackground(oldBrush);  // 恢复
    } else if (selected == jpgAction) {
        ok = ui->customplot->saveJpg(fileName, 0, 0, 1.0, 90);
    } else if (selected == bmpAction) {
        ok = ui->customplot->saveBmp(fileName);
    } else if (selected == pdfAction) {
        ok = ui->customplot->savePdf(fileName);
    }

    if (!ok) {
        QMessageBox::warning(this, tr("导出失败"), tr("无法保存文件。"));
    } else {
        statusBar()->showMessage(tr("已导出: %1").arg(fileName), 3000);
    }
}

void MainWindow::on_mouseMove(QMouseEvent *event)
{
    double x = ui->customplot->xAxis->pixelToCoord(event->pos().x());
    double y = ui->customplot->yAxis->pixelToCoord(event->pos().y());

    m_vLine->start->setCoords(x, ui->customplot->yAxis->range().lower);
    m_vLine->end->setCoords(x, ui->customplot->yAxis->range().upper);
    m_vLine->setVisible(true);

    m_hLine->start->setCoords(ui->customplot->xAxis->range().lower, y);
    m_hLine->end->setCoords(ui->customplot->xAxis->range().upper, y);
    m_hLine->setVisible(true);

    m_cursorText->position->setCoords(x, y);
    m_cursorText->setText(QString("x = %1\ny = %2")
                              .arg(x, 0, 'f', 4)
                              .arg(y, 0, 'f', 4));
    m_cursorText->setVisible(true);

    ui->customplot->replot(QCustomPlot::rpQueuedReplot);
}
