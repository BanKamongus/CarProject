#include "BanKEngine.h"
#include "Renderer.h"
#include "Input.h"


const float KeyframeAwait = 0.005f;
class Keyframe {
public:
    float time;
    Transform transform;

    Keyframe(float t, Transform tr)
        : time(t), transform(tr) {}
};


class B_CarGhost;
vector <B_CarGhost*> B_CarGhostsss;
class B_CarGhost : public BanKBehavior {
public:
    Transform TargetT;

    std::vector<Keyframe> keyframes;
    size_t currentIndex = 0;
    float Timer = 0;

    GameObj* BODY;
    Model* Model_BODY;

    bool Replay = false;

    B_CarGhost(const std::vector<Keyframe>& keyframes, Model* Model_BODY) : keyframes(keyframes), Model_BODY(Model_BODY)
    {
        B_CarGhostsss.push_back(this);
    }

    void Init() {
        BODY = GameObject->CreateChild();
        BODY->Transform.wPosition = glm::vec3(0, 0.5, 0);
        BODY->Transform.wScale = glm::vec3(1, 1, 1)* Migrate_Scale;
    }

    void Update() {

        if (Input::GetKeyDown(GLFW_KEY_P)) {
            Replay = true;
            Timer = 0;
            currentIndex = 0;
        }

        if (Replay) {
            Timer += Time.Deltatime;

            if (currentIndex >= keyframes.size()) {
                Replay = false;       // End replay when all keyframes are processed
                return;
            }

            // Process keyframes up to the current time
            while (currentIndex < keyframes.size() && keyframes[currentIndex].time <= Timer) {
                Transform T = keyframes[currentIndex].transform;

                TargetT.wPosition = T.wPosition;
                TargetT.wRotation = T.wRotation;
                TargetT.wScale    = T.wScale;

                currentIndex++;
            }

            float AnimSpeed = Time.Deltatime* 8;
            GameObject->Transform.wPosition = B_lerpVec3(GameObject->Transform.wPosition, TargetT.wPosition, AnimSpeed);
            GameObject->Transform.wRotation = B_lerpVec3(GameObject->Transform.wRotation, TargetT.wRotation,1);
            GameObject->Transform.wScale = B_lerpVec3(GameObject->Transform.wScale, TargetT.wScale, AnimSpeed);
        }
    }



    void Render(Shader& shader) {

        shader.setMat4("model", BODY->Transform.modelMatrix);
        shader.setMat4("normalMatrix", glm::transpose(glm::inverse(glm::mat3(BODY->Transform.modelMatrix))));
        Model_BODY->Draw(shader);
    }
};



















bool DoRaycast = true;

class Wheel : public BanKBehavior {
public:
    float Speed = 99;
    void Update() {
        GameObject->Transform.wRotation.x += Speed * Time.Deltatime * 25;
    }
};
Wheel* Edit_Wheel;

class B_Car : public BanKBehavior {
public:

    GameObj* CameraHolder;
    GameObj* CameraLookat;
    glm::vec3 CurrentLookat;
    float EngineRound;

    GameObj* BODY_XZ;
    GameObj* BODY;
    GameObj* RayDown_Front;
    GameObj* RayDown_Back;
    GameObj* RayDown_R;
    GameObj* RayDown_L;
    GameObj* rayColl_Ride;

    Renderer REN_Body;
    Renderer REN_Wheel_F;
    Renderer REN_Wheel_R;
    Model*   Model_BODY;
    Model*   Model_WHEEL;



    struct FrontWheel {
        Wheel* Wheel_L;

        float Angle = 0;
        float AngleMax = 211;
        float AngleHandling = 3.2;
    }FrontWheel;

    struct BackWheel {
        Wheel* Wheel_L;

        float AngularVelocity = 0;
        float AngularVelocityMax = 80;
        float DecelSpeed = 0.32f;
        float Acceleration = 32;
    }BackWheel;

