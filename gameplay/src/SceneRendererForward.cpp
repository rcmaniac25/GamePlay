#include "Base.h"
#include "SceneRendererForward.h"
#include "Terrain.h"
#include "Model.h"
#include "ParticleEmitter.h"
#include "Form.h"

namespace gameplay
{

SceneRendererForward::SceneRendererForward()
    : SceneRenderer()
{
}

SceneRendererForward::SceneRendererForward(const SceneRendererForward& copy)
{
}

SceneRendererForward::~SceneRendererForward()
{
}

SceneRendererForward* SceneRendererForward::create()
{
    return new SceneRendererForward();
}

unsigned int SceneRendererForward::render(VisibleSet* set)
{
    set->reset();
    unsigned int drawCalls = 0;
    Node* next = set->getNext();
    do
    {
        drawCalls += drawNode(next);
        next = set->getNext();

    } while ( next != NULL);

    return drawCalls;
}

unsigned int SceneRendererForward::drawNode(Node* node)
{
    unsigned int drawCalls = 0;

    // Draw Terrain
    Terrain* terrain = node->getTerrain();
    if (terrain)
        drawCalls += terrain->draw(isWireframe());

    // Draw Models (Shadows) TODO:

    // Draw Modes
    Model* model = node->getModel();
    if (model)
        drawCalls += model->draw(isWireframe());

    // Draw particles
    ParticleEmitter* emitter = node->getParticleEmitter();
    if (emitter)
        drawCalls += emitter->draw();

    // Draw forms
    Form* form = node->getForm();
    if (form)
        drawCalls += form->draw();

    return drawCalls;
}

}