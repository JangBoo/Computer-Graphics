#include "common/stdafx.h"

//world space position
glm::vec3 BODY_POSITION=glm::vec3(0.0f,2.5f,0.0f);
glm::vec3 NECK_POSITION=glm::vec3(-2.5f,3.0f,0.0f);
glm::vec3 HEAD_POSITION=glm::vec3(-4.0f,3.0f,0.0f);

glm::vec3 FRONT_LEFT_UPPER_POSITION=glm::vec3(-1.5f,1.5f,-0.5f);
glm::vec3 FRONT_LEFT_LOWER_POSITION=glm::vec3(-1.5f,0.5f,-0.5f);
glm::vec3 FRONT_RIGHT_UPPER_POSITION=glm::vec3(-1.5f,1.5f,0.5f);
glm::vec3 FRONT_RIGHT_LOWER_POSITION=glm::vec3(-1.5f,0.5f,0.5f);

glm::vec3 BACK_LEFT_UPPER_POSITION=glm::vec3(1.5f,1.5f,-0.5f);
glm::vec3 BACK_LEFT_LOWER_POSITION=glm::vec3(1.5f,0.5f,-0.5f);
glm::vec3 BACK_RIGHT_UPPER_POSITION=glm::vec3(1.5f,1.5f,0.5f);
glm::vec3 BACK_RIGHT_LOWER_POSITION=glm::vec3(1.5f,0.5f,0.5f);

glm::vec3 BODY_SCALER=glm::vec3(4.0f,1.5f,2.0f);
glm::vec3 NECK_SCALER=glm::vec3(2.5f,1.0f,0.5f);
glm::vec3 HEAD_SCALER=glm::vec3(1.5f,0.5f,0.5f);
glm::vec3 LEG_SCALER=glm::vec3(0.5f,1.0f,0.5f);


enum HorsePartEnum
{
    HEAD, NECK, BODY, FRONT_LEFT_UPPER,FRONT_LEFT_LOWER,FRONT_RIGHT_UPPER,FRONT_RIGHT_LOWER,BACK_LEFT_UPPER,BACK_LEFT_LOWER,BACK_RIGHT_UPPER,BACK_RIGHT_LOWER
};

glm::mat4 getHorseModel(HorsePartEnum horsePart, float rotateRadians, glm::vec3 rotateOrientation, float scaler, float moveOnX, float moveOnZ)
{
    glm::mat4 model=glm::mat4(1.0f);
    model=glm::rotate(model, glm::radians(rotateRadians), rotateOrientation);
    model=glm::scale(model, glm::vec3(scaler,scaler,scaler));
    switch (horsePart){
        case BODY:
            model=glm::translate(model, glm::vec3(BODY_POSITION[0]+moveOnX,BODY_POSITION[1],BODY_POSITION[2]+moveOnZ));
            model=glm::scale(model, BODY_SCALER);
            break;
        case NECK:
            model=glm::translate(model, glm::vec3(NECK_POSITION[0]+moveOnX,NECK_POSITION[1],NECK_POSITION[2]+moveOnZ));
            model=glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f,0.0f,1.0f));
            model=glm::scale(model, NECK_SCALER);
            break;
        case HEAD:
            model=glm::translate(model, glm::vec3(HEAD_POSITION[0]+moveOnX,HEAD_POSITION[1],HEAD_POSITION[2]+moveOnZ));
            model=glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f,0.0f,1.0f));
            model=glm::scale(model, HEAD_SCALER);
            break;
        case FRONT_LEFT_UPPER:
            model=glm::translate(model, glm::vec3(FRONT_LEFT_UPPER_POSITION[0]+moveOnX,FRONT_LEFT_UPPER_POSITION[1],FRONT_LEFT_UPPER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case FRONT_LEFT_LOWER:
            model=glm::translate(model, glm::vec3(FRONT_LEFT_LOWER_POSITION[0]+moveOnX,FRONT_LEFT_LOWER_POSITION[1],FRONT_LEFT_LOWER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case FRONT_RIGHT_UPPER:
            model=glm::translate(model, glm::vec3(FRONT_RIGHT_UPPER_POSITION[0]+moveOnX,FRONT_RIGHT_UPPER_POSITION[1],FRONT_RIGHT_UPPER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case FRONT_RIGHT_LOWER:
            model=glm::translate(model, glm::vec3(FRONT_RIGHT_LOWER_POSITION[0]+moveOnX,FRONT_RIGHT_LOWER_POSITION[1],FRONT_RIGHT_LOWER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case BACK_LEFT_UPPER:
            model=glm::translate(model, glm::vec3(BACK_LEFT_UPPER_POSITION[0]+moveOnX,BACK_LEFT_UPPER_POSITION[1],BACK_LEFT_UPPER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case BACK_LEFT_LOWER:
            model=glm::translate(model, glm::vec3(BACK_LEFT_LOWER_POSITION[0]+moveOnX,BACK_LEFT_LOWER_POSITION[1],BACK_LEFT_LOWER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case BACK_RIGHT_UPPER:
            model=glm::translate(model, glm::vec3(BACK_RIGHT_UPPER_POSITION[0]+moveOnX,BACK_RIGHT_UPPER_POSITION[1],BACK_RIGHT_UPPER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
        case BACK_RIGHT_LOWER:
            model=glm::translate(model, glm::vec3(BACK_RIGHT_LOWER_POSITION[0]+moveOnX,BACK_RIGHT_LOWER_POSITION[1],BACK_RIGHT_LOWER_POSITION[2]+moveOnZ));
            model=glm::scale(model, LEG_SCALER);
            break;
    }
    return model;
}
