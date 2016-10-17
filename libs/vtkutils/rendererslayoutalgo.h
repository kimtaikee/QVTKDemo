#ifndef RENDERERSLAYOUTALGO_H
#define RENDERERSLAYOUTALGO_H

#include <vtkRenderer.h>
#include <QList>

namespace VtkUtils
{

template <int count>
inline void layoutRenderers(const QList<vtkRenderer*>& renderers)
{

}
// |X|
template <>
inline void layoutRenderers<1>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 1);

    vtkRenderer* renderer = renderers.first();

    renderer->SetViewport(0.0, 0.0, 1.0, 1.0);
}

// |X|X|
template <>
inline void layoutRenderers<2>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 2);

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);

    renderer1->SetViewport(0.0, 0.0, 0.5, 1.0);
    renderer2->SetViewport(0.5, 0.0, 1.0, 1.0);
}

// |X|X|
// | X |
template <>
inline void layoutRenderers<3>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 3);

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);

    renderer1->SetViewport(0.0, 0.0, 0.5, 0.5);
    renderer2->SetViewport(0.5, 0.0, 1.0, 0.5);

    renderer3->SetViewport(0.0, 0.5, 1.0, 1.0);

}

// |X|X|
// |X|X|
template <>
inline void layoutRenderers<4>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 4);

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);

    renderer1->SetViewport(0.0, 0.0, 0.5, 0.5);
    renderer2->SetViewport(0.5, 0.0, 1.0, 0.5);

    renderer3->SetViewport(0.0, 0.5, 0.5, 1.0);
    renderer4->SetViewport(0.5, 0.5, 1.0, 1.0);
}

// |X|X|
// |X|X|
// | X |
template <>
inline void layoutRenderers<5>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 5);
    qreal dy = (qreal) 1 / 3;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);

    renderer1->SetViewport(0.0, 0.0, 0.5, dy);
    renderer2->SetViewport(0.5, 0.0, 1.0, dy);

    renderer3->SetViewport(0.0, dy, 0.5, 2 * dy);
    renderer4->SetViewport(0.5, dy, 1.0, 2 * dy);

    renderer5->SetViewport(0.0, 2 * dy, 1.0, 1.0);
}

// |X|X|X|
// |X|X|X|
template <>
inline void layoutRenderers<6>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 6);

    qreal dx = (qreal) 1 / 3;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);
    vtkRenderer* renderer6 = renderers.at(5);

    renderer1->SetViewport(0.0, 0.0, dx, 0.5);
    renderer2->SetViewport(dx, 0.0, 2 * dx, 0.5);
    renderer3->SetViewport(2 * dx, 0.0, 1.0, 0.5);

    renderer4->SetViewport(0.0, 0.5, dx, 1.0);
    renderer5->SetViewport(dx, 0.5, 2 * dx, 1.0);
    renderer6->SetViewport(2 * dx, 0.5, 1.0, 1.0);
}

// |X|X|X|
// |X|X|X|
// |  X  |
template <>
inline void layoutRenderers<7>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 7);

    qreal dx = (qreal) 1 / 3;
    qreal dy = (qreal) 1 / 3;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);
    vtkRenderer* renderer6 = renderers.at(5);
    vtkRenderer* renderer7 = renderers.at(6);

    renderer1->SetViewport(0.0, 0.0, dx, dy);
    renderer2->SetViewport(dx, 0.0, 2 * dx, dy);
    renderer3->SetViewport(2 * dx, 0.0, 1.0, dy);

    renderer4->SetViewport(0.0, dy, dx, 2 * dy);
    renderer5->SetViewport(dx, dy, 2 * dx, 2 * dy);
    renderer6->SetViewport(2 * dx, dy, 1.0, 2 * dy);

    renderer7->SetViewport(0.0, 2 * dy, 1.0, 1.0);
}