    void Init() {

        BODY_XZ = GameObject->CreateChild();
        /*Edit_Renderer16xM = BODY_XZ->AddComponent(new Renderer16x_Model);
        Edit_Renderer16xM->mModel = LAMBO;
        Edit_Renderer16xM->mMaterial = new B_Materials::LitDiffuseModelLoading;*/
        BODY_XZ->Transform.wPosition = glm::vec3(0, 0.5, 0);
        BODY_XZ->Transform.wScale = glm::vec3(1.5, 1.5, 1.5);

        BODY = BODY_XZ->CreateChild();
        BODY->Transform.wPosition = glm::vec3(0, 0, 0);
        BODY->Transform.wScale = glm::vec3(1, 1, 1)*Migrate_Scale;

        GameObj* FrontWheelOBJ;
        FrontWheelOBJ = BODY_XZ->CreateChild();
        FrontWheel.Wheel_L = FrontWheelOBJ->AddComponent(new Wheel);
        /*Edit_Renderer16xM = FrontWheelOBJ->AddComponent(new Renderer16x_Model);
        Edit_Renderer16xM->mModel = LAMBO_Wheels;
        Edit_Renderer16xM->mMaterial = new B_Materials::LitDiffuseModelLoading;*/
        FrontWheelOBJ->Transform.wPosition = glm::vec3(0, 0.25, 1.16);
        FrontWheelOBJ->Transform.wScale = glm::vec3(0.64, 0.64, 0.64);


        GameObj* BackWheelOBJ;
        BackWheelOBJ = BODY_XZ->CreateChild();
        BackWheel.Wheel_L = BackWheelOBJ->AddComponent(new Wheel);
        //Edit_Renderer16xM = BackWheelOBJ->AddComponent(new Renderer16x_Model);
        //Edit_Renderer16xM->mModel = LAMBO_Wheels;
        //Edit_Renderer16xM->mMaterial = new B_Materials::LitDiffuseModelLoading;
        BackWheelOBJ->Transform.wPosition = glm::vec3(0, 0.25, -1.0);
        BackWheelOBJ->Transform.wScale = glm::vec3(0.64, 0.64, 0.64);


        CameraHolder = BODY_XZ->CreateChild();
        CameraHolder->Transform.wPosition = glm::vec3(0, 2, -8) * 0.64f;

        CameraLookat = BODY_XZ->CreateChild();
        CameraLookat->Transform.wPosition = glm::vec3(0, 0, 30) * 4096.0f;


        RayDown_Front = BODY_XZ->CreateChild();
        RayDown_Front->Transform.wPosition = glm::vec3(0, 0.5, 1);

        RayDown_Back = BODY_XZ->CreateChild();
        RayDown_Back->Transform.wPosition = glm::vec3(0, 0.5, -1);

        RayDown_L = BODY_XZ->CreateChild();
        RayDown_L->Transform.wPosition = glm::vec3(-1, 0.5, 0);

        RayDown_R = BODY_XZ->CreateChild();
        RayDown_R->Transform.wPosition = glm::vec3(1, 0.5, 0);

        rayColl_Ride = BODY_XZ->CreateChild();
        rayColl_Ride->Transform.wPosition = glm::vec3(0, 0.5, 0);


    }

