#include "MainWindowController.hpp"

MainWindowController::MainWindowController(MainWindow* window)
    : window_(window),
    inputManager_(),
    mouseDragging_(false),
    mousePosition_(Vector2(0, 0))
{
    // Shader feature toggle signals
    for(int i = 1; i < window_->shaderFeatureToggles().size(); ++i)
    {
        connect(window_->shaderFeatureToggles()[i], SIGNAL(stateChanged(int)), this, SLOT(shaderFeatureToggled()));
    }
    
    // Shadow method radio button signals
    connect(window_->shadowMapMethodRadio(), SIGNAL(toggled(bool)), this, SLOT(shadowMappingMethodToggled()));
    connect(window_->voxelTreeMethodRadio(), SIGNAL(toggled(bool)), this, SLOT(voxelTreeMethodToggled()));
    
    // Debug overlay radio button signals
    for(int i = 1; i < window_->overlayRadios().size(); ++i)
    {
        connect(window_->overlayRadios()[i], SIGNAL(toggled(bool)), this, SLOT(overlayToggled()));
    }
    
    // Shadow map resolution radio button signals
    for(int i = 1; i < window_->shadowResolutionRadios().size(); ++i)
    {
        connect(window_->shadowResolutionRadios()[i], SIGNAL(toggled(bool)), SLOT(shadowResolutionToggled()));
    }
    
    // Shadow map cascades radio button signals
    for(int i = 1; i < window_->shadowCascadesRadios().size(); ++i)
    {
        connect(window_->shadowCascadesRadios()[i], SIGNAL(toggled(bool)), SLOT(shadowCascadesToggled()));
    }
}

bool MainWindowController::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::Paint)
    {
        update((1.0 / 60.0));
    }
    else if(event->type() == QEvent::MouseButtonPress && obj == window_->rendererWidget())
    {
        mousePressEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseButtonRelease && obj == window_->rendererWidget())
    {
        mouseReleaseEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::MouseMove && obj == window_->rendererWidget())
    {
        mouseMoveEvent(static_cast<QMouseEvent*>(event));
    }
    else if(event->type() == QEvent::KeyPress)
    {
        keyPressEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        keyReleaseEvent(static_cast<QKeyEvent*>(event));
        return true;
    }
    
    return QObject::eventFilter(obj, event);
}

void MainWindowController::shaderFeatureToggled()
{
    // The sender is a shader feature checkbox
    QCheckBox* sender = (QCheckBox*)QObject::sender();
    ShaderFeature feature = (ShaderFeature)sender->property("featureID").toInt();
    
    // Enable / disable based on check box state
    if(sender->isChecked())
    {
        window_->rendererWidget()->enableFeature(feature);
    }
    else
    {
        window_->rendererWidget()->disableFeature(feature);
    }
}

void MainWindowController::shadowMappingMethodToggled()
{
    window_->rendererWidget()->setShadowRenderMethod(SMM_ShadowMap);
}

void MainWindowController::voxelTreeMethodToggled()
{
    window_->rendererWidget()->setShadowRenderMethod(SMM_VoxelTree);
}

void MainWindowController::overlayToggled()
{
    // The sender is a overlay radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int overlay = radio->property("overlay").toInt();
    
    // Update the overlay index
    window_->rendererWidget()->setOverlay(overlay);

}

void MainWindowController::shadowResolutionToggled()
{
    // The sender is a shadow resolution radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int resolution = radio->property("resolution").toInt();
    
    // Update the shadow map resolution
    window_->rendererWidget()->setShadowMapResolution(resolution);
}

void MainWindowController::shadowCascadesToggled()
{
    // The sender is a shadow cascades radio button
    QRadioButton* radio = (QRadioButton*)QObject::sender();
    int cascades = radio->property("cascades").toInt();
    
    // Update the shadow map resolution
    window_->rendererWidget()->setShadowMapCascades(cascades);
}

void MainWindowController::update(float deltaTime)
{
    Camera* camera = window_->rendererWidget()->camera();

    // Calculate movement from inputs
    Vector3 movement;
    movement.x = inputManager_.getSidewaysMovement();
    movement.z = inputManager_.getForwardsMovement();
    movement.y = inputManager_.getVerticalMovement();
    movement = camera->localToWorldVector(movement);
    
    // Apply movement to camera (frame rate independent)
    float speed = (inputManager_.isKeyDown(IK_MoveFast)) ? 20.0 : 5.0;
    camera->translate(movement * speed * deltaTime);
}

void MainWindowController::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseDragging_ = true;
        mousePosition_ = Vector2(event->pos().x(), event->pos().y());
    }
}

void MainWindowController::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
        mouseDragging_ = false;
    }
}

void MainWindowController::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseDragging_)
    {
        Camera* camera = window_->rendererWidget()->camera();
        
        // Determine mouse movement
        Vector2 oldPosition = mousePosition_;
        Vector2 newPosition = Vector2 (event->pos().x(), event->pos().y());
        Vector2 deltaPosition = newPosition - oldPosition;
        mousePosition_ = newPosition;
        
        // Apply horizontal rotation
        Quaternion horizontal = Quaternion::rotation(deltaPosition.x * 0.5, Vector3::up());

        // Calculate vertical rotation
        Vector3 upAxis = camera->up().vec3();
        Vector3 forwardAxis = camera->forward().vec3();
        Vector3 sidewaysAxis = Vector3::cross(upAxis, forwardAxis);
        Quaternion vertical = Quaternion::rotation(deltaPosition.y * 0.5, sidewaysAxis);
    
        // Apply rotation, vertical first
        camera->setRotation(horizontal * vertical * camera->rotation());
    }
}

void MainWindowController::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_W)
        inputManager_.keyPressed(IK_MoveForward);
    else if(event->key() == Qt::Key::Key_A)
        inputManager_.keyPressed(IK_MoveLeft);
    else if(event->key() == Qt::Key::Key_S)
        inputManager_.keyPressed(IK_MoveBackwards);
    else if(event->key() == Qt::Key::Key_D)
        inputManager_.keyPressed(IK_MoveRight);
    else if(event->key() == Qt::Key::Key_E)
        inputManager_.keyPressed(IK_MoveUp);
    else if(event->key() == Qt::Key::Key_Q)
        inputManager_.keyPressed(IK_MoveDown);
    else if(event->key() == Qt::Key::Key_Shift)
        inputManager_.keyPressed(IK_MoveFast);
}

void MainWindowController::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_W)
        inputManager_.keyReleased(IK_MoveForward);
    else if(event->key() == Qt::Key::Key_A)
        inputManager_.keyReleased(IK_MoveLeft);
    else if(event->key() == Qt::Key::Key_S)
        inputManager_.keyReleased(IK_MoveBackwards);
    else if(event->key() == Qt::Key::Key_D)
        inputManager_.keyReleased(IK_MoveRight);
    else if(event->key() == Qt::Key::Key_E)
        inputManager_.keyReleased(IK_MoveUp);
    else if(event->key() == Qt::Key::Key_Q)
        inputManager_.keyReleased(IK_MoveDown);
    else if(event->key() == Qt::Key::Key_Shift)
        inputManager_.keyReleased(IK_MoveFast);
}
