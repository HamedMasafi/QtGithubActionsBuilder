#include "yamlhighlighter.h"

YamlHighlighter::YamlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Key highlighting
    keyFormat.setForeground(Qt::darkBlue);
    keyFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("^\\s*[\\w-]+(?=:)");
    rule.format = keyFormat;
    highlightingRules.append(rule);

    // Value highlighting
    valueFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(": .+$");
    rule.format = valueFormat;
    highlightingRules.append(rule);

    // Comment highlighting
    commentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression("#.*$");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    // List item highlighting
    listTitleFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("^\\s*-\\s+.*:");
    rule.format = listTitleFormat;
    highlightingRules.append(rule);

    listItemFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression("^\\s*-\\s+[^:]$");
    rule.format = listItemFormat;
    highlightingRules.append(rule);

    // Special characters highlighting
    specialCharFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("[\\[\\]{}]");
    rule.format = specialCharFormat;
    highlightingRules.append(rule);

    // String highlighting
    stringFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);

    // Numeric value highlighting
    numericFormat.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?\\b");
    rule.format = numericFormat;
    highlightingRules.append(rule);

    variableFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression("\\$\\{\\{\\s*\\S+\\s*\\}\\}");
    rule.format = variableFormat;
    highlightingRules.append(rule);
}

void YamlHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