    void Update() {
        if (Input::GetKey(GLFW_KEY_R)) {
            GameObject->Transform.wPosition = glm::vec3(284, 1, -191) * 2.5f;
            GameObject->Transform.wRotation = glm::vec3(0, 100, 0);

            BODY_XZ->Transform.wPosition = glm::vec3(0, 0, 0);
            BODY_XZ->Transform.wRotation = glm::vec3(0, 0, 0);

        }
        if (Input::GetKey(GLFW_KEY_Y)) {
            DoRaycast = true;
        }
        if (Input::GetKey(GLFW_KEY_T)) {
            DoRaycast = false;
        }
         
        //cout << endl << GameObject->Transform.wPosition.x << " " << GameObject->Transform.wPosition.y << " " << GameObject->Transform.wPosition.z << " ";

        if (Input::GetKey(GLFW_KEY_SPACE)) {
            BackWheel.AngularVelocity = B_lerp(BackWheel.AngularVelocity, 0, Time.Deltatime * BackWheel.DecelSpeed * 2);
        }
        else if (Input::GetKey(GLFW_KEY_W)) {
            BackWheel.AngularVelocity += Time.Deltatime * BackWheel.Acceleration;
        }
        else if (Input::GetKey(GLFW_KEY_S)) {
            BackWheel.AngularVelocity -= Time.Deltatime * BackWheel.Acceleration;
        }
        else {
            BackWheel.AngularVelocity = B_lerp(BackWheel.AngularVelocity, 0, Time.Deltatime * BackWheel.DecelSpeed);
        }
        BackWheel.AngularVelocity = B_clamp(BackWheel.AngularVelocity, -BackWheel.AngularVelocityMax, BackWheel.AngularVelocityMax);
        GameObject->Transform.wPosition += BODY_XZ->Transform.getForwardVector() * (Time.Deltatime * BackWheel.AngularVelocity);



        if (Input::GetKey(GLFW_KEY_E)) {
            GameObject->Transform.wPosition += BODY_XZ->Transform.getUpVector() * (Time.Deltatime * 16);
        }
        else if (Input::GetKey(GLFW_KEY_Q)) {
            GameObject->Transform.wPosition -= BODY_XZ->Transform.getUpVector() * (Time.Deltatime * 16);
        }
        if (Input::GetKey(GLFW_KEY_UP)) {
            BODY_XZ->Transform.wRotation.x += (Time.Deltatime * 50);
        }
        else if (Input::GetKey(GLFW_KEY_DOWN)) {
            BODY_XZ->Transform.wRotation.x -= (Time.Deltatime * 50);
        }




        float HandlingFactor = B_clamp(B_normalize(BackWheel.AngularVelocity, BackWheel.AngularVelocityMax * 1.5f, 20), 0, 1);
        if (Input::GetKey(GLFW_KEY_A)) {
            FrontWheel.Angle = B_lerp(FrontWheel.Angle, FrontWheel.AngleMax, Time.Deltatime * FrontWheel.AngleHandling * HandlingFactor);;
        }
        else if (Input::GetKey(GLFW_KEY_D)) {
            FrontWheel.Angle = B_lerp(FrontWheel.Angle, -FrontWheel.AngleMax, Time.Deltatime * FrontWheel.AngleHandling * HandlingFactor);;
        }
        else {
            FrontWheel.Angle = B_lerp(FrontWheel.Angle, 0, Time.Deltatime * FrontWheel.AngleHandling * 1.5f * HandlingFactor);
        }

        float TurnFactor = B_clamp(B_normalize(BackWheel.AngularVelocity, 0, 16), -1, 1);
        float Angle = FrontWheel.Angle * TurnFactor * Time.Deltatime;
        GameObject->Transform.wRotation.y += Angle;

        //GameObject->Transform.modelMatrix = glm::rotate(GameObject->Transform.modelMatrix, Angle, BODY_XZ->Transform.getUpVector());





        FrontWheel.Wheel_L->Speed = BackWheel.AngularVelocity;
        FrontWheel.Wheel_L->GameObject->Transform.wRotation.y = FrontWheel.Angle * 0.16f * HandlingFactor;
        BackWheel.Wheel_L->Speed = BackWheel.AngularVelocity;



        CameraControl();
        AudioUpdate();
        RecordCtrl();
    }


    void CameraControl() {
        /*BanKEngine::GlfwGlad::camera.Position = B_lerpVec3(BanKEngine::GlfwGlad::camera.Position, CameraHolder->Transform.getWorldPosition(), Time.Deltatime * 8);
        CurrentLookat = B_lerpVec3(CurrentLookat, CameraLookat->Transform.getWorldPosition(), Time.Deltatime * 12);
        BanKEngine::GlfwGlad::camera.LookAt(CurrentLookat);
        BanKEngine::GlfwGlad::FOV = B_clamp(B_normalize(BackWheel.AngularVelocity * BackWheel.AngularVelocity * 0.02f, BackWheel.AngularVelocityMax * 0.01f, BackWheel.AngularVelocityMax), 0, 1) * 20 + 35;*/
    }

