#include "filereaderutils.h"
#include "vtkUnstructuredGridRelevantPointsFilter.h"

#include <vtkUnstructuredGrid.h>
#include <vtkCellData.h>

#include <QDebug>

#define USE_POINT_INDICES_TO_INSERT
#define ALL_LINES -1
#define INVALID_MAT_ID -INT_MAX

#if defined(_WIN32)
// for _strnicmp
#include <string.h>
#define STRNCASECMP _strnicmp
#else
// for strcasecmp
#include <strings.h>
#define STRNCASECMP strncasecmp
#endif

namespace VtkUtils
{

static float Getf(const char *s)
{
    char *ends;
    double val = 0.0;

    // Check for one of these funky 'NASTRAN exponential format' strings.
    // This is where a value like '1.2345e-5' is actually represented in the
    // file as '1.2345-5' with the 'e' character missing. It is awkward but
    // apparently a NASTRAN standard. I guess the rationale is that given
    // an 8 character field width limit, removing the 'e' character gives them
    // one additional digit of precision. This logic is basically looking for
    // the condition of encountering a sign character, '-' or '+', AFTER having
    // seen characters that could represent part of a number. In such a case,
    // it MUST be the sign of the exponent.
    const char *p = s;
    char tmps[32];
    char *q = tmps;
    bool haveSeenNumChars = false;
    while (!haveSeenNumChars || (*p != '-' && *p != '+' && *p != '\0')) {
        if ('0' <= *p && *p <= '9' || *p == '.' || *p == '+' || *p == '-')
            haveSeenNumChars = true;
        *q++ = *p++;
    }

    if (haveSeenNumChars && (*p == '-' || *p == '+')) {
        *q++ = 'e';
        while (*p != '\0')
            *q++ = *p++;
        *q++ = '\0';
        errno = 0;
        val = strtod(tmps, &ends);
    } else {
        errno = 0;
        val = strtod(s, &ends);
    }

    if (errno != 0)
    {
        qDebug() << "Error occured at " << __FUNCTION__;
        return 0.0;
    }

    return (float) val;
}

// ****************************************************************************
// Function: Geti
//
// Purpose: Robust way of reading string for integer value
//
// Programmer: Mark C. Miller, Thu Apr  3 16:27:01 PDT 2008
//
// Modifications:
//    Jeremy Meredith, Thu Aug  7 13:43:03 EDT 2008
//    Format %s doesn't use space modifier.
//
// ****************************************************************************
static int Geti(const char *s)
{
    char *ends;

    errno = 0;
    long val = strtol(s, &ends, 10);

    if (errno != 0)
    {
        qDebug() << "Error occured at " << __FUNCTION__;
        return 0;
    }

    return (int) val;
}
AbstractFileReader::AbstractFileReader(QObject *parent) : QObject(parent)
{

}

void AbstractFileReader::setFileName(const QString& file)
{
    if (m_fileName != file)
        m_fileName = file;
}

QString AbstractFileReader::fileName() const
{
    return m_fileName;
}

void AbstractFileReader::setTitle(const QString& title)
{
    if (m_title != title)
        m_title = title;
}

QString AbstractFileReader::title() const
{
    return m_title;
}

VtkFileReader::VtkFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void VtkFileReader::run()
{
    vtkInitOnce(&m_reader);
    m_reader->ReadAllScalarsOn();
    m_reader->ReadAllVectorsOn();
    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();
    emit finished();
}

StlFileReader::StlFileReader(QObject *parent) : AbstractFileReader(parent)
{


}

void StlFileReader::run()
{
    vtkInitOnce(&m_reader);
    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();

    emit finished();
}

ObjFileReader::ObjFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void ObjFileReader::run()
{
    vtkInitOnce(&m_reader);
    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();
    emit finished();
}

DynaFileReader::DynaFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void DynaFileReader::run()
{
    vtkInitOnce(&m_reader);

    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();

    emit finished();
}

NastranFileReader::NastranFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void NastranFileReader::run()
{
    ifstream ifile(fileName().toLocal8Bit());
    if (ifile.fail())
        return;

    // Determine the file size and come up with an estimate of the
    // number of vertices and cells so we can size the points and cells.
    int nPoints = 100;
    int nCells = 100;
    int matCountOpt = 0;

#if !defined(MDSERVER)

    vtkPoints *pts = vtkPoints::New();
    pts->Allocate(nPoints);
#ifdef USE_POINT_INDICES_TO_INSERT
    pts->SetNumberOfPoints(nPoints);
#endif

    vtkUnstructuredGrid *ugrid = vtkUnstructuredGrid::New();
    ugrid->SetPoints(pts);
    ugrid->Allocate(nCells);
    pts->Delete();
    pts = ugrid->GetPoints();

#endif // if !defined(MDSERVER)

    char  line[1024];
    float pt[3];
    vtkIdType verts[8];
    bool recognized = false;
    bool titleRead = false;
    for(int lineIndex = 0; !ifile.eof(); ++lineIndex)
    {
        int matid = INVALID_MAT_ID;

        // Get the line
        ifile.getline(line, 1024);

        // Determine what the line is for.
        if(line[0] == '$' && !recognized)
        {
            if(!titleRead)
                m_title += line;
        }
        else if(strncmp(line, "GRID*", 5) == 0)
        {
            recognized = true;

            // These GRID* lines need a second line of data. Read it into
            // the same buffer at the end.
            ifile.getline(line + 72, 1024-72);

#define LONG_FIELD_WIDTH 16
            char *valstart = line + 81 - 1;
            pt[2] = Getf(valstart);

            valstart = line + 72 - LONG_FIELD_WIDTH+1 - 1;
            char *valend = line + 72;
            *valend = '\0';
            pt[1] = Getf(valstart);

            valstart -= LONG_FIELD_WIDTH;
            valend -= LONG_FIELD_WIDTH;
            *valend = '\0';
            pt[0] = Getf(valstart);

#ifdef USE_POINT_INDICES_TO_INSERT
            valstart -= (2 * LONG_FIELD_WIDTH);
            valend -= (2 * LONG_FIELD_WIDTH);
            *valend = '\0';
            int psi = Geti(valstart)-1;

#if !defined(MDSERVER)

            if(psi < nPoints)
                pts->SetPoint(psi, pt);
            else
            {
                int newSize = int(float(nPoints) * 1.1f);
                if(newSize < psi)
                    newSize = int(float(psi) * 1.1f);
                if(newSize <= nPoints)
                    newSize = nPoints + 1;

                qDebug() << "Resizing point array from " << nPoints
                         << " points to " << newSize
                         << " points because we need to insert point index "
                         << psi << endl;
                nPoints = newSize;
                pts->GetData()->Resize(nPoints);
                pts->SetNumberOfPoints(nPoints);
                pts->SetPoint(psi, pt);
            }

#endif // if !defined(MDSERVER)

#else
#if !defined(MDSERVER)
            pts->InsertNextPoint(pt);
#endif
#endif

        }
        else if(strncmp(line, "GRID", 4) == 0)
        {
            recognized = true;

#define SHORT_FIELD_WIDTH 8
            char *valstart = line + 48 - SHORT_FIELD_WIDTH;
            char *valend = line + 48;

            *valend = '\0';
            pt[2] = Getf(valstart);

            valstart -= SHORT_FIELD_WIDTH;
            valend -= SHORT_FIELD_WIDTH;
            *valend = '\0';
            pt[1] = Getf(valstart);

            valstart -= SHORT_FIELD_WIDTH;
            valend -= SHORT_FIELD_WIDTH;
            *valend = '\0';
            pt[0] = Getf(valstart);

#ifdef USE_POINT_INDICES_TO_INSERT
            valstart -= (2 * SHORT_FIELD_WIDTH);
            valend -= (2 * SHORT_FIELD_WIDTH);
            *valend = '\0';
            int psi = Geti(valstart)-1;

#if !defined(MDSERVER)

            if(psi < nPoints)
                pts->SetPoint(psi, pt);
            else
            {
                int newSize = int(float(nPoints) * 1.1f);
                if(newSize < psi)
                    newSize = int(float(psi) * 1.1f);
                if(newSize <= nPoints)
                    newSize = nPoints + 1;

                qDebug() << "Resizing point array from " << nPoints
                         << " points to " << newSize
                         << " points because we need to insert point index "
                         << psi << endl;
                nPoints = newSize;
                pts->GetData()->Resize(nPoints);
                pts->SetNumberOfPoints(nPoints);
                pts->SetPoint(psi, pt);
            }

#endif // if !defined(MDSERVER)

#else

#if !defined(MDSERVER)
            pts->InsertNextPoint(pt);
#endif // if !defined(MDSERVER)

#endif


        }
        else if(strncmp(line, "CHEXA", 5) == 0)
        {
#define INDEX_FIELD_WIDTH 8
            // CHEXA requires more point indices so read another line.
            ifile.getline(line + 72, 1024-72);

            char *valstart = line + 88;
            char *valend = valstart;
            verts[7] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[6] = Geti(valstart)-1;

            // Skip the blank
            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[5] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[4] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[3] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_HEXAHEDRON, 8, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CTETRA", 6) == 0)
        {
            char *valstart = line + 48;
            char *valend = valstart;
            verts[3] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_TETRA, 4, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CPYRAM", 6) == 0)
        {
            char *valstart = line + 56;
            char *valend = valstart;
            verts[4] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[3] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_PYRAMID, 5, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CPENTA", 6) == 0)
        {
            char *valstart = line + 64;
            char *valend = valstart;
            verts[5] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[4] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[3] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }
            //
            // http://www.simcenter.msstate.edu/docs/ug_io/3d_grid_file_type_nastran.html
            // says that if 5th and 6th nodes are identical, then its really a 5 noded
            // pyramid.
            //
#if !defined(MDSERVER)
            if (verts[4] == verts[5])
                ugrid->InsertNextCell(VTK_PYRAMID, 5, verts);
            else
                ugrid->InsertNextCell(VTK_WEDGE, 6, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CQUAD4", 6) == 0)
        {
            char *valstart = line + 48;
            char *valend = valstart;
            verts[3] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_QUAD, 4, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CTRIA2", 6) == 0 ||
                strncmp(line, "CTRIA3", 6) == 0)
        {
            char *valstart = line + 40;
            char *valend = valstart;
            verts[2] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            valend -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_TRIANGLE, 3, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "CBAR", 4) == 0)
        {
            char *valstart = line + 32;
            char *valend = valstart;
            verts[1] = Geti(valstart)-1;

            valstart -= INDEX_FIELD_WIDTH;
            *valend = '\0';
            verts[0] = Geti(valstart)-1;

            if (matCountOpt)
            {
                valstart -= INDEX_FIELD_WIDTH;
                valend -= INDEX_FIELD_WIDTH;
                *valend = '\0';
                matid = Geti(valstart);
            }

#if !defined(MDSERVER)
            ugrid->InsertNextCell(VTK_LINE, 2, verts);
            if (matCountOpt) m_matList.push_back(matid);
#endif
        }
        else if(strncmp(line, "TITLE", 5) == 0)
        {
            titleRead = true;
            char *ptr = 0;
            if((ptr = strstr(line, "=")) != 0)
                m_title = /*std::string(*/++ptr/*)*/;
            else
                m_title = /*std::string(*/line+5/*)*/;
        }
        else if(strncmp(line, "CEND", 4) == 0 ||
                strncmp(line, "BEGIN BULK", 10) == 0 ||
                strncmp(line, "NASTRAN", 7) == 0)
        {
            recognized = true;
        }

        if (matid != INVALID_MAT_ID)
            m_uniqMatIds[matid] = 1;
    }

    if(recognized) {
        vtkUnstructuredGridRelevantPointsFilter *rpf =
                vtkUnstructuredGridRelevantPointsFilter::New();
        rpf->SetInputData(ugrid);
        rpf->Update();

        m_dataObject = vtkUnstructuredGrid::New();
        m_dataObject->ShallowCopy(rpf->GetOutput());
        ugrid->Delete();
        rpf->Delete();
    }

    emit finished();
}

FluentFileReader::FluentFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void FluentFileReader::run()
{
    vtkInitOnce(&m_reader);

    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();

    vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast(m_reader->GetOutput()->GetBlock(0));
    int num = m_reader->GetNumberOfCellArrays();
    for (int i = 0; i < num; ++i) {
        QString name(m_reader->GetCellArrayName(i));
        m_dataMap.insert(name, grid->GetCellData()->GetArray(m_reader->GetCellArrayName(i)));
    }

    emit finished();
}

AnsysFileReader::AnsysFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void AnsysFileReader::run()
{
    // Open the file.
    ifstream ifile(fileName().toLocal8Bit());
    if (ifile.fail())
        return;

    // Determine the file size and come up with an estimate of the
    // number of vertices and cells so we can size the points and cells.
    int nPoints = 100;
    int nCells = 100;
    vtkPoints *pts = vtkPoints::New();
    pts->Allocate(nPoints);

    vtkUnstructuredGrid *ugrid = vtkUnstructuredGrid::New();
    ugrid->SetPoints(pts);
    ugrid->Allocate(nCells);
    pts->Delete();

#define MAX_ANSYS_LINE 200
    char  line[MAX_ANSYS_LINE];
    float pt[3];
    vtkIdType   verts[8];
    bool  recognized = false;
    bool  fatalError = false;
    bool  readingCoordinates = false;
    bool  readingConnectivity = false;
    int   expectedLineLength = 0;

    int   firstFieldWidth = 8;
    int   fieldWidth = 16;
    int   fieldStart = 56;

    for(int lineIndex = 0; !ifile.eof(); ++lineIndex)
    {
        // Get the line
        ifile.getline(line, MAX_ANSYS_LINE);
#if defined(_WIN32)
        int linelen = strlen(line);
#else
        int linelen = strlen(line)-1; // account for the end of line char.
#endif
        // If the line length is less than expected then pad with NULLs.
        if(expectedLineLength > 0 && linelen < expectedLineLength)
        {
            memset(line + linelen + 1, 0, (MAX_ANSYS_LINE - linelen - 1) * sizeof(char));
#if 0
            debug5 << "Padding line with NULLs" << endl;
            debug5 << line << endl;
#endif
        }

        // Give it a chance to break out of coordinate reading.
        if(readingCoordinates)
        {
            bool valid = true;
            for(int i = 0; i < firstFieldWidth && valid; ++i)
                valid &= (line[i] == ' ' || (line[i] >= '0' && line[i] <= '9'));

            if(!valid)
            {
                expectedLineLength = 0;
                readingCoordinates = false;
                continue;
            }
        }

        if(readingCoordinates)
        {
            char *valstart = line + fieldStart;
            char *valend = valstart + fieldWidth;
            pt[2] = atof(valstart);

            valstart -= fieldWidth;
            valend -= fieldWidth;
            *valend = '\0';
            pt[1] = atof(valstart);

            valstart -= fieldWidth;
            valend -= fieldWidth;
            *valend = '\0';
            pt[0] = atof(valstart);

#if 0
            debug4 << pt[0] << ", " << pt[1] << ", " << pt[2] << endl;
#endif
            pts->InsertNextPoint(pt);
        }
        else if(readingConnectivity)
        {
            // Get whether this cell is real from column 0
            line[fieldWidth] = '\0';
            bool realCell = atoi(line) > 0;
            if(!realCell)
            {
                expectedLineLength = 0;
                readingConnectivity = false;
                continue;
            }

            // Get the number of vertices in this cell from column 9.
            static const int ncellsColumn = 9;
            line[ncellsColumn * fieldWidth] = '\0';
            int nverts = atoi(line + (ncellsColumn-1) * fieldWidth);

            if(nverts == 8)
            {
                char *valstart = line + fieldStart;
                char *valend   = valstart + fieldWidth;
                for(int i = 0; i < 8; ++i)
                {
                    int ivalue = atoi(valstart);
                    verts[7-i] = (ivalue > 0) ? (ivalue - 1) : ivalue;
                    valstart -= fieldWidth;
                    valend   -= fieldWidth;
                    *valend = '\0';
                }

#if 0
                for(int j = 0; j < 8; ++j)
                    debug4 << ", " << verts[j];
                debug4 << endl;
#endif
                ugrid->InsertNextCell(VTK_HEXAHEDRON, 8, verts);
            }
            else
            {
                qDebug() << "The file " << fileName() << " contained cells "
                                                            "that are not hexes."<< endl;

                fatalError = true;
                break;
            }
        }
        else if(STRNCASECMP(line, "NBLOCK", 6) == 0)
        {
            char *comma = strstr(line, ",");
            if(comma != 0)
            {
                char *comma2 = strstr(comma+1, ",");
                if(comma2 != 0)
                {
                    *comma2 = '\0';
                    char *cols = comma + 1;
                    qDebug() << "Coordinate data stored in "
                             << atoi(cols) << " columns." << endl;
                    recognized = true;
                }
            }

            // Get the field format string. Use it to set expectedLineLength,
            // fieldWidth, and fieldStart.
            ifile.getline(line, 1024);
            if(line[0] == '(')
            {
                interpretFormatStringEx(line, firstFieldWidth, fieldStart, fieldWidth,
                                      expectedLineLength);
                qDebug() << "firstFieldWidth=" << firstFieldWidth
                         << ", fieldStart=" << fieldStart
                         << ", fieldWidth=" << fieldWidth
                         << ", expectedLineLength=" << expectedLineLength
                         << endl;
                readingCoordinates = true;
            }
            else
            {
                qDebug() << "Malformed format string: " << line << endl;
                fatalError = true;
            }
        }
        else if(STRNCASECMP(line, "EBLOCK", 6) == 0)
        {
            char *comma = strstr(line, ",");
            if(comma != 0)
            {
                char *comma2 = strstr(comma+1, ",");
                if(comma2 != 0)
                {
                    *comma2 = '\0';
                    char *cols = comma + 1;
                    qDebug() << "Connectivity data stored in "
                             << atoi(cols) << " columns." << endl;
                    recognized = true;
                }
            }

            // Get the field format string. Use it to set expectedLineLength,
            // fieldWidth, and fieldStart.
            ifile.getline(line, 1024);
            if(line[0] == '(')
            {
                interpretFormatStringEx(line, firstFieldWidth, fieldStart, fieldWidth,
                                      expectedLineLength);
                qDebug() << "firstFieldWidth=" << firstFieldWidth
                         << ", fieldStart=" << fieldStart
                         << ", fieldWidth=" << fieldWidth
                         << ", expectedLineLength=" << expectedLineLength
                         << endl;
                readingConnectivity = true;
            }
            else
            {
                qDebug() << "Malformed format string: " << line << endl;
                fatalError = true;
            }
        }
        else if(STRNCASECMP(line, "/COM", 4) == 0)
        {
            if(m_title.toStdString() == "")
                m_title = /*std::string(*/line+6/*)*/;
            recognized = true;
        }
        else if(STRNCASECMP(line, "/TITLE", 6) == 0)
        {
            m_title = /*std::string(*/line+8/*)*/;
            recognized = true;
        }
        else if(STRNCASECMP(line, "/BATCH",  6) == 0 ||
                STRNCASECMP(line, "/NOPR",   5) == 0 ||
                STRNCASECMP(line, "/CONFIG", 7) == 0 ||
                STRNCASECMP(line, "/NOLIST", 7) == 0)
        {
            recognized = true;
        }
        else
        {
            expectedLineLength = 0;
            readingCoordinates = false;
            readingConnectivity = false;
        }
    }

    if(recognized && !fatalError)
        m_dataObject = ugrid;
    else
        ugrid->Delete();

    emit finished();
}

void AnsysFileReader::interpretFormatString(char *line, int &fieldStart,
                                              int &fieldWidth, int &expectedLineLength) const
{
    char *fmt = line + 1;
    char *ptr = 0;

    expectedLineLength = 0;
    bool keepGoing = true;
    while(keepGoing)
    {
        int linelen = 0;

        if((ptr = strstr(fmt, ",")) != 0)
        {
            *ptr = '\0';
            interpret(fmt, fieldWidth, linelen);
            expectedLineLength += linelen;
            fmt = ptr + 1;
        }
        else if((ptr = strstr(fmt, ")")) != 0)
        {
            *ptr = '\0';
            interpret(fmt, fieldWidth, linelen);
            expectedLineLength += linelen;
            keepGoing = false;
        }
        else
            keepGoing = false;
    }

    fieldStart = expectedLineLength - fieldWidth;
}

void AnsysFileReader::interpretFormatStringEx(char *line, int &firstFieldWidth, int &fieldStart, int &fieldWidth, int &expectedLineLength) const
{
    char *fmt = line + 1;
    char *ptr = 0;

    expectedLineLength = 0;
    bool keepGoing = true;
    bool first = true;
    while(keepGoing)
    {
        int linelen = 0;

        if((ptr = strstr(fmt, ",")) != 0)
        {
            *ptr = '\0';
            interpret(fmt, fieldWidth, linelen);
            if(first)
            {
                first = false;
                firstFieldWidth = fieldWidth;
            }
            expectedLineLength += linelen;
            fmt = ptr + 1;
        }
        else if((ptr = strstr(fmt, ")")) != 0)
        {
            *ptr = '\0';
            interpret(fmt, fieldWidth, linelen);
            if(first)
            {
                first = false;
                firstFieldWidth = fieldWidth;
            }
            expectedLineLength += linelen;
            keepGoing = false;
        }
        else
            keepGoing = false;
    }

    fieldStart = expectedLineLength - fieldWidth;
}

void AnsysFileReader::interpret(const char *fmt, int &fieldWidth,
                                  int &linelen) const
{
    int i0, i1, i2;

    qDebug() << "avtANSYSFileFormat::Interpret: " << fmt << endl;

    // Example: 6e16.9
    if(sscanf(fmt, "%de%d.%d", &i0, &i1, &i2) == 3)
    {
        linelen = i0 * i1 / 2;
        fieldWidth = i1;
    }
    // Example: 19i7
    else if(sscanf(fmt, "%di%d", &i0, &i1) == 2)
    {
        linelen = i0 * i1;
        fieldWidth = i1;
    }
    else
    {
        qDebug() << "Invalid format string: " << fmt << endl;
    }
}

PlyFileReader::PlyFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void PlyFileReader::run()
{
    vtkInitOnce(&m_reader);
    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();
    emit finished();
}

PdbFileReader::PdbFileReader(QObject *parent) : AbstractFileReader(parent)
{

}

void PdbFileReader::run()
{
    vtkInitOnce(&m_reader);
    m_reader->SetFileName(fileName().toLocal8Bit());
    m_reader->Update();
    emit finished();
}

} // namespace VtkUtils
