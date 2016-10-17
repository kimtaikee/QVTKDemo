#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <QColor>
#include <QtMath>

#include <complex>

#include "utils_global.h"

class vtkActor;
namespace Utils
{

// alphabetical char
QString UTILS_EXPORT character(int index);
void UTILS_EXPORT explorer(const QString& path);
QImage UTILS_EXPORT star(const QSize& size = QSize(30, 30));

// declare some util functions
double UTILS_EXPORT random(int low, int high);

template <typename T>
inline static std::complex<T> random(int low, int high)
{
    std::complex<T> c(random(low, high), random(low, high));
    return c;
}

// convert QColor to double[3] clr
void UTILS_EXPORT vtkColor(const QColor& clr, double* vtkClr);
QColor UTILS_EXPORT qColor(double* pClr);
void UTILS_EXPORT qColor2HSV(const QColor& clr, double* hsv);

template <typename T, int size = 3>
class ArrayComparator
{
public:
    bool operator()(const T* lhs, const T* rhs)
    {
        for (auto i = 0; i < size; ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }
        return true;
    }
};

template <typename T, int size = 3>
class ArrayAssigner
{
public:
    void operator()(T* lhs, const T* rhs)
    {
        for (auto i = 0; i < size; ++i)
            lhs[i] = rhs[i];
    }
};

template <typename T, int size = 3>
class ArrayInitializer
{
public:
    void operator()(T* array, T value = T())
    {
        for (auto i = 0; i < size; ++i)
            array[i] = value;
    }
};

class Normalizer
{
public:
    void operator()(const double* input, double* output)
    {
        double mod = qSqrt(input[0] * input[0] +
                           input[1] * input[1] +
                           input[2] * input[2]);
        if (mod == 0) {
            output[0] = input[0];
            output[1] = input[1];
            output[2] = input[2];
        } else {
            output[0] = input[0] / mod;
            output[1] = input[1] / mod;
            output[2] = input[2] / mod;
        }
    }
};

typedef QList<vtkActor*> ActorList;

template <class T>
inline void vtkSafeDelete(T* obj)
{
    if (obj)
        obj->Delete();
}

template <class T>
inline void vtkSafeDelete(QList<T*>& objList)
{
    foreach (T* obj, objList)
        obj->Delete();
    objList.clear();
}

template <typename T>
inline T boundedValue(const T& value, const T& min, const T& max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

template <typename T>
static inline double module(T* vector)
{
    return qSqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}

template <typename T>
static inline double distance(T* pot1, T* pot2)
{
    double dX = pot2[0] - pot1[0];
    double dY = pot2[1] - pot1[1];
    double dZ = pot2[2] - pot1[2];
    return qSqrt(dX * dX + dY * dY + dZ * dZ);
}

/*!
 * \brief 求两点的法向量.
 */
template <typename T>
static inline void normal(T* inPot1, T* inPot2, T* outPot)
{
    outPot[0] = -(inPot2[1] - inPot1[1]);
    outPot[1] = inPot2[0] - inPot1[0];
    outPot[2] = inPot1[2];
}


} // namespace Utils
#endif
