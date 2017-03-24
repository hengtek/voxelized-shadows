#pragma once

#include <QEvent>
#include <QKeyEvent>

#include "MainWindow.hpp"
#include "Input.hpp"
#include "Vector2.hpp"

class MainWindowController : public QObject
{
    Q_OBJECT

public:
    MainWindowController(MainWindow* window);
    
protected:
    
    // Intercepts events for the renderer widget
    bool eventFilter(QObject* obj, QEvent* event);
    
public slots:
    
    // Shader feature checkboxes
    void shaderFeatureToggled();
    
    // Shadow method radio buttons
    void shadowMappingMethodToggled();
    void voxelTreeMethodToggled();
    
    // Debug overlay radio buttons
    void overlayToggled();
    
    // Shadow map radio buttons
    void shadowResolutionToggled();
    void shadowCascadesToggled();

private:
    MainWindow* window_;
    InputManager inputManager_;
    
    bool mouseDragging_;
    Vector2 mousePosition_;
    
    // Called each frame
    void update(float deltaTime);
    void applyCameraMovement();
    
    // Qt event handling
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
};