    float AudioTime = 0;
    irrklang::ISound* Current = nullptr;
    void AudioUpdate() {
        //float AudioWait = 1 / (4 + EngineRound * 0.2f);
        //AudioWait = abs(AudioWait);


        //if (AudioTime > AudioWait) {
        //    AudioTime = 0;
        //    Current = B_Audio.SoundEffects.Play("Car.wav");
        //    Current->setPlaybackSpeed(EngineRound * 0.064f + 1.52f); // Example: Increase pitch by 50%
        //    Current->setVolume(0.5f);
        //}
        //else {
        //    AudioTime += Time.Deltatime;
        //}


        //if (Current) {
        //    if (Input::GetKey([GLFW_KEY_SPACE]) {
        //        EngineRound = B_lerp(EngineRound, 0, Time.Deltatime * 1.0f);
        //    }
        //    else if (Input::GetKey([GLFW_KEY_W]) {
        //        Current->setVolume(1);
        //        EngineRound += Time.Deltatime * BackWheel.Acceleration * 1.72f;
        //    }
        //    else if (Input::GetKey([GLFW_KEY_S]) {
        //        EngineRound = B_lerp(EngineRound, 0, Time.Deltatime * 1.0f);
        //    }
        //    else {
        //        EngineRound = B_lerp(EngineRound, 0, Time.Deltatime * 1.0f);
        //    }
        //    EngineRound = B_clamp(EngineRound, 5, BackWheel.AngularVelocityMax);
        //}

    }



                struct RECm {
                    vector<Keyframe> keyframes;
                    float TimerPlace = 0;
                    float TimerRecord = 0;
                    bool isRecording = false;
                }RECm;
                void RecordCtrl() {

                    if (Input::GetKeyDown(GLFW_KEY_E)) {

                        if (RECm.isRecording) {
                            GameObj* NewGhost = GameObj::Create();
                            NewGhost->AddComponent(new B_CarGhost(RECm.keyframes,Model_BODY));
                            cout << "\nREC completed";
                        }
                        else
                        {
                            cout << "\nREC started";
                        }

                        RECm.isRecording = !RECm.isRecording;
                        RECm.TimerRecord = 0;
                        RECm.keyframes.clear();
                    }

                    if (RECm.isRecording) {

                        RECm.TimerRecord += Time.Deltatime;
                        //cout << endl << RECm.TimerRecord;
                            if (RECm.TimerPlace > KeyframeAwait) {
                                Transform T;
                                T.wPosition = GameObject->Transform.wPosition;
                                T.wScale = GameObject->Transform.wScale;
                                T.wRotation = BODY_XZ->Transform.getWorldRotation();

                                RECm.keyframes.push_back(Keyframe(RECm.TimerRecord, T));
                                RECm.TimerPlace = 0;
                            }
                            else
                            {    RECm.TimerPlace += Time.Deltatime;    }
                    }
                }







     
    void Render(Shader& shader) {

        shader.setMat4("model", BODY->Transform.modelMatrix);
        shader.setMat4("normalMatrix", glm::transpose(glm::inverse(glm::mat3(BODY->Transform.modelMatrix))));
        Model_BODY->Draw(shader);    
    }
};
 

























static bool isNearMe2D(const glm::vec3& Subject, const glm::vec3& Me, float Range)
{
    return (Subject.x > Me.x - Range && Subject.x < Me.x + Range) &&
        (Subject.z > Me.z - Range && Subject.z < Me.z + Range);
}
vector<B_Triangle> Dust2_Triangles;


