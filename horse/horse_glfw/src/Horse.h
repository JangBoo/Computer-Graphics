//----------------------------------------------------------------------------

#define TORSO_HEIGHT 3.0
#define TORSO_WIDTH 10.0
#define TORSO_DEPTH 3.0

#define NECK_HEIGHT 5.0
#define NECK_WIDTH 2.0
#define NECK_DEPTH 2.0

#define UPPER_ARM_HEIGHT 3.0
#define LOWER_ARM_HEIGHT 3.0
#define UPPER_ARM_WIDTH  1.0
#define LOWER_ARM_WIDTH  1.0

#define UPPER_LEG_HEIGHT 3.0
#define LOWER_LEG_HEIGHT 3.0
#define UPPER_LEG_WIDTH  1.0
#define LOWER_LEG_WIDTH  1.0

#define HEAD_HEIGHT 3.0
#define HEAD_WIDTH 1.5
#define HEAD_DEPTH 2.0

/*
class Horse
{
//----------------------------------------------------------------------------

private:
    MatrixStack  mvstack;
    mat4         base_model;
    GLuint       ModelView;
    // 36 vertices for a cube: (6 faces)(2 triangles/face)(3 vertices/triangle)
    const int NumVertices = 36;
public:


// Set up menu item indices, which we can also use with the joint angles
    enum
    {
        Torso = 0,
        Head = 1,
        LeftUpperArm = 2,
        LeftLowerArm = 3,
        RightUpperArm = 4,
        RightLowerArm = 5,
        LeftUpperLeg = 6,
        LeftLowerLeg = 7,
        RightUpperLeg = 8,
        RightLowerLeg = 9,
        Neck = 10,
        NumNodes,
        Quit
    };
// Joint angles with initial values
    GLfloat theta[NumNodes] =
    {
        0.0,	// Torso
        -80.0,	// Head
        180.0,	// LeftUpperArm
        0.0,	// LeftLowerArm
        180.0,	// RightUpperArm
        0.0,	// RightLowerArm
        170.0,	// LeftUpperLeg
        10.0,	// LeftLowerLeg
        170.0,	// RightUpperLeg
        10.0,	// RightLowerLeg
        -45.0	// Neck
    };

    GLfloat target[NumNodes] =
    {
        0.0,	// Torso
        0.0,	// Head
        0.0,	// LeftUpperArm
        0.0,	// LeftLowerArm
        0.0,	// RightUpperArm
        0.0,	// RightLowerArm
        0.0,	// LeftUpperLeg
        0.0,	// LeftLowerLeg
        0.0,	// RightUpperLeg
        0.0,	// RightLowerLeg
        0.0	    // Neck
    };

    GLint angle = Torso;

//----------------------------------------------------------------------------

    Node nodes[NumNodes];
    Horse(GLuint ModelView): ModelView(ModelView)
    {
    }

    void initNodes(void)
    {
        mat4  m;

        m = RotateY(theta[Torso]);
        nodes[Torso] = Node(m, this->torso, NULL, &nodes[Neck]);

        m = Translate(TORSO_WIDTH / 2 - NECK_WIDTH / 2, TORSO_HEIGHT, 0.0) * RotateZ(theta[Neck]);
        nodes[Neck] = Node(m, neck, &nodes[LeftUpperArm], &nodes[Head]);

        m = Translate(0.0, NECK_HEIGHT, 0.0) * RotateZ(theta[Head]);
        nodes[Head] = Node(m, head, NULL, NULL);

        m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_LEG_HEIGHT, -1.5 + UPPER_LEG_WIDTH / 2) * RotateZ(theta[LeftUpperArm]);
        nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

        m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_ARM_HEIGHT, 1.5 - UPPER_ARM_WIDTH / 2) * RotateZ(theta[RightUpperArm]);
        nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

        m = Translate(-(TORSO_WIDTH / 2 - UPPER_ARM_WIDTH / 2), 0.1*UPPER_ARM_HEIGHT, -1.5 + UPPER_ARM_WIDTH / 2) * RotateZ(theta[LeftUpperLeg]);
        nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

        m = Translate(-(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2), 0.1*UPPER_LEG_HEIGHT, 1.5 - UPPER_LEG_WIDTH / 2) * RotateZ(theta[RightUpperLeg]);
        nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

        m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[LeftLowerArm]);
        nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

        m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[RightLowerArm]);
        nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

        m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[LeftLowerLeg]);
        nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

        m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[RightLowerLeg]);
        nodes[RightLowerLeg] = Node(m, right_lower_leg, NULL, NULL);
    }

    void traverse(Node* node)
    {
        if (node == NULL)
        {
            return;
        }

        mvstack.push(base_model);

        base_model *= node->transform;
        node->render();

        if (node->child)
        {
            traverse(node->child);
        }

        base_model = mvstack.pop();

        if (node->sibling)
        {
            traverse(node->sibling);
        }
    }

    void torso()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) * Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void neck()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * NECK_HEIGHT, 0.0) * Scale(NECK_WIDTH, NECK_HEIGHT, NECK_DEPTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void head()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) * Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_DEPTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void left_upper_arm()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void left_lower_arm()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void right_upper_arm()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void right_lower_arm()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void left_upper_leg()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.65f, 0.65f, 0.65f));
        glm::mat4 translate = glm::translate(glm::mat4(0.1f), glm::vec3(0, -1, 0));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void left_lower_leg()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void right_upper_leg()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

    void right_lower_leg()
    {
        mvstack.push(base_model);

        mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

        glUniformMatrix4fv(ModelView, 1, GL_TRUE, base_model * instance);
        glDrawArrays(GL_TRIANGLES, 0, NumVertices);

        base_model = mvstack.pop();
    }

}
*/
