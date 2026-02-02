#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItemModel>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // تحميل أي جداول موجودة على الهارد
    db.loadAllTables("data");

    // ربط زر Execute بالفنكشن
    connect(ui->executeBtn, &QPushButton::clicked,
            this, &MainWindow::onExecuteClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ================================
// LINK BETWEEN GUI <-> DB ENGINE
// ================================
void MainWindow::onExecuteClicked()
{
    // 1️⃣ قراءة SQL من TextEdit
    QString sql = ui->sqlInput->toPlainText().trimmed();
    if (sql.isEmpty())
        return;

    // 2️⃣ Reset أي نتائج قديمة
    ui->outputBox->clear();
    ui->tableView->setModel(nullptr);

    // 3️⃣ تنفيذ الكويري
    QString result =
        QString::fromStdString(
            db.executeSQLWithResult(sql.toStdString())
            );

    // 🔥 الحل المهم (يحافظ على startsWith)
    result = result.trimmed();

    if (result.isEmpty())
        return;

    // ================================
    // CASE 1: RESULT IS A TABLE
    // ================================
    if (result.startsWith("__TABLE__"))
    {
        // إزالة الماركر
        result.remove("__TABLE__");
        result = result.trimmed();

        // تقسيم السطور
        QStringList lines =
            result.split("\n", Qt::SkipEmptyParts);

        if (lines.isEmpty())
            return;

        // إنشاء Model
        QStandardItemModel *model =
            new QStandardItemModel(this);

        // أول سطر = أسماء الأعمدة
        QStringList headers =
            lines.takeFirst().split(" ", Qt::SkipEmptyParts);

        model->setHorizontalHeaderLabels(headers);

        // باقي السطور = بيانات
        for (int row = 0; row < lines.size(); row++)
        {
            QStringList cols =
                lines[row].split(" ", Qt::SkipEmptyParts);

            for (int col = 0; col < cols.size(); col++)
            {
                model->setItem(
                    row,
                    col,
                    new QStandardItem(cols[col])
                    );
            }
        }

        // ربط الجدول بالـ View
        ui->tableView->setModel(model);

        // تحسين شكل الجدول
        ui->tableView->horizontalHeader()->setVisible(true);
        ui->tableView->horizontalHeader()
            ->setSectionResizeMode(QHeaderView::Stretch);
    }
    // ================================
    // CASE 2: NORMAL TEXT
    // ================================
    else
    {
        ui->outputBox->append(result);
    }
}