void Car_Raycast_Init(GameObj* GameObject,GameObj* CarOBJ, Model mModel) {

    ///////////////////////////
    BanKEngine::All_Update();

    //Get ONCE cus the map is STATIC
    glm::vec3 Vert0;
    glm::vec3 Vert1;
    glm::vec3 Vert2;

    Dust2_Triangles.clear();
    const glm::mat4& modelMatrix = GameObject->Transform.modelMatrix;
    const glm::vec3& carPosition = CarOBJ->Transform.wPosition;
    const float maxDistance = 99999999;
    const float maxDistanceSquared = maxDistance * maxDistance;  // Use squared distance to avoid sqrt for performance

    for (Mesh& m : mModel.meshes) {
        for (unsigned int i = 0; i < m.indices.size(); i += 3) {
            B_Triangle NewTriangle;

            Vert0 = getWorldPosition(m.vertices[m.indices[i]].Position, modelMatrix);
            Vert1 = getWorldPosition(m.vertices[m.indices[i + 1]].Position, modelMatrix);
            Vert2 = getWorldPosition(m.vertices[m.indices[i + 2]].Position, modelMatrix);

            NewTriangle.Vert0 = Vert0;
            NewTriangle.Vert1 = Vert1;
            NewTriangle.Vert2 = Vert2;
            Dust2_Triangles.push_back(NewTriangle);
        }
    }

}





