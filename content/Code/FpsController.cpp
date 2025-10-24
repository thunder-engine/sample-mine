#include <nativebehaviour.h>
#include <transform.h>
#include <actor.h>
#include <world.h>
#include <input.h>
#include <playerinput.h>
#include <camera.h>
#include <charactercontroller.h>
#include <timer.h>
#include <log.h>

#include "WorldManager.cpp"

class FpsController : public NativeBehaviour {
    A_OBJECT(FpsController, NativeBehaviour, Components)

    A_PROPERTIES(
        A_PROPERTY(PlayerInput *, playerInput, FpsController::playerInput, FpsController::setPlayerInput),
        A_PROPERTY(CharacterController *, characterController, FpsController::characterController, FpsController::setCharacterController),
        A_PROPERTY(Transform *, targetCube, FpsController::targetCube, FpsController::setTargetCube)
    )
    
    PlayerInput *m_playerInput = nullptr;
    CharacterController *m_characterCtrl = nullptr;
    Transform *m_targetCube = nullptr;
    
    float speed = 6.0f;
    float jumpSpeed = 5.0f;
    float gravity = 20.0f;
    Vector3 m_moveDirection;
    
public:
    // Use this to initialize behaviour
    void start() override {
        Input::mouseLockCursor(true);
    }

    // Will be called each frame. Use this to write your game logic
    void update() override {
        Camera *camera = Camera::current();
        if(camera) {
            Transform *camera_t = camera->transform();

            Vector4 delta = Input::mouseDelta() * 0.2f;

            Transform *t = transform();
            t->setRotation(t->rotation() - Vector3(0.0f, delta.x, 0.0f));

            camera_t->setRotation(camera_t->rotation() - Vector3(-delta.y, 0.0f, 0.0f));

            int x, y, z;
            Ray ray = camera->castRay(0.5f, 0.5f);
            Ray::Hit hit;
            bool result = world()->rayCast(ray, 4.0f, &hit);
            if (result) {
                Vector3 pos = hit.point - hit.normal * 0.5f;

                x = floor(pos.x);
                y = floor(pos.y);
                z = floor(pos.z);
                if(m_targetCube) {
                    m_targetCube->actor()->setEnabled(true);
                    m_targetCube->setPosition(Vector3(x + 0.5f, y + 0.5f, z + 0.5f));
                }
            } else {
                if(m_targetCube) {
                    m_targetCube->actor()->setEnabled(false);
                }
            }

            if (result) {
                if(Input::isMouseButtonDown(Input::MOUSE_LEFT)) {
                    WorldManager::changeBlock(x, y, z, BlockType::Air);
                } else if(Input::isMouseButtonDown(Input::MOUSE_RIGHT)) {
                    Vector3 pos = hit.point + hit.normal * 0.5f;

                    WorldManager::changeBlock(floor(pos.x), floor(pos.y), floor(pos.z), BlockType::Dirt);
                }
            }

            if(m_playerInput && m_characterCtrl) {
                m_moveDirection = t->quaternion() * Vector3(m_playerInput->axis("Side") * speed,
                                                            m_moveDirection.y,
                                                            -m_playerInput->axis("Front") * speed);

                if(m_characterCtrl->isGrounded()) {
                    if(m_playerInput->button("Jump")) {
                        m_moveDirection.y = jumpSpeed;
                    }

                    m_moveDirection.y -= gravity * Timer::deltaTime();
                    m_moveDirection.y = MAX(m_moveDirection.y, -10.0f);
                } else {
                    m_moveDirection.y = 0.0f;
                }
                m_characterCtrl->move(m_moveDirection * Timer::deltaTime());
            }
        }
    }
    
    PlayerInput *playerInput() const {
        return m_playerInput;
    }
    
    void setPlayerInput(PlayerInput *input) {
        m_playerInput = input;
    }
    
    CharacterController *characterController() const {
        return m_characterCtrl;
    }
    
    void setCharacterController(CharacterController *ctrl) {
        m_characterCtrl = ctrl;
    }
    
    Transform *targetCube() const {
        return m_targetCube;
    }
    
    void setTargetCube(Transform *target) {
        m_targetCube = target;
    }

};