// |X|X|X|X|
// |X|X|X|X|
template <>
inline void layoutRenderers<8>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 8);

    qreal dx = (qreal) 1 / 4;
    qreal dy = (qreal) 1 / 2;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);
    vtkRenderer* renderer6 = renderers.at(5);
    vtkRenderer* renderer7 = renderers.at(6);
    vtkRenderer* renderer8 = renderers.at(7);

    renderer1->SetViewport(0.0, 0.0, dx, dy);
    renderer2->SetViewport(dx, 0.0, 2 * dx, dy);
    renderer3->SetViewport(2 * dx, 0.0, 1.0, dy);
    renderer4->SetViewport(3 * dx, dy, 1.0, dy);

    renderer5->SetViewport(0.0, dy, dx, 1.0);
    renderer6->SetViewport(dx, dy, 2 * dx, 1.0);
    renderer7->SetViewport(2 * dx, dy, 3 * dx, 1.0);
    renderer8->SetViewport(3 * dx, dy, 1.0, 1.0);
}

// |X|X|X|
// |X|X|X|
// |X|X|X|
template <>
inline void layoutRenderers<9>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 9);

    qreal dx = (qreal) 1 / 3;
    qreal dy = (qreal) 1 / 3;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);
    vtkRenderer* renderer6 = renderers.at(5);
    vtkRenderer* renderer7 = renderers.at(6);
    vtkRenderer* renderer8 = renderers.at(7);
    vtkRenderer* renderer9 = renderers.at(8);

    renderer1->SetViewport(0.0, 0.0, dx, dy);
    renderer2->SetViewport(dx, 0.0, 2 * dx, dy);
    renderer3->SetViewport(2 * dx, 0.0, 1.0, dy);

    renderer4->SetViewport(0.0, dy, dx,2 * dy);
    renderer5->SetViewport(dx, dy, 2 * dx, 2 * dy);
    renderer6->SetViewport(2 * dx, dy, 1.0, 2 * dy);

    renderer7->SetViewport(0.0, 2 * dy, dx, 1.0);
    renderer8->SetViewport(dx, 2 * dy, 2 * dx, 1.0);
    renderer9->SetViewport(2 * dx, 2 * dy, 1.0, 1.0);
}

// |X|X|X|X|X|
// |X|X|X|X|X|
template <>
inline void layoutRenderers<10>(const QList<vtkRenderer*>& renderers)
{
    Q_ASSERT(renderers.size() == 10);

    qreal dx = (qreal) 1 / 5;
    qreal dy = (qreal) 1 / 2;

    vtkRenderer* renderer1 = renderers.at(0);
    vtkRenderer* renderer2 = renderers.at(1);
    vtkRenderer* renderer3 = renderers.at(2);
    vtkRenderer* renderer4 = renderers.at(3);
    vtkRenderer* renderer5 = renderers.at(4);
    vtkRenderer* renderer6 = renderers.at(5);
    vtkRenderer* renderer7 = renderers.at(6);
    vtkRenderer* renderer8 = renderers.at(7);
    vtkRenderer* renderer9 = renderers.at(8);
    vtkRenderer* renderer10 = renderers.at(9);

    renderer1->SetViewport(0.0, 0.0, dx, dy);
    renderer2->SetViewport(dx, 0.0, 2 * dx, dy);
    renderer3->SetViewport(2 * dx, 0.0, 3 * dx, dy);
    renderer4->SetViewport(3 * dx, 0.0, 4 * dx, dy);
    renderer5->SetViewport(4 * dx, 0.0, 1.0, dy);

    renderer6->SetViewport(0.0, dy, dx, 1.0);
    renderer7->SetViewport(dx, dy, 2 * dx, 1.0);
    renderer8->SetViewport(2 * dx, dy, 3 * dx, 1.0);
    renderer9->SetViewport(3 * dx, dy, 4 * dx, 1.0);
    renderer10->SetViewport(4 * dx, dy, 1.0, 1.0);
}

// |X|X|X|X|
// |X|X|X|X|
// |X|X| X |
template <>
inline void layoutRenderers<11>(const QList<vtkRenderer*>& renderers)
{

}

// |X|X|X|X|
// |X|X|X|X|
// |X|X|X|X|
template <>
inline void layoutRenderers<12>(const QList<vtkRenderer*>& renderers)
{

}

} // namespace VtkUtils

#endif // RENDERERSLAYOUTALGO_H
