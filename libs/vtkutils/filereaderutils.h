#ifndef FILEREADERUTILS_H
#define FILEREADERUTILS_H

#include <QObject>
#include <QRunnable>
#include <QMap>

#include <vtkDataObject.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkSTLReader.h>
#include <vtkOBJReader.h>
#include <vtkLSDynaReader.h>
#include <vtkFLUENTReader.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPLYReader.h>
#include <vtkPDBReader.h>

#include "vtkutils_global.h"
#include "vtkutils.h"

class vtkDataArray;

namespace VtkUtils
{

template <class T, class P = vtkDataObject>
class ReaderTempl
{
public:
//    virtual void run() = 0;
    virtual P* dataObject() const
    {
        if (m_reader)
            return m_reader->GetOutput();
        return nullptr;
    }

    T* reader() const { return m_reader; }

protected:
    T* m_reader = nullptr;
    P* m_dataObject = nullptr;
};

class VTKUTILS_EXPORT AbstractFileReader : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit AbstractFileReader(QObject* parent = nullptr);

    void setFileName(const QString& file);
    QString fileName() const;

    void setTitle(const QString& title);
    QString title() const;

signals:
    void finished();

protected:
    QString m_fileName;
    QString m_title;
};

class VTKUTILS_EXPORT VtkFileReader : public AbstractFileReader, public ReaderTempl<vtkGenericDataObjectReader>
{
    Q_OBJECT
public:
    explicit VtkFileReader(QObject* parent = nullptr);

    void run();
};

class VTKUTILS_EXPORT StlFileReader : public AbstractFileReader, public ReaderTempl<vtkSTLReader>
{
    Q_OBJECT
public:
    explicit StlFileReader(QObject* parent = nullptr);

    void run();

};

class VTKUTILS_EXPORT ObjFileReader : public AbstractFileReader, public ReaderTempl<vtkOBJReader>
{
    Q_OBJECT
public:
    explicit ObjFileReader(QObject* parent = nullptr);

    void run();
};

class VTKUTILS_EXPORT DynaFileReader : public AbstractFileReader, public ReaderTempl<vtkLSDynaReader, vtkMultiBlockDataSet>
{
    Q_OBJECT
public:
    explicit DynaFileReader(QObject* parent = nullptr);

    void run();
};

class VTKUTILS_EXPORT NastranFileReader : public AbstractFileReader, public ReaderTempl<vtkGenericDataObjectReader>
{
    Q_OBJECT
public:
    explicit NastranFileReader(QObject* parent = nullptr);

    void run();

protected:
    std::vector<int>  m_matList;
    std::map<int,int> m_uniqMatIds;
};

class VTKUTILS_EXPORT FluentFileReader : public AbstractFileReader, public ReaderTempl<vtkFLUENTReader, vtkMultiBlockDataSet>
{
    Q_OBJECT
public:
    explicit FluentFileReader(QObject* parent = nullptr);

    void run();

protected:
    QMap<QString, vtkDataArray*> m_dataMap;
};

class VTKUTILS_EXPORT AnsysFileReader : public AbstractFileReader, public ReaderTempl<vtkGenericDataObjectReader>
{
    Q_OBJECT
public:
    explicit AnsysFileReader(QObject* parent = nullptr);

    void run();

protected:
    void interpretFormatString(char *line, int &fieldStart,
        int &fieldWidth, int &expectedLineLength) const;
    void interpretFormatStringEx(char *line, int &firstFieldWidth, int &fieldStart,
                                   int &fieldWidth, int &expectedLineLength) const;
    void interpret(const char *fmt, int &fieldWidth,
        int &linelen) const;

};

class VTKUTILS_EXPORT PlyFileReader : public AbstractFileReader, public ReaderTempl<vtkPLYReader>
{
    Q_OBJECT
public:
    explicit PlyFileReader(QObject* parent = nullptr);

    void run();
};

class VTKUTILS_EXPORT PdbFileReader : public AbstractFileReader, public ReaderTempl<vtkPDBReader>
{
    Q_OBJECT
public:
    explicit PdbFileReader(QObject* parent = nullptr);

    void run();
};

} // namespace VtkUtils

#endif // FILEREADERUTILS_H
