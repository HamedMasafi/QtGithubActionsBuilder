#ifndef TEMPLATEENGINE_H
#define TEMPLATEENGINE_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QStack>

class TemplateEngine
{
public:
    TemplateEngine(const QString &templateFilePath);

    void setVariable(const QString &key, const QString &value);
    void setCondition(const QString &key, bool value);
    void setList(const QString &key, const QStringList &value);
    QString render();

private:
    struct LoopData {
        int startLine;
        QString variableName;
        QString currentValue;
        QStringList values;

        bool takeValue();
    };

    QString processTemplate(const QString &content);
    bool processIfStatement(const QString &line, QStack<bool> &conditionStack);
    bool processForLoop(const QString &line, int &lineIndex, const QStringList &lines);
    bool processEndFor(const QString &line, int &lineIndex);
    QString replacePlaceholders(const QString &line);
    QString replaceVariable(const QString &line,
                            const QString &name,
                            const QString &value,
                            bool *ok = nullptr) const;

    QString m_templateFilePath;
    QStack<LoopData*> m_loopStack;
    QMap<QString, QString> m_variables;
    QMap<QString, bool> m_conditions;
    QMap<QString, QStringList> m_lists;
};

#endif // TEMPLATEENGINE_H
