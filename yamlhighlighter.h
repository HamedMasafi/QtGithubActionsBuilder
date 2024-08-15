#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class YamlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit YamlHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keyFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat listItemFormat;
    QTextCharFormat listTitleFormat;
    QTextCharFormat specialCharFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numericFormat;
    QTextCharFormat variableFormat;
};
