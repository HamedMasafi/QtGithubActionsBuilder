#include "mainwindow.h"

#include "templateengine.h"
#include "yamlhighlighter.h"

#include <QFileDialog>
#include <QSettings>

QStringList generateFromCheckBoxes(QList<QPair<QCheckBox *, QString>> list)
{
    QStringList ret;
    for (auto i : list)
        if (i.first->isChecked())
            ret << i.second;
    return ret;
}

QString generateFromCheckBoxes(QList<QPair<QCheckBox *, QString>> list, const QString &sep)
{
    return generateFromCheckBoxes(list).join(sep);
}

QStringList getFromTextEdit(QPlainTextEdit *textEdit, const QString &defaultValue = "*")
{
    if (textEdit->toPlainText().trimmed().isEmpty())
        return {defaultValue};
    else
        return textEdit->toPlainText().split('\n');
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    new YamlHighlighter(textBrowserBuildOutput->document());

    QSettings s;
    if (s.contains("splitter"))
        splitter->restoreState(s.value("splitter").toByteArray());
}

MainWindow::~MainWindow()
{
    QSettings s;
    s.setValue("splitter", splitter->saveState());
}

void MainWindow::on_pushButtonGenerate_clicked()
{
    auto osList = generateFromCheckBoxes(
        {{checkBoxLinux, "ubuntu-latest"},
         {checkBoxWindows, "windows-latest"},
         {checkBoxMacos, "macos-latest"}},
        ", ");

    TemplateEngine engine{"/doc/dev/github/QtGithubActionsBuilder/build-template.yml"};
    engine.setVariable("name", lineEditName->text());
    engine.setVariable("qt_version", comboBoxQtVersion->currentText());
    engine.setVariable("os_list", osList);
    engine.setVariable("binary_name", lineEditBinaryName->text());

    engine.setCondition("on_push", checkBoxOnPush->isChecked());
    engine.setCondition("on_pull_request", checkBoxOnPullRequest->isChecked());
    engine.setCondition("fail_fast", checkBoxFailFast->isChecked());
    engine.setCondition("windows", checkBoxWindows->isChecked());
    engine.setCondition("linux", checkBoxLinux->isChecked());
    engine.setCondition("macos", checkBoxMacos->isChecked());
    engine.setCondition("upload", checkBoxUpload->isChecked());

    engine.setList("branches", getFromTextEdit(plainTextEditPushBranches));
    engine.setList("tags", getFromTextEdit(plainTextEditPushTags));

    textBrowserBuildOutput->setPlainText(engine.render());

    setWindowModified(true);
}

void MainWindow::on_pushButtonSave_clicked()
{
    QFile f{windowFilePath()};
    if (!f.open(QIODevice::WriteOnly))
        return;
    f.write(textBrowserBuildOutput->toPlainText().toUtf8());
    f.close();

    setWindowModified(false);
}

void MainWindow::on_pushButtonSaveAs_clicked()
{
    auto fileName
        = QFileDialog::getSaveFileName(this, "Save as", {}, "Yml files (*.yml);; All files (*)");

    if (!fileName.isEmpty()) {
        QFile f{fileName};
        if (!f.open(QIODevice::WriteOnly))
            return;
        f.write(textBrowserBuildOutput->toPlainText().toUtf8());
        f.close();

        setWindowFilePath(fileName);
        setWindowModified(false);
        pushButtonSave->setEnabled(true);
    }
}
