#include "templateengine.h"
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

TemplateEngine::TemplateEngine(const QString &templateFilePath)
    : m_templateFilePath(templateFilePath)
{}

void TemplateEngine::setVariable(const QString &key, const QString &value)
{
    m_variables[key] = value;
}

void TemplateEngine::setCondition(const QString &key, bool value)
{
    m_conditions[key] = value;
}

void TemplateEngine::setList(const QString &key, const QStringList &value)
{
    if (value.size())
        m_lists[key] = value;
}

QString TemplateEngine::render()
{
    QFile file(m_templateFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "Error: Unable to open template file";
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return processTemplate(content);
}

QString TemplateEngine::processTemplate(const QString &content)
{
    QStringList lines = content.split("\n");
    QStringList outputLines;
    QStack<bool> conditionStack;
    conditionStack.push(true);

    int i = 0;
    while (i < lines.size()) {
        QString line = lines[i];

        if (processIfStatement(line, conditionStack)) {
            ++i;
            continue;
        }

        if (processForLoop(line, i, lines)) {
            continue;
        }

        if (processEndFor(line, i)) {
            continue;
        }

        if (conditionStack.top() && m_loopStack.isEmpty()) {
            outputLines.append(replacePlaceholders(line));
        } else if (conditionStack.top() && !m_loopStack.isEmpty()) {
            auto loopData = m_loopStack.top();
            if (!loopData->currentValue.isEmpty())
                outputLines.append(
                    replaceVariable(line, loopData->variableName, loopData->currentValue));
        }

        ++i;
    }

    return outputLines.join("\n");
}

bool TemplateEngine::processIfStatement(const QString &line, QStack<bool> &conditionStack)
{
    static QRegularExpression ifRegex(R"(\{%\s*if\s+(\w+)\s*%\})");
    static QRegularExpression endifRegex(R"(\{%\s*endif\s*%\})");

    auto ifMatch = ifRegex.match(line);
    if (ifMatch.hasMatch()) {
        QString condition = ifMatch.captured(1);
        bool conditionMet = m_conditions.value(condition, false);
        conditionStack.push(conditionStack.top() && conditionMet);
        return true;
    }

    if (endifRegex.match(line).hasMatch()) {
        if (!conditionStack.isEmpty()) {
            conditionStack.pop();
        }
        return true;
    }

    return false;
}

bool TemplateEngine::processForLoop(const QString &line, int &lineIndex, const QStringList &lines)
{
    static QRegularExpression forRegex(R"(\{%\s*for\s+(\w+)\s+in\s+(\w+)\s*%\})");

    auto forMatch = forRegex.match(line);
    if (forMatch.hasMatch()) {
        QString itemName = forMatch.captured(1);
        QString listName = forMatch.captured(2);

        auto loopData = new LoopData;
        loopData->variableName = itemName;
        loopData->startLine = lineIndex;
        loopData->values = m_lists.value(listName, {});
        loopData->takeValue();
        m_loopStack.push(loopData);

        ++lineIndex;
        return true;
    }

    return false;
}

bool TemplateEngine::processEndFor(const QString &line, int &lineIndex)
{
    static QRegularExpression endForRegex(R"(\{%\s*endfor\s*%\})");

    if (endForRegex.match(line).hasMatch()) {
        if (!m_loopStack.isEmpty()) {
            auto loopData = m_loopStack.top();
            QString listName;

            if (loopData->values.size()) {
                loopData->takeValue();
                lineIndex = loopData->startLine + 1;
            } else {
                m_loopStack.pop();
                ++lineIndex;
            }
        }
        return true;
    }

    return false;
}

QString TemplateEngine::replacePlaceholders(const QString &line)
{
    QString result = line;
    QRegularExpression variableRegex(R"(\{%\s*(\w+)\s*%\})");
    QRegularExpressionMatchIterator i = variableRegex.globalMatch(result);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString placeholder = match.captured(1);
        QString value = m_variables.value(placeholder, "");
        result.replace(match.captured(0), value);
    }

    return result;
}

QString TemplateEngine::replaceVariable(
    const QString &line, const QString &name, const QString &value, bool *ok) const
{
    QString result = line;
    QRegularExpression variableRegex(R"(\{\{\s*)" + name + R"(\s*\}\})");
    QRegularExpressionMatchIterator i = variableRegex.globalMatch(result);

    if (ok)
        *ok = false;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        result.replace(match.captured(0), value);
        if (ok)
            *ok = true;
    }

    return result;
}

bool TemplateEngine::LoopData::takeValue()
{
    if (values.size()) {
        currentValue = values.takeFirst();
        return true;
    } else {
        return false;
    }
}