void Car_Raycast_Update(GameObj* CarOBJ, B_Car* CarBehav) {





    vector<B_Triangle> Triangle4Cast;
    for (const B_Triangle& trian : Dust2_Triangles) {
        if (isNearMe2D(trian.Vert0, CarOBJ->Transform.wPosition, 25)) {
            Triangle4Cast.push_back(trian);
        }
    }
    bool HIT = false;


    glm::vec3 Vec3_Up = CarBehav->BODY_XZ->Transform.getUpVector();
    glm::vec3 Vec3_Down = -Vec3_Up;
    glm::vec3 Vec3_Fwd = CarBehav->BODY_XZ->Transform.getForwardVector();
    glm::vec3 Vec3_Back = -Vec3_Fwd;
    glm::vec3 Vec3_LFT = CarBehav->BODY_XZ->Transform.getLeftVector();
    glm::vec3 Vec3_RHT = -Vec3_LFT;


    B_Ray rayFront;
    glm::vec3 rayFront_Hitpoint;
    rayFront.Origin = CarBehav->RayDown_Front->Transform.getWorldPosition();
    rayFront.Direction = Vec3_Down;
    if (RayIntersectSceneOptimized(rayFront, Triangle4Cast, rayFront_Hitpoint))
        HIT = true;


    B_Ray rayBack;
    glm::vec3 rayBack_Hitpoint;
    rayBack.Origin = CarBehav->RayDown_Back->Transform.getWorldPosition();
    rayBack.Direction = Vec3_Down;
    if (RayIntersectSceneOptimized(rayBack, Triangle4Cast, rayBack_Hitpoint))
        HIT = true;



    B_Ray rayLeft;
    glm::vec3 rayLeft_Hitpoint;
    rayLeft.Origin = CarBehav->RayDown_L->Transform.getWorldPosition();
    rayLeft.Direction = Vec3_Down;
    if (RayIntersectSceneOptimized(rayLeft, Triangle4Cast, rayLeft_Hitpoint))
        HIT = true;



    B_Ray rayRight;
    glm::vec3 rayRight_Hitpoint;
    rayRight.Origin = CarBehav->RayDown_R->Transform.getWorldPosition();
    rayRight.Direction = Vec3_Down;
    if (RayIntersectSceneOptimized(rayRight, Triangle4Cast, rayRight_Hitpoint))
        HIT = true;


    B_Ray rayColl_R;
    glm::vec3 rayColl_R_Hitpoint;
    rayColl_R.Origin = CarBehav->rayColl_Ride->Transform.getWorldPosition();
    rayColl_R.Direction = Vec3_RHT + Vec3_Fwd * 1.0f + Vec3_Up * 0.16f;
    RayIntersectSceneOptimized(rayColl_R, Triangle4Cast, rayColl_R_Hitpoint);

    B_Ray rayColl_L;
    glm::vec3 rayColl_L_Hitpoint;
    rayColl_L.Origin = CarBehav->rayColl_Ride->Transform.getWorldPosition();
    rayColl_L.Direction = Vec3_LFT + Vec3_Fwd * 1.0f + Vec3_Up * 0.16f;
    RayIntersectSceneOptimized(rayColl_L, Triangle4Cast, rayColl_L_Hitpoint);

    B_Ray rayColl_R_Rear;
    glm::vec3 rayColl_R_Rear_Hitpoint;
    rayColl_R_Rear.Origin = CarBehav->rayColl_Ride->Transform.getWorldPosition();
    rayColl_R_Rear.Direction = Vec3_RHT + Vec3_Back * 1.0f + Vec3_Up * 0.16f;
    RayIntersectSceneOptimized(rayColl_R_Rear, Triangle4Cast, rayColl_R_Rear_Hitpoint);

    B_Ray rayColl_L_Rear;
    glm::vec3 rayColl_L_Rear_Hitpoint;
    rayColl_L_Rear.Origin = CarBehav->rayColl_Ride->Transform.getWorldPosition();
    rayColl_L_Rear.Direction = Vec3_LFT + Vec3_Back * 1.0f + Vec3_Up * 0.16f;
    RayIntersectSceneOptimized(rayColl_L_Rear, Triangle4Cast, rayColl_L_Rear_Hitpoint);




    float DistanceFB = glm::length(rayFront.Origin - rayBack.Origin);
    float backDis = glm::length(rayBack_Hitpoint - rayBack.Origin);
    float frontDis = glm::length(rayFront_Hitpoint - rayFront.Origin);
    float x_yDiff = frontDis - backDis;

    float DistanceLF = glm::length(rayLeft.Origin - rayRight.Origin);
    float leftDis = glm::length(rayLeft_Hitpoint - rayLeft.Origin);
    float rightDis = glm::length(rayRight_Hitpoint - rayRight.Origin);
    float z_yDiff = rightDis - leftDis;

    //std::cout << std::fixed << std::setprecision(2);
    //cout << endl << "|XyDiff| " << x_yDiff << "  Back:" << backDis << " Front:" << frontDis;



    //Hitlocation->Transform.wPosition = rayBack_Hitpoint;
    //HitlocationL->Transform.wPosition = rayFront_Hitpoint;
    if (DoRaycast && HIT) {


        CarOBJ->Transform.wPosition = 0.25f * (rayFront_Hitpoint + rayBack_Hitpoint + rayRight_Hitpoint + rayLeft_Hitpoint);
        CarBehav->BODY_XZ->Transform.wRotation.x += x_yDiff * 20.5f;
        CarBehav->BODY_XZ->Transform.wRotation.z -= z_yDiff * 20.5f;


        //Hitlocation->Transform.wPosition = rayColl_R_Hitpoint;
        //HitlocationL->Transform.wPosition = rayColl_L_Hitpoint;
        if (glm::distance(rayColl_R_Hitpoint, CarOBJ->Transform.wPosition) < 3.2f) {
            CarOBJ->Transform.wPosition -= rayColl_R.Direction * 0.1f;
            CarBehav->BackWheel.AngularVelocity *= 0.9f;
            CarBehav->FrontWheel.Angle += 12;
        }
        else if (glm::distance(rayColl_L_Hitpoint, CarOBJ->Transform.wPosition) < 3.2f) {
            CarOBJ->Transform.wPosition -= rayColl_L.Direction * 0.1f;
            CarBehav->BackWheel.AngularVelocity *= 0.9f;
            CarBehav->FrontWheel.Angle -= 12;
        }
        
        if (glm::distance(rayColl_R_Rear_Hitpoint, CarOBJ->Transform.wPosition) < 3.2f) {
            CarOBJ->Transform.wPosition -= rayColl_R_Rear.Direction * 0.1f;
            CarBehav->BackWheel.AngularVelocity *= 0.9f;
            CarBehav->FrontWheel.Angle -= 12;
        }
        else if (glm::distance(rayColl_L_Rear_Hitpoint, CarOBJ->Transform.wPosition) < 3.2f) {
            CarOBJ->Transform.wPosition -= rayColl_L_Rear.Direction * 0.1f;
            CarBehav->BackWheel.AngularVelocity *= 0.9f;
            CarBehav->FrontWheel.Angle += 12;
        }



    }




}